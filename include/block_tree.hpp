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

#include <block_tree_algorithm_helper_v2.hpp>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree {

    public:

        typedef uint64_t size_type;
        typedef input_t input_type;
        typedef typename block_tree_2d::algorithm::node_type node_type;
        typedef hash_table::hash_table_chainning<uint64_t , uint64_t> htc_type;

    private:
        size_type m_k;
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
            m_topology = sdsl::bit_vector(BITMAP_RESIZE);
            m_is_pointer = sdsl::bit_vector(BITMAP_RESIZE);
            m_level_ones = sdsl::int_vector<>(ARRAY_RESIZE);
            m_pointers = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(ARRAY_RESIZE, 0));
            m_offsets = std::vector<sdsl::int_vector<>>(1, sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }

        void add_new_pointers_offsets(){
            m_offsets.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
            m_pointers.push_back(sdsl::int_vector<>(ARRAY_RESIZE, 0));
        }


        void compact_current_level(const std::vector<node_type> &nodes, const size_type level, size_type &topology_index, size_type &is_pointer_index) {
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
                    m_offsets[level][offset_index++] = node.offset_x;
                    m_offsets[level][offset_index++] = node.offset_y;
                    m_pointers[level][pointer_index++] = node.ptr;
                }
            }
            m_offsets[level].resize(offset_index);
            m_pointers[level].resize(pointer_index);
            sdsl::util::bit_compress(m_offsets[level]);
            sdsl::util::bit_compress(m_pointers[level]);
        }

    public:

        const size_type &dimensions = m_dimensions;
        //Pre: adjacency_lists have to contain at least one 1
        block_tree(input_type &adjacency_lists, const size_type d, const size_type k){
            m_k = k;
            m_dimensions = d;
            size_type k_pow_2 = k*k;
            size_type blocks = k_pow_2;
            size_type block_size = m_dimensions / k;
            std::vector<node_type> nodes(blocks);
            //Init map between z_order and position in vector nodes
            block_tree_2d::algorithm::hash_type hash;
            for(uint64_t i = 0; i < k_pow_2; ++i){
                hash.insert({i,i});
            }
            size_type level = 1, topology_index = 0, is_pointer_index = 0;
            init_structure();
            std::cout << "Level: " << level << std::endl;
            while (block_size > 1) {
                htc_type m_htc(2*nodes.size()); //2* nodes, in order to reduce resize operations
                block_tree_2d::algorithm::get_fingerprint_blocks(adjacency_lists, m_htc, dimensions, block_size, hash, nodes);
                block_tree_2d::algorithm::get_type_of_nodes(adjacency_lists, m_htc, dimensions, block_size, hash, nodes);
                block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
                compact_current_level(nodes, level, topology_index, is_pointer_index);
                block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, k_pow_2, nodes);
                ++level;
                std::cout << "Level: " << level << std::endl;
                block_size = block_size / k;
            }
            std::cout << std::endl;
        }
    };
}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_HPP
