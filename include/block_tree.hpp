/***
BSD 2-Clause License

Copyright (c) 2018, Adrián
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**/


//
// Created by Adrián on 31/07/2019.
//

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_HPP

#define BITMAP_RESIZE 512
#define ARRAY_RESIZE 64
#define BT_VERBOSE 0

#include <block_tree_algorithm_helper.hpp>
#include "alternative_code.hpp"
#include <logger.hpp>
#include <algorithm>
#include <result_util.hpp>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree {

    public:

        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef input_t input_type;
        typedef typename block_tree_2d::algorithm::node_type node_type;
        typedef hash_table::hash_table_chainning<uint64_t , uint64_t> htc_type;

    protected:
        size_type m_k;
        size_type m_k2;
        size_type m_dimensions;
        size_type m_height;
        //sdsl::bit_vector m_topology;
        //sdsl::bit_vector::rank_1_type m_topology_rank;
        //sdsl::rank_support_v5<1> m_topology_rank;
        //sdsl::bit_vector::select_1_type m_topology_select;
        sdsl::bit_vector m_t;
        sdsl::rank_support_v5<> m_t_rank;
        sdsl::bit_vector::select_1_type m_t_select;
        sdsl::bit_vector m_l;
        sdsl::bit_vector m_is_pointer;
        sdsl::bit_vector::rank_1_type m_is_pointer_rank;
        std::vector<sdsl::int_vector<>> m_pointers;
        std::vector<sdsl::int_vector<>> m_offsets;
        sdsl::int_vector<> m_level_ones;

    public:
        const size_type &k = m_k;
        const size_type &height = m_height;
        const size_type &dimensions = m_dimensions;

    protected:

        virtual void copy(const block_tree &p){
            m_k = p.m_k;
            m_k2 = m_k*m_k;
            m_height = p.m_height;
            m_dimensions = p.m_dimensions;
            /*m_topology = p.m_topology;
            m_topology_rank = p.m_topology_rank;
            m_topology_rank.set_vector(&m_topology);
            m_topology_select = p.m_topology_select;
            m_topology_select.set_vector(&m_topology);*/
            m_t = p.m_t;
            m_t_rank = p.m_t_rank;
            m_t_rank.set_vector(&m_t);
            m_t_select = p.m_t_select;
            m_t_select.set_vector(&m_t);
            m_l = p.m_l;
            m_is_pointer = p.m_is_pointer;
            m_is_pointer_rank = p.m_is_pointer_rank;
            m_is_pointer_rank.set_vector(&m_is_pointer);
            m_level_ones = p.m_level_ones;
            m_pointers = p.m_pointers;
            m_offsets = p.m_offsets;
        }


        inline uint64_t compute_dimensions(input_type &adjacency_lists){
            uint64_t d = adjacency_lists.size()-1;
            for(size_type r = 0; r < adjacency_lists.size(); ++r){
                for(size_type c = 0; c < adjacency_lists[r].size(); ++c){
                    if(d < adjacency_lists[r][c]){
                        d =  adjacency_lists[r][c];
                    }
                }
            }
            return d+1;
        }

        inline void init_construction(size_type &h, size_type &total_size, input_type &adjacency_lists,
                                      const size_type kparam){
            m_k = kparam;
            m_dimensions = compute_dimensions(adjacency_lists);
            m_k2 = m_k*m_k;
            h = (size_type) std::ceil(std::log(m_dimensions)/std::log(m_k));
            total_size = (size_type) std::pow(m_k, h);
            if(adjacency_lists.size() < total_size){
                adjacency_lists.resize(total_size);
            }
        }

        /*virtual void construction(input_type &adjacency_lists, size_type h, size_type block_size, size_type blocks){
            std::vector<node_type> nodes(blocks);
            //Init map between z_order and position in vector nodes
            block_tree_2d::algorithm::hash_type hash;
            for(uint64_t i = 0; i < blocks; ++i){
                hash.insert({i,i});
            }
            size_type level = 0, topology_index = m_k2, leaves_index = 0, is_pointer_index = 0;
            //std::cout << "Level: " << level << std::endl;
            init_structure();
            //m_topology[0]=1;
            m_t[0] = 1;
            m_offsets[0].resize(0);
            m_pointers[0].resize(0);
            while (block_size > 1) {
                ++level;
                util::logger::log("Processing level " + std::to_string(level) + " of " + std::to_string(h));
                util::logger::log("Block size: " + std::to_string(block_size));
                auto pointers = processing_level(level, adjacency_lists, block_size, topology_index, is_pointer_index, hash, nodes);
                util::logger::log("Pointers: " + std::to_string(pointers));
                block_size = block_size / k;

            }
            m_t.resize(topology_index);
            ++level;
            util::logger::log("Processing last level (" + std::to_string(level) + ")");
            block_tree_2d::algorithm::compute_last_level(adjacency_lists, hash, nodes);
            util::logger::log("Compacting last level (" + std::to_string(level) + ")");
            compact_last_level(nodes, leaves_index);
            m_height = level;
            m_l.resize(leaves_index);
            //m_topology.resize(topology_index);
            m_is_pointer.resize(is_pointer_index);
            m_level_ones.resize(2*m_height);
            sdsl::util::init_support(m_t_rank, &m_t);
            sdsl::util::init_support(m_t_select, &m_t);
            sdsl::util::init_support(m_is_pointer_rank, &m_is_pointer);
            sdsl::util::bit_compress(m_level_ones);
            util::logger::log("2D Block Tree DONE!!!");
        }*/

        template <class Container>
        inline void check_resize(Container&& cont, const size_type pos) {
            if(pos >= cont.size()){
                cont.resize(cont.size()*2);
            }
        }

        inline void init_structure(){
            std::cout << "bitmap resize: " << BITMAP_RESIZE << std::endl;
            std::cout << "array resize: " << ARRAY_RESIZE << std::endl;
            m_is_pointer = sdsl::bit_vector(BITMAP_RESIZE);
            std::cout << "is_pointer" << std::endl;
            //m_topology = sdsl::bit_vector(BITMAP_RESIZE, 0);
            m_t = sdsl::bit_vector(BITMAP_RESIZE, 0);
            m_l = sdsl::bit_vector(BITMAP_RESIZE, 0);
            std::cout << "topology" << std::endl;
            m_level_ones = sdsl::int_vector<>(ARRAY_RESIZE, 0);
            std::cout << "level_ones" << std::endl;
            m_pointers = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(0, 0));
            std::cout << "pointers" << std::endl;
            m_offsets = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(0, 0));
            std::cout << "init structure done" << std::endl;
        }

        void add_new_pointers_offsets(){
            m_offsets.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
            m_pointers.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }


        virtual size_type compact_current_level(const std::vector<node_type> &nodes, const size_type level,
                                   size_type &topology_index, size_type &is_pointer_index) {
            size_type offset_index = 0, pointer_index = 0;
            add_new_pointers_offsets();
            check_resize(m_level_ones, 2*level+1);
            m_level_ones[2*level] = topology_index; //number of elements up to the current level (excluded)
            m_level_ones[2*level+1] = m_level_ones[2*(level-1)+1]; //number of pointers up to the current level (included)
            size_type pointers = 0;
            for(const auto &node : nodes) {
                //Check sizes
                //check_resize(m_topology, topology_index);
                check_resize(m_t, topology_index);
                check_resize(m_is_pointer, is_pointer_index);
                if(node.type == NODE_INTERNAL){
                    m_t[topology_index++] = 1;
                }else if (node.type == NODE_EMPTY){
                    m_t[topology_index++] = 0;
                    m_is_pointer[is_pointer_index++] = 0;
                }else {
                    m_t[topology_index++] = 0;
                    m_is_pointer[is_pointer_index++] = 1;
                    ++m_level_ones[2*level+1];
                    check_resize(m_offsets[level], offset_index+1);
                    check_resize(m_pointers[level], pointer_index);
                    m_offsets[level][offset_index++] = codes::alternative_code::encode(node.offset_x);
                    m_offsets[level][offset_index++] = codes::alternative_code::encode(node.offset_y);
                    m_pointers[level][pointer_index++] = node.ptr;
                    ++pointers;
                }
            }
            m_offsets[level].resize(offset_index);
            m_pointers[level].resize(pointer_index);
            sdsl::util::bit_compress(m_offsets[level]);
            sdsl::util::bit_compress(m_pointers[level]);
            return pointers;
        }

        void compact_last_level(const std::vector<node_type> &nodes, size_type &leaves_index) {

            for(const auto &node : nodes) {
                //Check sizes
                check_resize(m_l, leaves_index);
                if(node.type == NODE_INTERNAL){
                    m_l[leaves_index++] = 1;
                }else if (node.type == NODE_EMPTY){
                    m_l[leaves_index++] = 0;
                }
            }
        }



        template <class add_function, class result_type>
        void recursive_access_region(const size_type min_x, const size_type max_x, const size_type min_y, const size_type max_y,
                                     const size_type x, const size_type y, const size_type idx, const size_type level,
                                     const size_type block_size, result_type &result, add_function add,
                                     const bool taking_pointer=false, const size_type level_taking_pointer = 0){

#if BT_VERBOSE
            std::cout << "at position: " << idx << std::endl;
            std::cout << "x=" << x << " y=" << y << std::endl;
            std::cout << "block_size: " << block_size << std::endl;
            std::cout << "current_level: " << level << std::endl;
            std::cout << std::endl;
#endif
            if(level == m_height){
                //if(m_topology[idx]){
                if(m_l[idx - m_t.size()]){
                    //Adding result
                    add(result, x, y);
                    //result[y].push_back(x);
                }
            }else{
                //if(m_topology[idx]){
                if(m_t[idx]){
                    size_type new_min_x, new_max_x, new_min_y, new_max_y;
                    //size_type start_children =  m_topology_rank(idx + 1) * m_k2;
                    size_type start_children =  m_t_rank(idx + 1) * m_k2;
                    size_type new_block_size = block_size / m_k;
                    size_type disp_x = 0;
                    for (size_type i = min_x / new_block_size; i <= max_x / new_block_size; i++) {
                        new_min_x = 0;
                        if (i == min_x / new_block_size) {
                            new_min_x = min_x % new_block_size;
                        }
                        new_max_x = new_block_size - 1;
                        if (i == max_x / new_block_size) {
                            new_max_x = max_x % new_block_size;
                        }
                        size_type disp_y = 0;
                        for (size_type j = min_y / new_block_size; j <= max_y / new_block_size; j++) {
                            new_min_y = 0;
                            if (j == min_y / new_block_size) {
                                new_min_y = min_y % new_block_size;
                            }
                            new_max_y = new_block_size - 1;
                            if (j == max_y / new_block_size) {
                                new_max_y = max_y % new_block_size;
                            }
                            recursive_access_region(new_min_x, new_max_x, new_min_y, new_max_y,
                                                    x + (new_block_size * i - min_x) * disp_x,
                                                    y + (new_block_size * j - min_y) * disp_y,
                                                    start_children + codes::zeta_order::encode(i, j),
                                                    level + 1,
                                                    new_block_size, result, add, taking_pointer, level_taking_pointer);
                            disp_y = 1;
                        }
                        disp_x = 1;
                    }
                }else if (taking_pointer_condition(taking_pointer, level_taking_pointer, level)){
                    size_type pos_zero = idx_leaf(idx);
                    if(idx > 0 && m_is_pointer[pos_zero]){
                        value_type offset_x, offset_y;
                        size_type pointer;
                        leaf_node_info(pos_zero, level, pointer, offset_x, offset_y);
                        take_pointer(min_x, max_x, min_y, max_y, x, y, pointer,
                                offset_x, offset_y, level, block_size, result, add);
                    }
                }
            }
        }


        virtual inline bool taking_pointer_condition(const bool taking_pointer, const size_type level_taking_pointer, const size_type level){
            return !(taking_pointer && level <= level_taking_pointer);
        }

        virtual inline size_type idx_leaf(const size_type idx){
            //return idx - m_topology_rank(idx+1) - (m_k2-1);
            return idx - m_t_rank(idx+1) - (m_k2-1);
        }

        virtual void leaf_node_info(const size_type pos_zero, const size_type level,
                            size_type &pointer, value_type &offset_x, value_type &offset_y){
            size_type p_pointer = m_is_pointer_rank(pos_zero+1)- m_level_ones[2*(level-1)+1] -1;
            pointer = m_level_ones[2*level]+ m_pointers[level][p_pointer];
            offset_x = codes::alternative_code::decode(m_offsets[level][2*p_pointer]);
            offset_y = codes::alternative_code::decode(m_offsets[level][2*p_pointer+1]);
        }

        template <class result_type, class add_function>
        void take_pointer(const size_type min_x, const size_type  max_x,
                          const size_type min_y, const size_type max_y,
                          const size_type x, const size_type y,
                          size_type ptr,
                          const value_type offset_x, const value_type offset_y,
                          const size_type level, size_type block_size, result_type &result, add_function add){

            value_type new_min_x = min_x + offset_x;
            value_type new_min_y = min_y + offset_y;
            value_type length_x = max_x - min_x;
            value_type length_y = max_y - min_y;

            size_type steps = 0;
            auto l = level;
            while(new_min_x < 0 || new_min_y < 0 || new_min_x + length_x >= block_size || new_min_y + length_y >= block_size){
                auto zth = ptr % m_k2;
                auto p = codes::zeta_order::decode(zth);
                new_min_x += p.first * block_size;
                new_min_y += p.second * block_size;
                block_size *= m_k;
                l--;
                ptr = m_t_select(ptr / m_k2);
            }
            recursive_access_region(static_cast<size_type >(new_min_x),
                                    static_cast<size_type >(new_min_x + length_x),
                                    static_cast<size_type >(new_min_y),
                                    static_cast<size_type >(new_min_y + length_y), x, y, ptr, l, block_size, result, add, true, level);
        }

        template<class Container>
        void print_container(const Container &cont, std::string name){
            std::cout << name << ": [";
            for(size_type i = 0; i < cont.size(); ++i){
                std::cout << " " << cont[i] << ", ";
            }
            std::cout << "]" << std::endl;
        }

        //returns number of pointers
     /*   size_type processing_level(const size_type level, input_type &adjacency_lists, size_type block_size,
                            size_type &topology_index, size_type &is_pointer_index,
                            typename block_tree_2d::algorithm::hash_type &hash, std::vector<node_type> &nodes){

            //htc_type m_htc(2*nodes.size()); //2* nodes, in order to reduce resize operations
            htc_type m_htc(std::min(10240UL, 2*nodes.size()));
            util::logger::log("Computing fingerprint of blocks at level=" + std::to_string(level));
            block_tree_2d::algorithm::get_fingerprint_blocks_stack_lite(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
            util::logger::log("Computing fingerprint of shifts at level=" + std::to_string(level));
            block_tree_2d::algorithm::get_type_of_nodes_stack_lite(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
            util::logger::log("Clearing adjacency lists at level=" + std::to_string(level));
            block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
            util::logger::log("Compacting level=" + std::to_string(level));
            auto pointers = compact_current_level(nodes, level, topology_index, is_pointer_index);
            util::logger::log("Number of new pointers=" + std::to_string(pointers));
            util::logger::log("Preparing next level");
            block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, m_k2, nodes);
            return pointers;
            //std::cout << "Level: " << level << std::endl;
        }*/

    public:

        block_tree() = default;

        //Pre: adjacency_lists have to contain at least one element
        block_tree(input_type &adjacency_lists, const size_type kparam){

            size_type h, total_size;
            init_construction(h, total_size, adjacency_lists, kparam);
            size_type blocks = m_k2, block_size = total_size/m_k;

        }


        void print(){
            //print_container(m_topology, "topology");
            print_container(m_t, "t");
            print_container(m_l, "l");
            print_container(m_is_pointer, "is_pointer");
            print_container(m_level_ones, "level_ones");
            for(auto i = 0; i < m_pointers.size(); ++i){
                std::string name = "pointers[" + std::to_string(i) + "]";
                print_container(m_pointers[i], name);
            }
            for(auto i = 0; i < m_offsets.size(); ++i){
                std::string name = "offsets[" + std::to_string(i) + "]";
                print_container(m_offsets[i], name);
            }
        }

        size_type first_level_with_pointer(){
            for(size_type l = 0; l < m_pointers.size(); ++l){
                if(!m_pointers[l].empty()){
                    
                    std::cout << "pointer: " << m_pointers[l][0] << std::endl;
                    std::cout << "offset_x: " << codes::alternative_code::decode(m_offsets[l][0]) << std::endl;
                    std::cout << "offset_y: " << codes::alternative_code::decode(m_offsets[l][1]) << std::endl;
                    return l;
                }
            }
        }

        inline std::vector<size_type> access(const size_type id, const size_type direct_id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(direct_id, direct_id, id, id, 0, 0, 0, 0, block_size, r, add_in_row());
            return r;
        }

        inline void access_region(const size_type min_x, const size_type min_y,
                           const size_type max_x, const size_type max_y,
                           input_type &result){
            size_type size_vector = max_y - min_y+1;
            result = input_type(size_vector);
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(min_x, max_x, min_y, max_y, 0, 0, 0, 0, block_size, result, add_in_region());

        }

        inline std::vector<size_type> neigh(size_type id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(0, this->m_dimensions-1, id, id, 0, 0, 0, 0, block_size, r, add_in_row());
            return r;
        }

        inline std::vector<size_type> reverse_neigh(size_type id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(id, id, 0, this->m_dimensions-1, 0, 0, 0, 0, block_size, r, add_in_column());
            return r;
        }

        //! Copy constructor
        block_tree(const block_tree &p) {
            copy(p);
        }

        //! Move constructor
        block_tree(block_tree &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        block_tree &operator=(block_tree &&p) {
            if (this != &p) {
                m_k = std::move(p.m_k);
                m_k2 = m_k * m_k;
                m_height = std::move(p.m_height);
                m_dimensions = std::move(p.m_dimensions);
               /* m_topology = std::move(p.m_topology);
                m_topology_rank = std::move(p.m_topology_rank);
                m_topology_rank.set_vector(&m_topology);
                m_topology_select = std::move(p.m_topology_select);
                m_topology_select.set_vector(&m_topology);*/
                m_t = std::move(p.m_t);
                m_t_rank = std::move(p.m_t_rank);
                m_t_rank.set_vector(&m_t);
                m_t_select = std::move(p.m_t_select);
                m_t_select.set_vector(&m_t);
                m_l = std::move(p.m_l);
                m_is_pointer = std::move(p.m_is_pointer);
                m_is_pointer_rank = std::move(p.m_is_pointer_rank);
                m_is_pointer_rank.set_vector(&m_is_pointer);
                m_level_ones = std::move(p.m_level_ones);
                m_pointers = std::move(p.m_pointers);
                m_offsets = std::move(p.m_offsets);

            }
            return *this;
        }

        //! Assignment operator
        block_tree &operator=(const block_tree &p) {
            if (this != &p) {
                copy(p);
            }
            return *this;
        }

        //! Swap method
        /*! Swaps the content of the two data structure.
        *  You have to use set_vector to adjust the supported bit_vector.
        *  \param bp_support Object which is swapped.
        */
        void swap(block_tree &p) {
            std::swap(m_k, p.m_k);
            std::swap(m_k2, p.m_k2);
            std::swap(m_dimensions, p.m_dimensions);
            std::swap(m_level_ones, p.m_level_ones);
            std::swap(m_height, p.m_height);
            /*std::swap(m_topology, p.m_topology);
            sdsl::util::swap_support(m_topology_rank, p.m_topology_rank, &m_topology, &(p.m_topology));
            sdsl::util::swap_support(m_topology_select, p.m_topology_select, &m_topology, &(p.m_topology));*/
            std::swap(m_t, p.m_t);
            sdsl::util::swap_support(m_t_rank, p.m_t_rank, &m_t, &(p.m_t));
            sdsl::util::swap_support(m_t_select, p.m_t_select, &m_t, &(p.m_t));
            std::swap(m_l, p.m_l);
            std::swap(m_is_pointer, p.m_is_pointer);
            sdsl::util::swap_support(m_is_pointer_rank, p.m_is_pointer_rank, &m_is_pointer, &(p.m_is_pointer));
            std::swap(m_pointers, p.m_pointers);
            std::swap(m_offsets, p.m_offsets);
        }

        //! Serializes the data structure into the given ostream
        virtual size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, const std::string name="")const
        {
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(
                    v, name, sdsl::util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += sdsl::write_member(m_k, out, child, "k");
            written_bytes += sdsl::write_member(m_height, out, child, "height");
            written_bytes += sdsl::write_member(m_dimensions, out, child, "dimensions");
            //written_bytes += m_topology.serialize(out, child, "topology");
            written_bytes += m_t.serialize(out, child, "t");
           // std::cout << "Bytes after t: "<<  written_bytes << std::endl;
            // written_bytes += m_topology_rank.serialize(out, child, "topology_rank");
            written_bytes += m_t_rank.serialize(out, child, "t_rank");
            //std::cout << "Bytes after t_rank: "<<  written_bytes << std::endl;
            //written_bytes += m_topology_select.serialize(out, child, "topology_select");
            written_bytes += m_t_select.serialize(out, child, "t_select");
            //std::cout << "Bytes after t_select: "<<  written_bytes << std::endl;
            written_bytes += m_l.serialize(out, child, "l");
            //std::cout << "Bytes after l: "<<  written_bytes << std::endl;
            written_bytes += m_level_ones.serialize(out, child, "level_ones");
            //std::cout << "level_ones_size: " << m_level_ones.size() << std::endl;
            //std::cout << "Bytes after level_ones: "<<  written_bytes << std::endl;
            written_bytes += m_is_pointer.serialize(out, child, "is_pointer");
            //std::cout << "Bytes after is_pointer: "<<  written_bytes << std::endl;
            written_bytes += m_is_pointer_rank.serialize(out, child, "is_pointer_rank");
            uint64_t m_pointers_size = m_pointers.size();
            //std::cout << "pointers_size: " << m_pointers_size << std::endl;
            sdsl::write_member(m_pointers_size, out, child, "pointers_size");
            written_bytes += sdsl::serialize_vector(m_pointers, out, child, "pointers");
            //std::cout << "Bytes pointers: "<<  written_bytes << std::endl;
            uint64_t m_offsets_size = m_offsets.size();
            //std::cout << "pointers_size: " << m_offsets_size << std::endl;
            sdsl::write_member(m_offsets_size, out, child, "offsets_size");
            written_bytes += sdsl::serialize_vector(m_offsets, out, child, "offsets");
            //std::cout << "Bytes offsets: "<<  written_bytes << std::endl;
            sdsl::structure_tree::add_size(child, written_bytes);
            return written_bytes;
        }

        //! Loads the data structure from the given istream.
        virtual void load(std::istream& in)
        {
            sdsl::read_member(m_k, in);
            sdsl::read_member(m_height, in);
            sdsl::read_member(m_dimensions, in);
            /*m_topology.load(in);
            m_topology_rank.load(in);
            m_topology_rank.set_vector(&m_topology);
            m_topology_select.load(in);
            m_topology_select.set_vector(&m_topology);*/
            m_t.load(in);
            m_t_rank.load(in, &m_t);
            m_t_select.load(in, &m_t);
            m_l.load(in);
            m_level_ones.load(in);
            m_is_pointer.load(in);
            m_is_pointer_rank.load(in, &m_is_pointer);
            uint64_t m_pointers_size = 0;
            sdsl::read_member(m_pointers_size, in);
            m_pointers.resize(m_pointers_size);
            sdsl::load_vector(m_pointers, in);
            uint64_t m_offsets_size = 0;
            sdsl::read_member(m_offsets_size, in);
            m_offsets.resize(m_offsets_size);
            sdsl::load_vector(m_offsets, in);
            m_k2 = m_k * m_k;
        }


        virtual bool is_pointer(size_type idx, size_type level, size_type &pos_zero){
            pos_zero= this->idx_leaf(idx);
            return (idx > 0 && m_is_pointer[pos_zero]);
        }

        std::string node_to_json(size_type id, size_type z_order, bool is_leaf, bool is_ptr, bool is_explicit, size_type level,
                                 size_type x, size_type y, size_type x_i, size_type y_i,
                                 size_type ptr = 0, value_type off_x = 0, value_type off_y = 0){

            std::string json = " { id=" + std::to_string(id) + ", z_order=" + std::to_string(z_order) +
                                ", level=" + std::to_string(level) + ", ";
            if(is_leaf){
                if(is_ptr){
                    json += "type='POINTER', ";
                    json += "id_ptr=" + std::to_string(ptr) + ", ";
                    json += "off_x=" + std::to_string(off_x) + ", ";
                    json += "off_y=" + std::to_string(off_y) + ", ";
                }else if (is_explicit) {
                    json += "type='EXPLICIT', ";
                }else{
                    json += "type='EMPTY', ";
                }
            }else{
                json += "type='INTERNAL', ";
            }
            json += "min_x= " + std::to_string(x) + ", max_x= " + std::to_string(x_i) + ", ";
            json += "min_y= " + std::to_string(y) + ", max_y= " + std::to_string(y_i) + " }";
            return json;

        }


        void display(){

            std::ofstream out("block_tree.json");
            typedef std::unordered_map<size_type, size_type > map_z_order_type;
            size_type start = m_k2, level = 0, elements = m_k2;
            size_type block_size = std::pow(2, m_height) / m_k;
            map_z_order_type map_z_order, next_map_z_order;
            for(size_type i = 0; i < m_k2; ++i){
                map_z_order.insert({i, i});
            }
            while(level < m_height){
                ++level;
                std::cout << "At level: " << level << " and block_size: " << block_size << std::endl;
                size_type ones = 0;
                size_type i = start;
                size_type new_elements = 0;
                while(i < start + elements){
                    if(m_t[i]){
                        auto z_order = map_z_order.find(i-start)->second;
                        auto pos = codes::zeta_order::decode(z_order);
                        auto x = pos.first * block_size;
                        auto y = pos.second * block_size;
                        auto x_i = x + block_size-1;
                        auto y_i = y + block_size-1;
                        for(auto z_next = z_order * m_k2; z_next < (z_order+1)*m_k2; ++z_next){
                            next_map_z_order.insert({new_elements, z_next});
                            ++new_elements;
                        }
                        out << node_to_json(i, z_order, false, false, false, level, x, y, x_i, y_i) << std::endl;
                        ++ones;
                    }else{
                        size_type pos_zero;
                        if(level < m_height && this->is_pointer(i, level, pos_zero)){
                            auto z_order = map_z_order.find(i-start)->second;
                            auto pos = codes::zeta_order::decode(z_order);
                            auto x = pos.first * block_size;
                            auto y = pos.second * block_size;
                            auto x_i = x + block_size-1;
                            auto y_i = y + block_size-1;
                            size_type ptr;
                            value_type off_x, off_y;
                            this->leaf_node_info(pos_zero, level, ptr, off_x, off_y);
                            out << node_to_json(i, z_order, true, true, false, level, x, y, x_i, y_i, ptr, off_x, off_y) << std::endl;
                        }else{
                            auto z_order = map_z_order.find(i-start)->second;
                            auto pos = codes::zeta_order::decode(z_order);
                            auto x = pos.first * block_size;
                            auto y = pos.second * block_size;
                            auto x_i = x + block_size-1;
                            auto y_i = y + block_size-1;
                            out << node_to_json(i, z_order, true, false, false, level, x, y, x_i, y_i) << std::endl;
                        };
                    }
                    ++i;
                }
                start = i;
                elements = ones*m_k2;
                block_size = block_size/m_k;
                map_z_order.clear();
                std::swap(map_z_order, next_map_z_order);
            }
            out.close();


        }

    };

}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_HPP
