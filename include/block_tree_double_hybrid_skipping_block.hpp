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
// Created by Adrián on 09/08/2019.
//

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_DOUBLE_HYBRID_SKIPPING_BLOCK_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_DOUBLE_HYBRID_SKIPPING_BLOCK_HPP

#include <block_tree_algorithm_helper_v2.hpp>
#include "alternative_code.hpp"
#include <logger.hpp>
#include <block_tree.hpp>
#include <vector>
#include <adjacency_list_helper.hpp>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree_double_hybrid_skipping_block : public block_tree<input_t> {

    public:

        typedef input_t input_type;
        typedef typename block_tree<input_t>::value_type value_type;
        typedef typename block_tree<input_t>::size_type size_type;
        typedef typename block_tree<input_t>::node_type node_type;
        typedef typename block_tree<input_t>::htc_type htc_type;
        typedef typename block_tree<input_t>::htc_multiple_type htc_multiple_type;
        typedef typename block_tree<input_t>::htc_multiple_v2_type htc_multiple_v2_type;

    private:

        size_type m_minimum_level; //starts block_tree
        size_type m_maximum_level; //ends block_tree
        size_type m_zeroes;
        sdsl::bit_vector m_explicit;
        sdsl::rank_support_v5<> m_rank_explicit;
        std::vector<sdsl::int_vector<>> m_explicit_location;

        void add_new_pointers_offsets_explicit(){
            this->m_offsets.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
            this->m_pointers.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
            m_explicit_location.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }

        size_type compact_current_level(const std::vector<node_type> &nodes, const size_type level,
                                        size_type &topology_index, size_type &is_pointer_index, size_type &explicit_index) {
            size_type offset_index = 0, pointer_index = 0, explicit_location_index = 0;
            add_new_pointers_offsets_explicit();
            this->check_resize(this->m_level_ones, 3*level+1);
            this->m_level_ones[3*level] = topology_index; //number of elements up to the current level (excluded)
            this->m_level_ones[3*level+1] = this->m_level_ones[3*(level-1)+1]; //number of pointers up to the current level (included)
            this->m_level_ones[3*level+2] = this->m_level_ones[3*(level-1)+2]; //number of explicit up to the current level (included)
            size_type pointers = 0;
            for(const auto &node : nodes) {
                //Check sizes
                //check_resize(m_topology, topology_index);
                this->check_resize(this->m_t, topology_index);
                this->check_resize(this->m_is_pointer, is_pointer_index);
                this->check_resize(m_explicit, explicit_index);
                if(node.type == NODE_INTERNAL){
                    this->m_t[topology_index++] = 1;
                }else if (node.type == NODE_EMPTY){
                    this->m_t[topology_index++] = 0;
                    this->m_is_pointer[is_pointer_index++] = 0;
                }else if (node.type == NODE_LEAF){
                    this->m_t[topology_index++] = 0;
                    this->m_is_pointer[is_pointer_index++] = 1;
                    ++(this->m_level_ones[3*level+1]);
                    this->m_explicit[explicit_index++] = 0;
                    this->check_resize(this->m_offsets[level], offset_index+1);
                    this->check_resize(this->m_pointers[level], pointer_index);
                    this->m_offsets[level][offset_index++] = codes::alternative_code::encode(node.offset_x);
                    this->m_offsets[level][offset_index++] = codes::alternative_code::encode(node.offset_y);
                    this->m_pointers[level][pointer_index++] = node.ptr;
                    ++pointers;
                }else{
                    this->m_t[topology_index++] = 0;
                    this->m_is_pointer[is_pointer_index++] = 1;
                    ++(this->m_level_ones[3*level+1]);
                    ++(this->m_level_ones[3*level+2]);
                    m_explicit[explicit_index++] = 1;
                    this->check_resize(m_explicit_location[level], explicit_location_index+1);
                    m_explicit_location[level][explicit_location_index++] = node.offset_x;
                    m_explicit_location[level][explicit_location_index++] = node.offset_y;
                }
            }
            this->m_offsets[level].resize(offset_index);
            this->m_pointers[level].resize(pointer_index);
            m_explicit_location[level].resize(explicit_location_index);
            sdsl::util::bit_compress(this->m_offsets[level]);
            sdsl::util::bit_compress(this->m_pointers[level]);
            sdsl::util::bit_compress(m_explicit_location[level]);
            return pointers;
        }


        void construction(input_type &adjacency_lists, size_type h, size_type block_size, size_type blocks,
                          size_type last_block_size_k2_tree){

            typedef typename input_type::value_type::iterator iterator_value_type;

            std::cout << "Construction hybrid_blocktree with height=" << h << std::endl;
            //0. Init the structure
            this->init_structure();
            m_explicit = sdsl::bit_vector(BITMAP_RESIZE);
            m_explicit_location = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(0, 0));
            this->m_offsets[0].resize(0);
            this->m_pointers[0].resize(0);

            //1. Obtaining minimum block size where there are identical blocks
            //size_type min_block_size = 16;
            //m_minimum_level = 16;

            size_type min_block_size = last_block_size_k2_tree;
            m_minimum_level = h - log2(min_block_size);
            m_maximum_level = h+1;
            util::logger::log("Minimum level=" + std::to_string(m_minimum_level) + " and block_size=" + std::to_string(min_block_size));
            //exit(10);

            //2. Building LOUDS of k2_tree until min_block_size and map between z_order and position in vector nodes
            block_tree_2d::algorithm::hash_type hash;
            m_zeroes = block_tree_2d::algorithm::build_k2_tree(adjacency_lists, this->k, h, min_block_size, this->m_t, hash);

            /*for(size_type i = 0; i < this->m_topology.size(); ++i){
                std::cout << this->m_topology[i] << ", ";
            }*/
            std::cout << std::endl;
            std::vector<node_type> nodes(hash.size());
            size_type topology_index = this->m_t.size(), leaves_index = 0, explicit_index = 0, is_pointer_index = 0;
            size_type l = m_minimum_level;
            block_size = min_block_size / this->m_k;
            bool last_k2_tree = false;
            while (!last_k2_tree && block_size > 1) {
                ++l;
                util::logger::log("Processing level " + std::to_string(l) + " of " + std::to_string(h));
                util::logger::log("Block size: " + std::to_string(block_size));
                htc_type m_htc(std::min(10240UL, 2*nodes.size()));
                //util::logger::log("Computing bits required by k2-tree=");
                //size_type bits_k2_tree = bits_last_k2_tree(adjacency_lists, this->k, block_size);
                std::vector<iterator_value_type> iterators_to_delete;
                util::logger::log("Computing fingerprint of blocks at level=" + std::to_string(l));
                block_tree_2d::algorithm::get_fingerprint_blocks_skipping_blocks_stack_lite(adjacency_lists, this->k,
                        m_htc, this->dimensions, block_size, hash, nodes, iterators_to_delete, true);
                block_tree_2d::algorithm::mark_to_delete(iterators_to_delete);


                util::logger::log("Computing fingerprint of shifts at level=" + std::to_string(l));
                block_tree_2d::algorithm::get_type_of_nodes_skipping_blocks_stack_lite(adjacency_lists, this->k, m_htc, this->dimensions, block_size, hash, nodes);


                size_type bits_k2_tree = 0, leaf_nodes = 0, empty_nodes = 0, internal_nodes = 0, explicit_nodes = 0;
                size_type bits_per_offset = 0, bits_per_pointer = 0, bits_per_explicit = 0;
                for(const auto &node: nodes){
                    if(node.type == NODE_LEAF) {
                        bits_k2_tree += node.bits;
                        auto b_offx = sdsl::bits::hi(codes::alternative_code::encode(node.offset_x)) + 1;
                        if (bits_per_offset < b_offx) {
                            bits_per_offset = b_offx;
                        }
                        auto b_offy = sdsl::bits::hi(codes::alternative_code::encode(node.offset_y)) + 1;
                        if (bits_per_offset < b_offy) {
                            bits_per_offset = b_offy;
                        }
                        auto b_ptr = sdsl::bits::hi(node.ptr) + 1;
                        if (bits_per_pointer < b_ptr) {
                            bits_per_pointer = b_ptr;
                        }
                        ++leaf_nodes;
                    }else if(node.type == NODE_EXPLICIT){
                        bits_k2_tree += block_size*this->m_k;
                        auto b_offx = sdsl::bits::hi(codes::alternative_code::encode(node.offset_x)) + 1;
                        if (bits_per_explicit < b_offx) {
                            bits_per_explicit = b_offx;
                        }
                        auto b_offy = sdsl::bits::hi(codes::alternative_code::encode(node.offset_y)) + 1;
                        if (bits_per_explicit < b_offy) {
                            bits_per_explicit = b_offy;
                        }
                        ++explicit_nodes;
                    }else if(node.type == NODE_EMPTY){
                        ++empty_nodes;
                    }else{
                        ++internal_nodes;
                    }
                }
                //size_type bits_block_tree = leaf_nodes*(bits_per_pointer + 2*bits_per_offset + 2) + empty_nodes*2 + internal_nodes;
                last_k2_tree = (bits_k2_tree < (leaf_nodes*(bits_per_pointer + 2*bits_per_offset + 3)
                        + empty_nodes + explicit_nodes*(bits_per_explicit*2+3)));
                //last_k2_tree = block_size < 4;
                if(last_k2_tree){
                    this->m_t.resize(topology_index);
                    m_maximum_level = l;
                    size_type height_subtree = h - l +1;
                    std::cout << "Height: " << h << " current_level: " << l << " height_subtree: " << height_subtree << std::endl;
                    block_tree_2d::algorithm::build_last_k2_tree(adjacency_lists, this->k, height_subtree, block_size, this->m_t, this->m_l);
                    std::cout << "T size: " << this->m_t.size() << std::endl;
                    std::cout << "L size: " << this->m_l.size() << std::endl;
                }else{
                    util::logger::log("Clearing adjacency lists at level=" + std::to_string(l));
                    block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
                    util::logger::log("Compacting level=" + std::to_string(l));
                    auto pointers = this->compact_current_level(nodes, l - m_minimum_level, topology_index, is_pointer_index, explicit_index);
                    util::logger::log("Number of new pointers=" + std::to_string(pointers));
                    util::logger::log("Preparing next level");
                    block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, this->m_k2, nodes);
                    block_size = block_size / this->k;
                    std::cout << "T size: " << this->m_t.size() << std::endl;
                }

            }
            if(!last_k2_tree){
                ++l;
                util::logger::log("Processing last level (" + std::to_string(l) + ")");
                block_tree_2d::algorithm::compute_last_level(adjacency_lists, hash, nodes);
                util::logger::log("Compacting last level (" + std::to_string(l) + ")");
                this->compact_last_level(nodes, leaves_index);
                this->m_l.resize(leaves_index);
                std::cout << "L size: " << this->m_l.size() << std::endl;
            }
            //m_maximum_level = h+1;
            this->m_height = h;
            this->m_is_pointer.resize(is_pointer_index);
            m_explicit.resize(explicit_index);
            this->m_level_ones.resize(3*(m_maximum_level - m_minimum_level));
            std::cout << "m_minimum_level " << m_minimum_level << std::endl;
            std::cout << "m_maximum_level " << m_maximum_level << std::endl;
            sdsl::util::init_support(this->m_t_rank, &this->m_t);
            sdsl::util::init_support(this->m_t_select, &this->m_t);
            sdsl::util::init_support(this->m_is_pointer_rank, &this->m_is_pointer);
            sdsl::util::init_support(m_rank_explicit, &m_explicit);
            sdsl::util::bit_compress(this->m_level_ones);
            util::logger::log("2D Block Tree DONE!!!");
        }


        void copy(const block_tree_double_hybrid_skipping_block &p){
            block_tree<input_type >::copy(p);
            m_zeroes = p.m_zeroes;
            m_minimum_level = p.m_minimum_level;
            m_maximum_level = p.m_maximum_level;
            m_explicit = p.m_explicit;
            m_rank_explicit = p.m_rank_explicit;
            m_rank_explicit.set_vector(&m_explicit);
            m_explicit_location = p.m_explicit_location;
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
            std::cout << "height: " << height << std::endl;
            std::cout << std::endl;
#endif
            //std::cout << "level: " << level << " block_size: " << block_size << " min_x: " << min_x << " max_x: " << max_x << " min_y: " << min_y << " max_y: " << max_y << std::endl;
            if(level == this->m_height){
                //if(m_topology[idx]){
                if(this->m_l[idx - this->m_t.size()]){
                    //Adding result
                    add(result, x, y);
                    //result[y].push_back(x);
                }
            }else{
                //if(m_topology[idx]){
                if(this->m_t[idx]){
                    size_type new_min_x, new_max_x, new_min_y, new_max_y;
                    //size_type start_children =  m_topology_rank(idx + 1) * m_k2;
                    size_type start_children =  this->m_t_rank(idx + 1) * this->m_k2;
                    size_type new_block_size = block_size / this->m_k;
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
                    //std::cout << "taking_pointer" << std::endl;
                    size_type pos_leaf = idx_leaf(idx);
                    if(idx > 0 && this->m_is_pointer[pos_leaf]){
                        //std::cout << "is_pointer" << std::endl;
                        size_type pos_pointer_or_explicit = idx_pointer_or_explicit(pos_leaf);
                        size_type pos_explicit = idx_explicit(pos_pointer_or_explicit, level);
                        if(m_explicit[pos_pointer_or_explicit]){
                            size_type offset_x, offset_y;
                            explicit_node_info(pos_explicit, level, offset_x, offset_y);
                            //std::cout << "explicit: " << x + offset_x << ", " << y + offset_y << std::endl;
                            if(min_x <= offset_x && offset_x <= max_x && min_y <= offset_y && offset_y <= max_y){
                                add(result, x + offset_x - min_x, y + offset_y - min_y);
                            }
                        }else{
                            //std::cout << "pointer" << std::endl;
                            size_type pos_pointer = idx_pointer(pos_explicit, pos_pointer_or_explicit, level);
                            value_type offset_x, offset_y;
                            size_type pointer;
                            leaf_node_info(pos_pointer, level, pointer, offset_x, offset_y);
                            take_pointer(min_x, max_x, min_y, max_y, x, y, pointer,
                                         offset_x, offset_y, level, block_size, result, add);
                        }
                    }
                }
            }
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
                auto zth = ptr % this->m_k2;
                auto p = codes::zeta_order::decode(zth);
                new_min_x += p.first * block_size;
                new_min_y += p.second * block_size;
                block_size *= this->m_k;
                l--;
                ptr = this->m_t_select(ptr / this->m_k2);
            }
            recursive_access_region(static_cast<size_type >(new_min_x),
                                    static_cast<size_type >(new_min_x + length_x),
                                    static_cast<size_type >(new_min_y),
                                    static_cast<size_type >(new_min_y + length_y), x, y, ptr, l, block_size, result, add, true, level);
        }


        inline bool taking_pointer_condition(const bool taking_pointer, const size_type level_taking_pointer, const size_type level){
            return level > m_minimum_level && level < m_maximum_level && !(taking_pointer && level <= level_taking_pointer);
        }

        inline size_type idx_leaf(const size_type idx) {
            return idx - this->m_t_rank(idx + 1) - m_zeroes;
        }


        inline size_type idx_pointer_or_explicit(const size_type idx){
            return this->m_is_pointer_rank(idx+1)-1;
        }

        inline size_type idx_explicit(const size_type idx, const size_type level){
            return m_rank_explicit(idx+1) - this->m_level_ones[3*(level-m_minimum_level-1)+2]-1;
        }

        inline size_type idx_pointer(const size_type idx_explicit, const size_type idx_pointer_or_explicit, const size_type level){
            return idx_pointer_or_explicit - this->m_level_ones[3*(level-m_minimum_level-1)+1] - (idx_explicit+1);
        }

        void leaf_node_info(const size_type idx_pointer, const size_type level,
                            size_type &pointer, value_type &offset_x, value_type &offset_y){

            pointer = this->m_level_ones[3*(level-m_minimum_level)]+ this->m_pointers[level-m_minimum_level][idx_pointer];
            offset_x = codes::alternative_code::decode(this->m_offsets[level-m_minimum_level][2*idx_pointer]);
            offset_y = codes::alternative_code::decode(this->m_offsets[level-m_minimum_level][2*idx_pointer+1]);
        }

        void explicit_node_info(const size_type pos_explicit, const size_type level, size_type &offset_x, size_type &offset_y){
            offset_x = m_explicit_location[level-m_minimum_level][2*pos_explicit];
            offset_y = m_explicit_location[level-m_minimum_level][2*pos_explicit+1];
        }

        bool is_pointer(size_type idx, size_type level, size_type &pos_zero){
            if(level <= m_minimum_level || level >= m_maximum_level){
                return false;
            }
            pos_zero= this->idx_leaf(idx);
            return (idx > 0 && this->m_is_pointer[pos_zero]);
        }


    public:

        const size_type &minimum_level = m_minimum_level;
        const size_type &maximum_level = m_maximum_level;
        block_tree_double_hybrid_skipping_block() = default;

        block_tree_double_hybrid_skipping_block(input_type &adjacency_lists, const size_type kparam, const size_type level) {
            size_type h, total_size;
            this->init_construction(h, total_size, adjacency_lists, kparam);
            size_type blocks = this->m_k2, block_size = total_size/this->m_k;
            construction(adjacency_lists, h, block_size, blocks, level);
        }


        inline void access_region(const size_type min_x, const size_type min_y,
                                  const size_type max_x, const size_type max_y,
                                  input_type &result){
            size_type size_vector = max_y - min_y+1;
            result = input_type(size_vector);
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            this->recursive_access_region(min_x, max_x, min_y, max_y, 0, 0, 0, 0, block_size, result, add_in_region());

        }

        inline std::vector<size_type> neigh(size_type id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            this->recursive_access_region(0, this->m_dimensions-1, id, id, 0, 0, 0, 0, block_size, r, add_in_row());
            return r;
        }

        inline std::vector<size_type> reverse_neigh(size_type id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            this->recursive_access_region(id, id, 0, this->m_dimensions-1, 0, 0, 0, 0, block_size, r, add_in_column());
            return r;
        }


        //! Copy constructor
        block_tree_double_hybrid_skipping_block(const block_tree_double_hybrid_skipping_block &p) {
            copy(p);
        }

        //! Move constructor
        block_tree_double_hybrid_skipping_block(block_tree_double_hybrid_skipping_block &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        block_tree_double_hybrid_skipping_block &operator=(block_tree_double_hybrid_skipping_block &&p) {
            if (this != &p) {
                block_tree<input_type>::operator=(p);
                m_minimum_level = std::move(p.m_minimum_level);
                m_maximum_level = std::move(p.m_maximum_level);
                m_zeroes = std::move(p.m_zeroes);
                m_explicit = std::move(p.m_explicit);
                m_rank_explicit = std::move(p.m_rank_explicit);
                m_rank_explicit.set_vector(&m_explicit);
                m_explicit_location = std::move(p.m_explicit_location);
            }
            return *this;
        }

        //! Assignment operator
        block_tree_double_hybrid_skipping_block &operator=(const block_tree_double_hybrid_skipping_block &p) {
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
        void swap(block_tree_double_hybrid_skipping_block &p) {
            block_tree<input_type>::swap(p);
            std::swap(m_minimum_level, p.m_minimum_level);
            std::swap(m_maximum_level, p.m_maximum_level);
            std::swap(m_zeroes, p.m_zeroes);
            std::swap(m_explicit, p.m_explicit);
            sdsl::util::swap_support(m_rank_explicit, p.m_rank_explicit, &m_explicit, &(p.m_explicit));
            std::swap(m_explicit_location, p.m_explicit_location);
        }


        //! Serializes the data structure into the given ostream
         size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr,
                            const std::string name="")const
        {
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(
                    v, name, sdsl::util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += block_tree<input_type>::serialize(out, v, name);
            written_bytes += sdsl::write_member(m_minimum_level, out, child, "minimum_level");
            written_bytes += sdsl::write_member(m_maximum_level, out, child, "maximum_level");
            written_bytes += sdsl::write_member(m_zeroes, out, child, "zeroes");
            written_bytes += m_explicit.serialize(out, child, "explicit");
            written_bytes += m_rank_explicit.serialize(out, child, "rank_explicit");
            uint64_t m_explicit_location_size = m_explicit_location.size();
            sdsl::write_member(m_explicit_location_size, out, child, "explicit_location_size");
            written_bytes += sdsl::serialize_vector(m_explicit_location, out, child, "explicit_location");
            return written_bytes;
        }

        //! Loads the data structure from the given istream.
        void load(std::istream& in)
        {
            block_tree<input_type>::load(in);
            sdsl::read_member(m_minimum_level, in);
            sdsl::read_member(m_maximum_level, in);
            sdsl::read_member(m_zeroes, in);
            m_explicit.load(in);
            m_rank_explicit.load(in);
            m_rank_explicit.set_vector(&m_explicit);
            uint64_t m_explicit_location_size = 0;
            sdsl::read_member(m_explicit_location_size, in);
            m_explicit_location.resize(m_explicit_location_size);
            sdsl::load_vector(m_explicit_location, in);
        }


    };
}
#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_DOUBLE_HYBRID_SKIPPING_BLOCK_HPP
