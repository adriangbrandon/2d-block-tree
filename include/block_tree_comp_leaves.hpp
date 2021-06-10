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

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_COMP_LEAVES_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_COMP_LEAVES_HPP

#include <block_tree_algorithm_helper.hpp>
#include "alternative_code.hpp"
#include "dataset_reader.hpp"
#include "dac_vector.hpp"
#include <logger.hpp>
#include <block_tree.hpp>
#include <vector>
#include <adjacency_list_helper.hpp>

namespace block_tree_2d {

    template <class reader_t = dataset_reader::web_graph,
              class input_t = std::vector<std::vector<int64_t>>,
              uint8_t leaf_size = 4, uint8_t voc_word = 16>
    class block_tree_comp_leaves {

    public:

        typedef input_t input_type;
        typedef typename block_tree<input_t>::value_type value_type;
        typedef typename block_tree<input_t>::size_type size_type;
        typedef typename block_tree<input_t>::node_type node_type;
        typedef typename block_tree<input_t>::htc_type htc_type;
        typedef reader_t reader_type;
        typedef struct {
            size_type min_x, max_x, min_y, max_y;
        } region_type;

    private:

        size_type m_k;
        size_type m_k2;
        size_type m_dimensions;
        size_type m_height;
        sdsl::bit_vector m_t;
        sdsl::rank_support_v5<> m_t_rank;
        sdsl::bit_vector::select_1_type m_t_select;
        sdsl::bit_vector m_is_pointer;
        sdsl::bit_vector::rank_1_type m_is_pointer_rank;
        std::vector<sdsl::int_vector<>> m_pointers;
        std::vector<sdsl::int_vector<>> m_offsets;
        sdsl::int_vector<> m_level_ones;
        value_type m_msb;
        size_type m_minimum_level; //starts block_tree
        size_type m_maximum_level; //ends block_tree
        size_type m_zeroes;
        sdsl::bit_vector m_explicit;
        sdsl::rank_support_v5<> m_rank_explicit;
        sdsl::int_vector<voc_word> m_voc;
        sdsl::dac_vector_dp_opt<> m_l;
        size_type m_ones_prev_leaves;

        typedef struct {
            size_type x;
            size_type y;
            size_type child;
        } node_info_type;

        void add_new_pointers_offsets_explicit(){
            this->m_offsets.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
            this->m_pointers.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }

        template<class CollectionNodes>
        size_type compact_current_level(const CollectionNodes &nodes, const size_type level,
                                        size_type &topology_index, size_type &is_pointer_index, size_type &explicit_index) {
            size_type offset_index = 0, pointer_index = 0;
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
                }
            }
            this->m_offsets[level].resize(offset_index);
            this->m_pointers[level].resize(pointer_index);
            sdsl::util::bit_compress(this->m_offsets[level]);
            sdsl::util::bit_compress(this->m_pointers[level]);
            return pointers;
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

