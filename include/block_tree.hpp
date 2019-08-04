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

#include <block_tree_algorithm_helper_v2.hpp>
#include "alternative_code.hpp"

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree {

    public:

        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef input_t input_type;
        typedef typename block_tree_2d::algorithm::node_type node_type;
        typedef hash_table::hash_table_chainning<uint64_t , uint64_t> htc_type;

    private:
        size_type m_k;
        size_type m_k2;
        size_type m_dimensions;
        size_type m_height;
        sdsl::bit_vector m_topology;
        sdsl::bit_vector::rank_1_type m_topology_rank;
        sdsl::bit_vector::select_1_type m_topology_select;
        sdsl::bit_vector m_is_pointer;
        sdsl::bit_vector::rank_1_type m_is_pointer_rank;
        std::vector<sdsl::int_vector<>> m_pointers;
        std::vector<sdsl::int_vector<>> m_offsets;
        sdsl::int_vector<> m_level_ones;

    public:
        const size_type &k = m_k;
        const size_type &height = m_height;

    private:

        void copy(const block_tree &p){
            m_k = p.m_k;
            m_k2 = m_k*m_k;
            m_height = p.m_height;
            m_dimensions = p.m_dimensions;
            m_topology = p.m_topology;
            m_topology_rank = p.m_topology_rank;
            m_topology_rank.set_vector(&m_topology);
            m_topology_select = p.m_topology_select;
            m_topology_select.set_vector(&m_topology);
            m_is_pointer = p.m_is_pointer;
            m_is_pointer_rank = p.m_is_pointer_rank;
            m_is_pointer_rank.set_vector(&m_is_pointer);
            m_level_ones = p.m_level_ones;
            m_pointers = p.m_pointers;
            m_offsets = p.m_offsets;
        }


        template <class Container>
        inline void check_resize(Container&& cont, const size_type pos) {
            if(pos >= cont.size()){
                cont.resize(cont.size()*2);
            }
        }

        inline void init_structure(){
            m_topology = sdsl::bit_vector(BITMAP_RESIZE, 0);
            m_is_pointer = sdsl::bit_vector(BITMAP_RESIZE);
            m_level_ones = sdsl::int_vector<>(ARRAY_RESIZE);
            m_pointers = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(ARRAY_RESIZE, 0));
            m_offsets = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }

        void add_new_pointers_offsets(){
            m_offsets.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
            m_pointers.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }


        void compact_current_level(const std::vector<node_type> &nodes, const size_type level,
                                   size_type &topology_index, size_type &is_pointer_index) {
            size_type offset_index = 0, pointer_index = 0;
            add_new_pointers_offsets();
            check_resize(m_level_ones, 2*level+1);
            m_level_ones[2*level] = topology_index; //number of elements up to the current level (excluded)
            m_level_ones[2*level+1] = m_level_ones[2*(level-1)+1]; //number of pointers up to the current level (included)
            for(const auto &node : nodes) {
                //Check sizes
                check_resize(m_topology, topology_index);
                check_resize(m_is_pointer, is_pointer_index);
                if(node.type == NODE_INTERNAL){
                    m_topology[topology_index++] = 1;
                }else if (node.type == NODE_EMPTY){
                    m_topology[topology_index++] = 0;
                    m_is_pointer[is_pointer_index++] = 0;
                }else {
                    m_topology[topology_index++] = 0;
                    m_is_pointer[is_pointer_index++] = 1;
                    ++m_level_ones[2*level+1];
                    check_resize(m_offsets[level], offset_index+1);
                    check_resize(m_pointers[level], pointer_index);
                    m_offsets[level][offset_index++] = codes::alternative_code::encode(node.offset_x);
                    m_offsets[level][offset_index++] = codes::alternative_code::encode(node.offset_y);
                    m_pointers[level][pointer_index++] = node.ptr;
                }
            }
            m_offsets[level].resize(offset_index);
            m_pointers[level].resize(pointer_index);
            sdsl::util::bit_compress(m_offsets[level]);
            sdsl::util::bit_compress(m_pointers[level]);
        }

        void compact_last_level(const std::vector<node_type> &nodes, size_type &topology_index) {
            for(const auto &node : nodes) {
                //Check sizes
                check_resize(m_topology, topology_index);
                if(node.type == NODE_INTERNAL){
                    m_topology[topology_index++] = 1;
                }else if (node.type == NODE_EMPTY){
                    m_topology[topology_index++] = 0;
                }
            }
        }

        void recursive_access_region(const size_type min_x, const size_type max_x, const size_type min_y, const size_type max_y,
                                     const size_type x, const size_type y, const size_type idx, const size_type level,
                                     const size_type block_size, input_type &result,
                                     const bool taking_pointer=false, const size_type level_taking_pointer = 0){

#if BT_VERBOSE
            std::cout << "at position: " << idx << std::endl;
            std::cout << "x=" << x << " y=" << y << std::endl;
            std::cout << "block_size: " << block_size << std::endl;
            std::cout << "current_level: " << level << std::endl;
            std::cout << "height: " << height << std::endl;
            std::cout << std::endl;
#endif
            if(level == m_height){
                if(m_topology[idx]){
                    //Adding result
                    result[y].push_back(x);
                }
            }else{
                if(m_topology[idx]){
                    size_type new_min_x, new_max_x, new_min_y, new_max_y;
                    size_type start_children =  m_topology_rank(idx + 1) * m_k2;
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
                                                    new_block_size, result, taking_pointer, level_taking_pointer);
                            disp_y = 1;
                        }
                        disp_x = 1;
                    }
                }else if (!(taking_pointer && level <= level_taking_pointer)){
                    size_type pos_zero = idx - m_topology_rank(idx+1) - (m_k2-1);
                    if(idx > 0 && m_is_pointer[pos_zero]){
                        value_type offset_x, offset_y;
                        size_type pointer;
                        leaf_node_info(pos_zero, level, pointer, offset_x, offset_y);
                        take_pointer(min_x, max_x, min_y, max_y, x, y, pointer,
                                offset_x, offset_y, level, block_size, result);
                    }
                }
            }
        }


        void leaf_node_info(const size_type pos_zero, const size_type level,
                            size_type &pointer, value_type &offset_x, value_type &offset_y){
            size_type p_pointer = m_is_pointer_rank(pos_zero+1)- m_level_ones[2*(level-1)+1] -1;
            pointer = m_level_ones[2*level]+ m_pointers[level][p_pointer];
            offset_x = codes::alternative_code::decode(m_offsets[level][2*p_pointer]);
            offset_y = codes::alternative_code::decode(m_offsets[level][2*p_pointer+1]);
        }

        void take_pointer(const size_type min_x, const size_type  max_x,
                          const size_type min_y, const size_type max_y,
                          const size_type x, const size_type y,
                          size_type ptr,
                          const value_type offset_x, const value_type offset_y,
                          const size_type level, size_type block_size, input_type &result){

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
                ptr = m_topology_select(ptr / m_k2);
            }
            recursive_access_region(static_cast<size_type >(new_min_x),
                                    static_cast<size_type >(new_min_x + length_x),
                                    static_cast<size_type >(new_min_y),
                                    static_cast<size_type >(new_min_y + length_y), x, y, ptr, l, block_size, result, true, level);
        }

        template<class Container>
        void print_container(const Container &cont, std::string name){
            std::cout << name << ": [";
            for(size_type i = 0; i < cont.size(); ++i){
                std::cout << " " << cont[i] << ", ";
            }
            std::cout << "]" << std::endl;
        }

    public:

        const size_type &dimensions = m_dimensions;
        //Pre: adjacency_lists have to contain at least one 1
        block_tree(input_type &adjacency_lists, const size_type d, const size_type kparam){
            m_k = kparam;
            m_dimensions = d;
            m_k2 = m_k*m_k;
            auto h = std::ceil(std::log(m_dimensions)/std::log(m_k));
            size_type blocks = m_k2;
            auto total_size = (size_type) std::pow(m_k, h);
            if(adjacency_lists.size() < total_size){
                adjacency_lists.resize(total_size);
            }
            size_type block_size = total_size/m_k;
            std::vector<node_type> nodes(blocks);
            //Init map between z_order and position in vector nodes
            block_tree_2d::algorithm::hash_type hash;
            for(uint64_t i = 0; i < m_k2; ++i){
                hash.insert({i,i});
            }
            size_type level = 0, topology_index = m_k2, is_pointer_index = 0;
            //std::cout << "Level: " << level << std::endl;
            init_structure();
            m_topology[0]=1;
            m_offsets[0].resize(0);
            m_pointers[0].resize(0);
            while (block_size > 1) {
                //std::cout << "Block size at level " << level << ": " << block_size << std::endl;
                ++level;
                htc_type m_htc(2*nodes.size()); //2* nodes, in order to reduce resize operations
                block_tree_2d::algorithm::get_fingerprint_blocks(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
                block_tree_2d::algorithm::get_type_of_nodes(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
                block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
                compact_current_level(nodes, level, topology_index, is_pointer_index);
                block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, m_k2, nodes);
                //std::cout << "Level: " << level << std::endl;
                block_size = block_size / k;

            }
            block_tree_2d::algorithm::compute_last_level(adjacency_lists, hash, nodes);
            compact_last_level(nodes, topology_index);
            m_height = level+1;
            m_topology.resize(topology_index);
            m_is_pointer.resize(is_pointer_index);
            m_level_ones.resize(2*m_height);
            sdsl::util::init_support(m_topology_rank, &m_topology);
            sdsl::util::init_support(m_topology_select, &m_topology);
            sdsl::util::init_support(m_is_pointer_rank, &m_is_pointer);
            sdsl::util::bit_compress(m_level_ones);
            //std::cout << std::endl;
        }


        void print(){
            print_container(m_topology, "topology");
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

        void access_region(const size_type min_x, const size_type min_y,
                           const size_type max_x, const size_type max_y,
                           input_type &result){
            size_type size_vector = max_y - min_y+1;
            result = input_type(size_vector);
            //result = std::vector<sdsl::bit_vector>(size_vector, sdsl::bit_vector(size_bit_vector, 0));
            auto block_size = (size_type) std::pow(m_k, m_height);
            std::cout << "Block size: " << block_size << std::endl;
            recursive_access_region(min_x, max_x, min_y, max_y, 0, 0, 0, 0, block_size, result);

        }
    };
}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_HPP