       /* void construction(const std::string &file_name, input_type &adjacency_lists, size_type h,
                          size_type block_size, size_type blocks,
                          size_type last_block_size_k2_tree, const size_type n_rows = 0, const size_type n_cols = 0){

            typedef typename input_type::value_type::iterator iterator_value_type;

            std::cout << "Construction comp_ones block_tree with height=" << h << std::endl;
            //0. Init the structure
            this->init_structure();
            m_explicit = sdsl::bit_vector(BITMAP_RESIZE);
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
            //block_tree_2d::algorithm::hash_type hash;
            size_type n_elem = 0;
            m_zeroes = block_tree_2d::algorithm::build_k2_tree(adjacency_lists, this->m_k, h,
                                                               min_block_size, this->m_t, n_elem, true);
            //TODO: zorder mapping
            sdsl::util::init_support(this->m_t_rank, &this->m_t);


            std::cout << adjacency_lists.size() << std::endl;
            auto rows_cols = reader_type::read(file_name, adjacency_lists, n_rows, n_cols);
            m_msb = rows_cols.second / n_cols -1;
            h = (size_type) std::ceil(std::log(this->m_dimensions)/std::log(this->m_k));
            auto total_size = (size_type) std::pow(this->m_k, h);
            if(adjacency_lists.size() < total_size){
                adjacency_lists.resize(total_size);
            }
            std::cout << std::endl;
            std::vector<node_type> nodes(n_elem);
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

                block_tree_2d::algorithm::get_fingerprint_blocks_comp_ones(adjacency_lists, this->k,
                        m_htc, this->dimensions, block_size, this->m_t, this->m_t_rank, topology_index,
                        nodes, iterators_to_delete, true);
                block_tree_2d::algorithm::mark_to_delete(iterators_to_delete);


                util::logger::log("Computing fingerprint of shifts at level=" + std::to_string(l));

                block_tree_2d::algorithm::get_type_of_nodes_skipping_blocks_stack_lite(adjacency_lists, this->k,
                                                                                       m_htc, this->dimensions,
                                                                                       block_size, this->m_t,
                                                                                       this->m_t_rank, topology_index,
                                                                                       nodes);


                size_type bits_k2_tree = 0, leaf_nodes = 0, empty_nodes = 0, internal_nodes = 0, explicit_nodes = 0;
                size_type bits_per_offset = 0, bits_per_pointer = 0, bits_per_explicit = 0;
                auto height = log2(block_size);
                auto explicit_bits = std::pow(this->m_k, height+1)-1;
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
                        //TODO: comp_ones in k2-tree?
                        bits_k2_tree += 2;
                        ++explicit_nodes;
                    }else if(node.type == NODE_EMPTY){
                        bits_k2_tree += 2;
                        ++empty_nodes;
                    }else{
                        ++internal_nodes;
                    }
                }
                //size_type bits_block_tree = leaf_nodes*(bits_per_pointer + 2*bits_per_offset + 2) + empty_nodes*2 + internal_nodes;
                std::cout << "Size: " << nodes.size() << std::endl;
                std::cout << "Empty nodes: " << empty_nodes << std::endl;
                std::cout << "Explicit nodes: " << explicit_nodes << std::endl;
                std::cout << "Internal nodes: " << internal_nodes << std::endl;
                std::cout << "Leaf nodes: " << leaf_nodes << std::endl;

                last_k2_tree = (bits_k2_tree < (leaf_nodes*(bits_per_pointer + 2*bits_per_offset + 3)
                        + empty_nodes*2 + explicit_nodes*3));
                //last_k2_tree = block_size < 4;
                if(last_k2_tree){
                    this->m_t.resize(topology_index);
                    this->m_is_pointer.resize(is_pointer_index);
                    m_maximum_level = l;
                    size_type height_subtree = h - l +1;
                    block_tree_2d::algorithm::build_last_k2_tree(adjacency_lists, this->k, height_subtree,
                                                                 block_size, this->m_t, this->m_l, this->m_is_pointer);
                }else{
                    util::logger::log("Clearing adjacency lists at level=" + std::to_string(l));
                    block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
                    util::logger::log("Compacting level=" + std::to_string(l));
                    auto pointers = this->compact_current_level(nodes, l - m_minimum_level, topology_index,
                                                                is_pointer_index, explicit_index);
                    util::logger::log("Number of new pointers=" + std::to_string(pointers));
                    util::logger::log("Preparing next level");
                    //block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, this->m_k2, nodes);
                    block_tree_2d::algorithm::prepare_next_level(adjacency_lists, this->m_k2, nodes);
                    //nodes = std::vector<node_type>(internal_nodes*this->m_k2);
                    block_size = block_size / this->k;
                    std::cout << "T size: " << this->m_t.size() << std::endl;
                    //TODO: zorder mapping
                    sdsl::util::init_support(this->m_t_rank, &this->m_t);
                }

            }
            if(!last_k2_tree){
                ++l;
                util::logger::log("Processing last level (" + std::to_string(l) + ")");
                //block_tree_2d::algorithm::compute_last_level(adjacency_lists, hash, nodes);
                block_tree_2d::algorithm::compute_last_level(adjacency_lists, this->m_k, this->m_t,
                                                             this->m_t_rank,  topology_index, nodes);
                util::logger::log("Compacting last level (" + std::to_string(l) + ")");
                this->compact_last_level(nodes, leaves_index);
                this->m_is_pointer.resize(is_pointer_index);
                std::cout << "L size: " << this->m_l.size() << std::endl;
            }
            adjacency_lists.clear();
            //m_maximum_level = h+1;
            this->m_height = h;
            //this->m_is_pointer.resize(is_pointer_index);
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

        }*/


        void copy(const block_tree_comp_leaves &p){
            m_k = p.m_k;
            m_k2 = m_k*m_k;
            m_height = p.m_height;
            m_dimensions = p.m_dimensions;
            m_t = p.m_t;
            m_t_rank = p.m_t_rank;
            m_t_rank.set_vector(&m_t);
            m_t_select = p.m_t_select;
            m_t_select.set_vector(&m_t);
            m_is_pointer = p.m_is_pointer;
            m_is_pointer_rank = p.m_is_pointer_rank;
            m_is_pointer_rank.set_vector(&m_is_pointer);
            m_level_ones = p.m_level_ones;
            m_pointers = p.m_pointers;
            m_offsets = p.m_offsets;
            m_zeroes = p.m_zeroes;
            m_minimum_level = p.m_minimum_level;
            m_maximum_level = p.m_maximum_level;
            m_explicit = p.m_explicit;
            m_rank_explicit = p.m_rank_explicit;
            m_rank_explicit.set_vector(&m_explicit);
            m_msb = p.m_msb;
            m_voc = p.m_voc;
            m_l = p.m_l;
            m_ones_prev_leaves = p.m_ones_prev_leaves;
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
                }else {
                    if(taking_pointer_condition(taking_pointer, level_taking_pointer, level)){
                        size_type pos_leaf = idx_leaf(idx);
                        if(idx > 0 && this->m_is_pointer[pos_leaf]){
                            size_type pos_pointer_or_explicit = idx_pointer_or_explicit(pos_leaf);
                            size_type pos_explicit = idx_explicit(pos_pointer_or_explicit, level);

                            if(m_explicit[pos_pointer_or_explicit]){
                                //TODO: adding all the elements on the queried region
                                for (size_type offset_y = min_y; offset_y <= max_y; ++offset_y) {
                                    for (size_type offset_x = min_x; offset_x <= max_x; ++offset_x) {
                                        add(result, x + offset_x - min_x, y + offset_y - min_y);
                                    }
                                }
                            }else{
                                size_type pos_pointer = idx_pointer(pos_explicit, pos_pointer_or_explicit, level);
                                value_type offset_x, offset_y;
                                size_type pointer;
                                leaf_node_info(pos_pointer, level, pointer, offset_x, offset_y);
                                take_pointer(min_x, max_x, min_y, max_y, x, y, pointer,
                                             offset_x, offset_y, level, block_size, result, add);
                            }
                        }
                    }else if(level >= m_maximum_level){
                        size_type pos_leaf = idx_leaf(idx);
                        if(this->m_is_pointer[pos_leaf]){
                            for (size_type offset_y = min_y; offset_y <= max_y; ++offset_y) {
                                for (size_type offset_x = min_x; offset_x <= max_x; ++offset_x) {
                                    add(result, x + offset_x - min_x, y + offset_y - min_y);
                                }
                            }
                        }
                    }

                }
            }
        }


        inline void add_value(const region_type &query, const size_type x, const size_type y,
                              const size_type n_cols, std::vector<int> &result){

            auto b = x / n_cols;
            auto c = x % n_cols;
            //std::cout << "adding b=" << b << " x=" << c << " y=" << y << std::endl;
            auto i = (query.max_x-query.min_x+1) * (y-query.min_y) + (c- query.min_x);
            result[i] = result[i] | (0x0001 << b);
        }

        //min_x, max_x, min_y and max_y are relative to the raster matrix
        void recursive_multiple_region(const region_type &query, std::vector<region_type> &regions,
                                     const size_type x, const size_type y, const size_type idx, const size_type level,
                                     const size_type block_size, const size_type n_cols, const size_type n_rows,
                                     std::vector<int> &result,
                                     const bool taking_pointer=false, const size_type level_taking_pointer = 0){

            assert(!regions.empty());
            //std::cout << "rec" << std::endl;
#if BT_VERBOSE
            std::cout << "at position: " << idx << std::endl;
            std::cout << "x=" << x << " y=" << y << std::endl;
            std::cout << "block_size: " << block_size << std::endl;
            std::cout << "current_level: " << level << std::endl;
            std::cout << "Regions:" << std::endl;
            for(const auto &reg :regions){
                std::cout << "(" << x+reg.min_x << ", " << x+reg.max_x << ") x (" << y+reg.min_y << ", " << y+reg.max_y << ")" << std::endl;
            }
            std::cout << std::endl;
#endif
            /*if(level == this->m_height){
                //if(m_topology[idx]){
                if(this->m_l[idx - this->m_t.size()]){
                    //Adding result
                    add_value(query, x, y, n_cols, result);
                    //result[y].push_back(x);
                }*/
            if(block_size == leaf_size){
                if(m_t[idx]) {
                    size_type c_i = m_t_rank(idx+1) - m_ones_prev_leaves -1;
                    size_type code = m_voc[m_l[c_i]];
                    while(code){
                        size_type set_bit = sdsl::bits::lo(code);
                        size_type o_x = set_bit % leaf_size;
                        size_type o_y = set_bit / leaf_size;
                        for(auto const &region : regions) {
                            if(region.min_x <= o_x && o_x <= region.max_x
                                && region.min_y <= o_y && o_y <= region.max_y){
                                add_value(query,x + o_x, y + o_y, n_cols, result);
                            }
                        }
                        code = code & sdsl::bits::lo_unset[set_bit+1];
                    }
                }else if(level >= m_maximum_level){
                    size_type pos_leaf = idx_leaf(idx);
                    if(this->m_is_pointer[pos_leaf]){
                        for(auto const &region : regions) {
                            size_type min_x, max_x, min_y, max_y;
                            for (size_type offset_y = region.min_y; offset_y <= region.max_y; ++offset_y) {
                                for (size_type offset_x = region.min_x; offset_x <= region.max_x; ++offset_x) {
                                    add_value(query,x + offset_x, y + offset_y, n_cols, result);
                                }
                            }
                        }
                    }
                }
            }else{
                //if(m_topology[idx]){
                if(this->m_t[idx]){
                    size_type new_min_x, new_max_x, new_min_y, new_max_y;
                    //size_type start_children =  m_topology_rank(idx + 1) * m_k2;
                    size_type start_children =  this->m_t_rank(idx + 1) * this->m_k2;
                    size_type new_block_size = block_size / this->m_k;
                    std::vector<std::vector<region_type>> queues(this->m_k2);

                    for(auto &region : regions){

                        size_type min_x = region.min_x;
                        size_type max_x = region.max_x;
                        size_type min_y = region.min_y;
                        size_type max_y = region.max_y;
                        for (size_type i = min_x / new_block_size; i <= max_x / new_block_size; i++) {
                            new_min_x = 0;
                            if (i == min_x / new_block_size) {
                                new_min_x = min_x % new_block_size;
                            }
                            new_max_x = new_block_size - 1;
                            if (i == max_x / new_block_size) {
                                new_max_x = max_x % new_block_size;
                            }
                            for (size_type j = min_y / new_block_size; j <= max_y / new_block_size; j++) {
                                new_min_y = 0;
                                if (j == min_y / new_block_size) {
                                    new_min_y = min_y % new_block_size;
                                }
                                new_max_y = new_block_size - 1;
                                if (j == max_y / new_block_size) {
                                    new_max_y = max_y % new_block_size;
                                }

                                auto n_i = i * this->k + j;
                                assert(n_i < this->m_k2);
                                assert(0 <= new_min_x && new_min_x <= new_block_size-1);
                                assert(0 <= new_min_y && new_min_y <= new_block_size-1);
                                assert(0 <= new_max_x && new_max_x <= new_block_size-1);
                                assert(0 <= new_max_y && new_max_y <= new_block_size-1);
                                queues[n_i].emplace_back(region_type{new_min_x, new_max_x, new_min_y, new_max_y});
                            }
                        }
                    }
                    regions.clear();
                    for(size_type i = 0; i < this->m_k; ++i){
                        for(size_type j = 0; j < this->m_k; ++j){
                            auto n_i = i * this->m_k + j;
                            assert(n_i < this->m_k2);
                            if(queues[n_i].size()>0) {
                                assert(queues[n_i].size() > 0);
                               // std::cout << "n_i:" << n_i << std::endl;
                               // std::cout << "regions: " << queues[n_i].size() << std::endl;
                                recursive_multiple_region(query, queues[n_i],
                                                          x + (new_block_size * i),
                                                          y + (new_block_size * j),
                                                          start_children + codes::zeta_order::encode(i, j),
                                                          level + 1,
                                                          new_block_size, n_cols, n_rows, result,
                                                          taking_pointer, level_taking_pointer);
                            }
                        }
                    }
                }else {
                    if(taking_pointer_condition(taking_pointer, level_taking_pointer, level)){
                        size_type pos_leaf = idx_leaf(idx);
                        if(idx > 0 && this->m_is_pointer[pos_leaf]){
                            size_type pos_pointer_or_explicit = idx_pointer_or_explicit(pos_leaf);
                            size_type pos_explicit = idx_explicit(pos_pointer_or_explicit, level);

                            if(m_explicit[pos_pointer_or_explicit]){
                                for(auto const &region : regions) {
                                    size_type min_x, max_x, min_y, max_y;
                                    for (size_type offset_y = region.min_y; offset_y <= region.max_y; ++offset_y) {
                                        for (size_type offset_x = region.min_x; offset_x <= region.max_x; ++offset_x) {
                                            add_value(query,x + offset_x, y + offset_y, n_cols, result);
                                        }
                                    }
                                }

                            }else{
                                //std::cout << "Taking pointer" << std::endl;
                                size_type pos_pointer = idx_pointer(pos_explicit, pos_pointer_or_explicit, level);
                                value_type offset_x, offset_y;
                                size_type pointer;
                                leaf_node_info(pos_pointer, level, pointer, offset_x, offset_y);
                                take_pointer(query, regions, x, y, pointer, offset_x, offset_y, level,
                                             block_size, n_cols, n_rows, result);
                            }
                        }
                    }else if(level >= m_maximum_level){
                        size_type pos_leaf = idx_leaf(idx);
                        if(this->m_is_pointer[pos_leaf]){
                            for(auto const &region : regions) {
                                size_type min_x, max_x, min_y, max_y;
                                for (size_type offset_y = region.min_y; offset_y <= region.max_y; ++offset_y) {
                                    for (size_type offset_x = region.min_x; offset_x <= region.max_x; ++offset_x) {
                                        add_value(query,x + offset_x, y + offset_y, n_cols, result);
                                    }
                                }
                            }
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

        void take_pointer(const region_type &query, std::vector<region_type> &regions,
                          const size_type x, const size_type y,
                          size_type ptr,
                          const value_type offset_x, const value_type offset_y,
                          const size_type level, size_type block_size,
                          const size_type n_cols, const size_type n_rows,
                          std::vector<int> &result){
                value_type new_min_x, new_min_y, length_x, length_y;
                value_type left = regions.front().min_x + offset_x;
                value_type right = regions.back().max_x + offset_x;
                value_type lower = regions.front().min_y + offset_y;
                value_type upper = regions.back().max_y + offset_y;
                /*for(const auto &region : regions) {
                    new_min_x = region.min_x + offset_x;
                    new_min_y = region.min_y + offset_y;
                    length_x = region.max_x - region.min_x;
                    length_y = region.max_y - region.min_y;
                    if(left > new_min_x) left = new_min_x;
                    if(right < new_min_x + length_x) right = new_min_x + length_x;
                    if(lower > new_min_y) lower = new_min_y;
                    if(upper < new_min_y + length_y) upper = new_min_y + length_y;
                    //new_regions.push(region(new_min_x, new_min_x + length_x, new_min_y, new_min_y + length_y));
                }*/
#if BT_VERBOSE
                std::cout << "lower=" << lower << " upper=" << upper << std::endl;
                std::cout << "left=" << left << " right=" << right << std::endl;
                std::cout << "bsize=" << block_size << std::endl;
#endif
                auto l = level;
                length_x = right-left;
                length_y = upper-lower;
                auto shift_x = 0;
                auto shift_y = 0;
                size_type new_x = x - offset_x;
                size_type new_y = y - offset_y;
                while(left + shift_x < 0 || lower + shift_y < 0 ||
                      left + shift_x + length_x >= block_size ||
                      lower + shift_y + length_y >= block_size){
                    auto zth = ptr % this->m_k2;
                    auto p = codes::zeta_order::decode(zth);
                    shift_x += p.first * block_size;
                    shift_y += p.second * block_size;
                    new_x = new_x - p.first * block_size;
                    new_y = new_y - p.second * block_size;
                    //std::cout << "shift_x=" << shift_x << " shift_y=" << shift_y << std::endl;
                    block_size *= this->m_k;
                    l--;
                    ptr = this->m_t_select(ptr / this->m_k2);
                }
#if BT_VERBOSE
                std::cout << "bsize final=" << block_size << std::endl;
                std::cout << "iniciales" << std::endl;
                for(const auto &region : regions){
                    std::cout << "(" << region.min_x << ", " << region.max_x
                    << ")x(" << region.min_y << ", " << region.max_y << ")" << std::endl;
                }
#endif

                for(auto &region : regions) {
                    length_x = region.max_x - region.min_x;
                    length_y = region.max_y - region.min_y;
                    region.min_x = region.min_x + offset_x + shift_x;
                    region.min_y = region.min_y + offset_y + shift_y;
                    region.max_x = region.min_x + length_x;
                    region.max_y = region.min_y + length_y;
                }
#if BT_VERBOSE
                std::cout << "finales" << std::endl;
                for(const auto &region : regions){
                    std::cout << "(" << region.min_x << ", " << region.max_x
                              << ")x(" << region.min_y << ", " << region.max_y << ")" << std::endl;
                }
                std::cout << std::endl;
#endif
                recursive_multiple_region(query, regions, new_x, new_y, ptr, l, block_size, n_cols, n_rows, result,
                                          true, level);
        }


        inline bool taking_pointer_condition(const bool taking_pointer, const size_type level_taking_pointer, const size_type level){
            //std::cout << "condition-> taking_pointer=" << taking_pointer << " level_taking_pointer=" << level_taking_pointer << " level=" << level << std::endl;
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
        const size_type &k = m_k;
        const size_type &height = m_height;
        const size_type &dimensions = m_dimensions;

        block_tree_comp_leaves() = default;

        /*block_tree_comp_leaves(const std::string &file_name, const size_type kparam, const size_type level,
                                                const size_type n_rows=0, const size_type n_cols=0) {
            input_type adjacency_lists;
            reader_type::read(file_name, adjacency_lists, n_rows, n_cols);
            size_type h, total_size;
            this->init_construction(h, total_size, adjacency_lists, kparam);
            size_type blocks = this->m_k2, block_size = total_size/this->m_k;
            construction(file_name, adjacency_lists, h, block_size, blocks, level, n_rows, n_cols);
        }*/



        inline std::vector<size_type> access(const size_type id, const size_type direct_id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            this->recursive_access_region(direct_id, direct_id, id, id, 0, 0, 0, 0, block_size, r, add_in_row());
            return r;
        }


        inline void values_region(const size_type min_x, const size_type min_y,
                                  const size_type max_x, const size_type max_y,
                                  const size_type n_cols, size_type n_rows,
                                  std::vector<int> &result){
            size_type size_vector = (max_y - min_y+1) * (max_x - min_x + 1);
            result = std::vector<int>(size_vector, 0);
            std::vector<region_type> regions;
            for(size_type i = 0; i <= m_msb; ++i){
                regions.emplace_back(region_type{i*n_cols+min_x, i*n_cols+max_x, min_y, max_y});
            }
            region_type query = region_type{min_x, max_x, min_y, max_y};
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            this->recursive_multiple_region(query, regions, 0, 0, 0, 0, block_size, n_cols, n_rows, result);

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


        void traverse_leaf(const size_type idx, const size_type block_size,
                                const size_type x, const size_type y,
                                const sdsl::bit_vector &aux_l,
                                size_type &cw, size_type &to_delete){

            auto new_block_size = block_size / this->m_k;
            auto start_children = this->m_t_rank(idx+1) * this->m_k2;
            for(auto i = 0; i < this->m_k2; ++i) {
                if(new_block_size > 1){
                    if (this->m_t[start_children + i]) {
                        traverse_leaf(start_children + i, new_block_size,
                                      (i % this->m_k) * new_block_size + x,
                                      (i / this->m_k) * new_block_size + y,
                                      aux_l,cw,to_delete);
                    }else {
                        auto zth = idx_leaf(start_children+i);
                        if(m_is_pointer[zth]){
                            auto s_x = (i % this->m_k) * new_block_size + x;
                            auto s_y = (i / this->m_k) * new_block_size + y;
                            for(auto dx = 0; dx <  new_block_size; ++dx){
                                for(auto dy = 0; dy < new_block_size; ++dy){
                                    cw = cw | (1ULL << ((s_y + dy) * leaf_size + (s_x+dx)));
                                }
                            }
                        }
                    }
                    ++to_delete;
                }else{
                    if(aux_l[start_children + i - this->m_t.size()]){
                        auto s_x = (i % this->m_k) + x;
                        auto s_y = (i / this->m_k) + y;
                        cw = cw | (1ULL << (s_y * leaf_size + s_x));
                    }
                    ++to_delete;
                }

            }


        }

        std::pair<size_type, size_type> compute_cw(const size_type idx, const size_type block_size,
                                                   const sdsl::bit_vector &aux_l){
            size_type to_delete = 0;
            size_type cw = 0;
            traverse_leaf(idx, block_size, 0, 0, aux_l, cw, to_delete);
            return {cw, to_delete};
        }



        void traverse(const size_type idx, const size_type block_size, std::unordered_map<size_type, size_type> &freq,
                        std::vector<size_type> &code_leaves, const sdsl::bit_vector &aux_l,
                        size_type &last_t, size_type &prev_ones){
            ++prev_ones;
            auto start_children = this->m_t_rank(idx+1) * this->m_k2;
            auto new_block_size = block_size / this->m_k;
            for(auto i = start_children; i < start_children + this->m_k2; ++i){
                if(new_block_size > leaf_size){
                    if(this->m_t[i]){
                        traverse(i, new_block_size, freq, code_leaves, aux_l, last_t, prev_ones);
                    }
                }else{
                    if(this->m_t[i]){
                        auto pair = compute_cw(i, new_block_size, aux_l);
                        auto it = freq.find(pair.first);
                        if(it != freq.end()){
                            it->second++;
                        }else{
                            freq.insert({pair.first, 1});
                        }
                        code_leaves.push_back(pair.first);
                    }
                    //last_t = std::max(i, last_t);
                    if(last_t < i) last_t = i;
                }

            }

        }

        std::pair<size_type, size_type> shannon_entropy_bits_delete(){
            std::unordered_map<size_type, size_type> freq;
            size_type bits_delete = 0;
            size_type n_leaves = 0;
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            traverse(0, block_size, freq, bits_delete, n_leaves);

            double entropy = 0;
            for(const auto &v: freq){
                entropy += (v.second/ (double) n_leaves) * std::log2(n_leaves / (double) v.second);

                std::cout << "value=" << v.first << " count=" << v.second << std::endl;
                std::cout << std::log2(n_leaves / (double) v.second) << std::endl;
            }
            std::cout << "Number leaves= " << n_leaves << std::endl;
            std::cout << "H0= " << entropy << std::endl;
            auto nh0 = static_cast<size_type >(n_leaves*std::ceil(entropy));
            return {nh0, bits_delete};

        }



        //! Copy constructor
        block_tree_comp_leaves(const block_tree_comp_leaves &p) {
            copy(p);
        }

        //! Move constructor
        block_tree_comp_leaves(block_tree_comp_leaves &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        block_tree_comp_leaves &operator=(block_tree_comp_leaves &&p) {
            if (this != &p) {
                m_k = std::move(p.m_k);
                m_k2 = m_k * m_k;
                m_height = std::move(p.m_height);
                m_dimensions = std::move(p.m_dimensions);
                m_t = std::move(p.m_t);
                m_t_rank = std::move(p.m_t_rank);
                m_t_rank.set_vector(&m_t);
                m_t_select = std::move(p.m_t_select);
                m_t_select.set_vector(&m_t);
                m_is_pointer = std::move(p.m_is_pointer);
                m_is_pointer_rank = std::move(p.m_is_pointer_rank);
                m_is_pointer_rank.set_vector(&m_is_pointer);
                m_level_ones = std::move(p.m_level_ones);
                m_pointers = std::move(p.m_pointers);
                m_offsets = std::move(p.m_offsets);
                m_minimum_level = std::move(p.m_minimum_level);
                m_maximum_level = std::move(p.m_maximum_level);
                m_zeroes = std::move(p.m_zeroes);
                m_explicit = std::move(p.m_explicit);
                m_rank_explicit = std::move(p.m_rank_explicit);
                m_rank_explicit.set_vector(&m_explicit);
                m_msb = std::move(p.m_msb);
                m_l = std::move(p.m_l);
                m_voc = std::move(p.m_voc);
                m_ones_prev_leaves = std::move(p.m_ones_prev_leaves);
            }
            return *this;
        }

        //! Assignment operator
        block_tree_comp_leaves &operator=(const block_tree_comp_leaves &p) {
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
        void swap(block_tree_comp_leaves &p) {
            std::swap(m_k, p.m_k);
            std::swap(m_k2, p.m_k2);
            std::swap(m_dimensions, p.m_dimensions);
            std::swap(m_level_ones, p.m_level_ones);
            std::swap(m_height, p.m_height);
            std::swap(m_t, p.m_t);
            sdsl::util::swap_support(m_t_rank, p.m_t_rank, &m_t, &(p.m_t));
            sdsl::util::swap_support(m_t_select, p.m_t_select, &m_t, &(p.m_t));
            std::swap(m_is_pointer, p.m_is_pointer);
            sdsl::util::swap_support(m_is_pointer_rank, p.m_is_pointer_rank, &m_is_pointer, &(p.m_is_pointer));
            std::swap(m_pointers, p.m_pointers);
            std::swap(m_offsets, p.m_offsets);
            std::swap(m_minimum_level, p.m_minimum_level);
            std::swap(m_maximum_level, p.m_maximum_level);
            std::swap(m_zeroes, p.m_zeroes);
            std::swap(m_explicit, p.m_explicit);
            sdsl::util::swap_support(m_rank_explicit, p.m_rank_explicit, &m_explicit, &(p.m_explicit));
            std::swap(m_msb, p.m_msb);
            std::swap(m_l, p.m_l);
            std::swap(m_voc, p.m_voc);
            std::swap(m_ones_prev_leaves, p.m_ones_prev_leaves);
        }


        //! Serializes the data structure into the given ostream
         size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr,
                            const std::string name="")const
        {
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(
                    v, name, sdsl::util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += sdsl::write_member(m_k, out, child, "k");
            written_bytes += sdsl::write_member(m_height, out, child, "height");
            written_bytes += sdsl::write_member(m_dimensions, out, child, "dimensions");
            written_bytes += m_t.serialize(out, child, "t");
            written_bytes += m_t_rank.serialize(out, child, "t_rank");
            written_bytes += m_t_select.serialize(out, child, "t_select");
            written_bytes += m_level_ones.serialize(out, child, "level_ones");
            written_bytes += m_is_pointer.serialize(out, child, "is_pointer");
            written_bytes += m_is_pointer_rank.serialize(out, child, "is_pointer_rank");
            uint64_t m_pointers_size = m_pointers.size();
            sdsl::write_member(m_pointers_size, out, child, "pointers_size");
            written_bytes += sdsl::serialize_vector(m_pointers, out, child, "pointers");
            uint64_t m_offsets_size = m_offsets.size();
            sdsl::write_member(m_offsets_size, out, child, "offsets_size");
            written_bytes += sdsl::serialize_vector(m_offsets, out, child, "offsets");
            written_bytes += sdsl::write_member(m_minimum_level, out, child, "minimum_level");
            written_bytes += sdsl::write_member(m_maximum_level, out, child, "maximum_level");
            written_bytes += sdsl::write_member(m_zeroes, out, child, "zeroes");
            written_bytes += m_explicit.serialize(out, child, "explicit");
            written_bytes += m_rank_explicit.serialize(out, child, "rank_explicit");
            written_bytes += sdsl::write_member(m_msb, out, child, "msb");
            written_bytes += m_l.serialize(out, child, "l");
            written_bytes += m_voc.serialize(out, child, "voc");
            written_bytes += sdsl::write_member(m_ones_prev_leaves, out, child, "ones_prev_leaves");
            return written_bytes;
        }

        //! Loads the data structure from the given istream.
        void load(std::istream& in)
        {
            sdsl::read_member(m_k, in);
            sdsl::read_member(m_height, in);
            sdsl::read_member(m_dimensions, in);
            m_t.load(in);
            m_t_rank.load(in, &m_t);
            m_t_select.load(in, &m_t);
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
            sdsl::read_member(m_minimum_level, in);
            sdsl::read_member(m_maximum_level, in);
            sdsl::read_member(m_zeroes, in);
            m_explicit.load(in);
            m_rank_explicit.load(in, &m_explicit);
            sdsl::read_member(m_msb, in);
            m_l.load(in);
            m_voc.load(in);
            sdsl::read_member(m_ones_prev_leaves, in);
        }

        //! Loads the data structure from the given istream.
        void from(std::istream& in)
        {
            sdsl::read_member(m_k, in);
            sdsl::read_member(m_height, in);
            sdsl::read_member(m_dimensions, in);
            m_t.load(in);
            m_t_rank.load(in, &m_t);
            m_t_select.load(in, &m_t);
            sdsl::bit_vector l_aux;
            l_aux.load(in);
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
            sdsl::read_member(m_minimum_level, in);
            sdsl::read_member(m_maximum_level, in);
            sdsl::read_member(m_zeroes, in);
            m_explicit.load(in);
            m_rank_explicit.load(in, &m_explicit);
            sdsl::read_member(m_msb, in);

            std::cout << "1" << std::endl;
            m_ones_prev_leaves = 0;
            std::unordered_map<size_type, size_type> hash;
            size_type n_leaves = 0, last_t = 0;
            std::vector<size_type> code_leaves;
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            traverse(0, block_size, hash, code_leaves, l_aux, last_t, m_ones_prev_leaves);

            std::cout << "2" << std::endl;
            //Cutting bitmaps
            auto last_pointer = idx_leaf(last_t);
            m_is_pointer.resize(last_pointer+1);
            sdsl::util::init_support(m_is_pointer_rank, &m_is_pointer);

            m_t.resize(last_t+1);
            sdsl::util::init_support(m_t_rank, &m_t);
            sdsl::util::init_support(m_t_select, &m_t);

            std::cout << "3" << std::endl;
            //Building the vocab
            m_voc.resize(hash.size());
            std::vector<std::pair<size_type, size_type>> vocab_vector (hash.begin(),hash.end());
            hash.clear();
            auto sortby = [](std::pair<size_type, size_type> a, std::pair<size_type, size_type> b) {
                return a.second > b.second;
            };
            std::sort(vocab_vector.begin(), vocab_vector.end(), sortby);
            size_type cw = 0;
            for(const auto &c: vocab_vector){
                hash.insert({c.first, cw});
                m_voc[cw]=c.first;
                ++cw;
            }

            //Encoding the leaves
            for(int j = 0; j < code_leaves.size(); ++j){
                auto it = hash.find(code_leaves[j]);
                code_leaves[j] = it->second;
            }
            hash.clear();
            m_l = sdsl::dac_vector_dp_opt<>(code_leaves);

        }


        void pointers(){
            for(auto level = m_minimum_level; level < m_maximum_level; ++level){
                auto p_offset = 0;
                auto p_no_offset = 0;
                for(size_type i = 0; i < this->m_offsets[level-m_minimum_level].size()/2; ++i){
                    auto offset_x = codes::alternative_code::decode(this->m_offsets[level-m_minimum_level][2*i]);
                    auto offset_y = codes::alternative_code::decode(this->m_offsets[level-m_minimum_level][2*i+1]);
                    if(offset_x == 0 && offset_y == 0){
                        p_no_offset++;
                    }else{
                        p_offset++;
                    }
                }
                std::cout << "Level=" << level << " pointers=" << this->m_pointers[level-m_minimum_level].size() << " p_offset=" << p_offset << " p_no_offset=" << p_no_offset << std::endl;
            }
        }

        void display(){

            std::ofstream out("block_tree.json");
            typedef std::unordered_map<size_type, size_type > map_z_order_type;
            size_type start = this->m_k2, level = 0, elements = this->m_k2;
            size_type block_size = std::pow(2, this->m_height) / this->m_k;
            map_z_order_type map_z_order, next_map_z_order;
            for(size_type i = 0; i < this->m_k2; ++i){
                map_z_order.insert({i, i});
            }
            while(level < this->m_height){
                ++level;
                std::cout << "At level: " << level << " and block_size: " << block_size << std::endl;
                size_type ones = 0;
                size_type i = start;
                size_type new_elements = 0;
                while(i < start + elements){
                    if(this->m_t[i]){
                        auto z_order = map_z_order.find(i-start)->second;
                        auto pos = codes::zeta_order::decode(z_order);
                        auto x = pos.first * block_size;
                        auto y = pos.second * block_size;
                        auto x_i = x + block_size-1;
                        auto y_i = y + block_size-1;
                        for(auto z_next = z_order * this->m_k2; z_next < (z_order+1)*this->m_k2; ++z_next){
                            next_map_z_order.insert({new_elements, z_next});
                            ++new_elements;
                        }
                        out << this->node_to_json(i, z_order, false, false, false, level, x, y, x_i, y_i) << std::endl;
                        ++ones;
                    }else{
                        if(level > m_minimum_level && level < m_maximum_level){
                            size_type pos_leaf = idx_leaf(i);
                            if(i > 0 && this->m_is_pointer[pos_leaf]){
                                auto z_order = map_z_order.find(i-start)->second;
                                auto pos = codes::zeta_order::decode(z_order);
                                auto x = pos.first * block_size;
                                auto y = pos.second * block_size;
                                auto x_i = x + block_size-1;
                                auto y_i = y + block_size-1;
                                size_type pos_pointer_or_explicit = idx_pointer_or_explicit(pos_leaf);
                                size_type pos_explicit = idx_explicit(pos_pointer_or_explicit, level);
                                if(m_explicit[pos_pointer_or_explicit]){
                                    out << this->node_to_json(i, z_order, true, true, true, level, x, y, x_i, y_i) << std::endl;
                                }else{
                                    size_type pos_pointer = this->idx_pointer(pos_explicit, pos_pointer_or_explicit, level);
                                    value_type offset_x, offset_y;
                                    size_type pointer;
                                    this->leaf_node_info(pos_pointer, level, pointer, offset_x, offset_y);
                                    out << this->node_to_json(i, z_order, true, true, false, level, x, y, x_i, y_i,
                                                              pointer, offset_x, offset_y) << std::endl;
                                }
                            }else{
                                auto z_order = map_z_order.find(i-start)->second;
                                auto pos = codes::zeta_order::decode(z_order);
                                auto x = pos.first * block_size;
                                auto y = pos.second * block_size;
                                auto x_i = x + block_size-1;
                                auto y_i = y + block_size-1;
                                out << this->node_to_json(i, z_order, true, false, false, level, x, y, x_i, y_i) << std::endl;
                            };
                        }else{
                            auto z_order = map_z_order.find(i-start)->second;
                            auto pos = codes::zeta_order::decode(z_order);
                            auto x = pos.first * block_size;
                            auto y = pos.second * block_size;
                            auto x_i = x + block_size-1;
                            auto y_i = y + block_size-1;
                            out << this->node_to_json(i, z_order, true, false, false, level, x, y, x_i, y_i) << std::endl;
                        };
                    }
                    ++i;
                }
                start = i;
                elements = ones*this->m_k2;
                block_size = block_size/this->m_k;
                map_z_order.clear();
                std::swap(map_z_order, next_map_z_order);
            }
            out.close();


        }


    };
}
#endif //INC_2D_BLOCK_TREE_block_tree_comp_ones_access_HPP
