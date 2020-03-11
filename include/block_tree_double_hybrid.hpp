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

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_DOUBLE_HYBRID_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_DOUBLE_HYBRID_HPP

#include <block_tree_algorithm_helper_v2.hpp>
#include "alternative_code.hpp"
#include <logger.hpp>
#include <block_tree.hpp>
#include <vector>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree_double_hybrid : public block_tree<input_t> {

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
        static constexpr size_type large_block_size = 1024;







        void construction(input_type &adjacency_lists, size_type h, size_type block_size, size_type blocks,
                          size_type last_block_size_k2_tree){


            std::cout << "Construction hybrid_blocktree with height=" << h << std::endl;
            //0. Init the structure
            this->init_structure();
            this->m_offsets[0].resize(0);
            this->m_pointers[0].resize(0);

            //1. Obtaining minimum block size where there are identical blocks
            //size_type min_block_size = 16;
            //m_minimum_level = 16;

            size_type min_block_size = last_block_size_k2_tree;
            m_minimum_level = h - log2(min_block_size);
            util::logger::log("Minimum level=" + std::to_string(m_minimum_level) + " and block_size=" + std::to_string(min_block_size));
            //exit(10);

            //2. Building LOUDS of k2_tree until min_block_size and map between z_order and position in vector nodes
            block_tree_2d::algorithm::hash_type hash;
            m_zeroes = block_tree_2d::algorithm::build_k2_tree(adjacency_lists, this->k, h, min_block_size, this->m_topology, hash);

            /*for(size_type i = 0; i < this->m_topology.size(); ++i){
                std::cout << this->m_topology[i] << ", ";
            }*/
            std::cout << std::endl;
            std::vector<node_type> nodes(hash.size());
            size_type topology_index = this->m_topology.size(), is_pointer_index = 0;
            size_type l = m_minimum_level;
            block_size = min_block_size / this->m_k;
            bool last_k2_tree = false;
            while (!last_k2_tree && block_size > 1) {
                ++l;
                util::logger::log("Processing level " + std::to_string(l) + " of " + std::to_string(h));
                util::logger::log("Block size: " + std::to_string(block_size));
                htc_type m_htc(std::min(10240UL, 2*nodes.size()));
                util::logger::log("Computing fingerprint of blocks at level=" + std::to_string(l));
                block_tree_2d::algorithm::get_fingerprint_blocks_stack_lite(adjacency_lists, this->k, m_htc, this->dimensions, block_size, hash, nodes, true);
                util::logger::log("Computing fingerprint of shifts at level=" + std::to_string(l));
                block_tree_2d::algorithm::get_type_of_nodes_stack_lite(adjacency_lists, this->k, m_htc, this->dimensions, block_size, hash, nodes);

                size_type bits_k2_tree = 0, leaf_nodes = 0, bits_per_offset = 0, bits_per_pointer = 0;
                for(const auto &node: nodes){
                    if(node.type == NODE_LEAF){
                        bits_k2_tree += node.bits;
                        auto b_offx = sdsl::bits::hi(codes::alternative_code::encode(node.offset_x))+1;
                        if(bits_per_offset < b_offx){
                            bits_per_offset = b_offx;
                        }
                        auto b_offy = sdsl::bits::hi(codes::alternative_code::encode(node.offset_y))+1;
                        if(bits_per_offset < b_offy){
                            bits_per_offset = b_offy;
                        }
                        auto b_ptr = sdsl::bits::hi(node.ptr)+1;
                        if(bits_per_pointer < b_ptr){
                            bits_per_pointer = b_ptr;
                        }
                        ++leaf_nodes;
                    }
                }
                //last_k2_tree = (bits_k2_tree < leaf_nodes*(bits_per_pointer + 2*bits_per_offset + 2));
                last_k2_tree = block_size < 4;
                if(last_k2_tree){
                    this->m_topology.resize(topology_index);
                    m_maximum_level = l;
                    size_type height_subtree = h - l +1; //TODO: ojo con +1
                    std::cout << "Height: " << h << " current_level: " << l << " height_subtree: " << height_subtree << std::endl;
                    block_tree_2d::algorithm::build_last_k2_tree(adjacency_lists, this->k, height_subtree, block_size, this->m_topology);
                    std::cout << "Topology size: " << this->m_topology.size() << std::endl;
                }else{
                    util::logger::log("Clearing adjacency lists at level=" + std::to_string(l));
                    block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
                    util::logger::log("Compacting level=" + std::to_string(l));
                    auto pointers = this->compact_current_level(nodes, l - m_minimum_level, topology_index, is_pointer_index);
                    util::logger::log("Number of new pointers=" + std::to_string(pointers));
                    util::logger::log("Preparing next level");
                    block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, this->m_k2, nodes);
                    block_size = block_size / this->k;
                    std::cout << "Topology size: " << this->m_topology.size() << std::endl;
                }

            }
            if(!last_k2_tree){
                ++l;
                util::logger::log("Processing last level (" + std::to_string(l) + ")");
                block_tree_2d::algorithm::compute_last_level(adjacency_lists, hash, nodes);
                util::logger::log("Compacting last level (" + std::to_string(l) + ")");
                this->compact_last_level(nodes, topology_index);
                this->m_topology.resize(topology_index);
            }
            //m_maximum_level = h+1;
            this->m_height = h;
            this->m_is_pointer.resize(is_pointer_index);
            this->m_level_ones.resize(2*(this->m_height - m_minimum_level));
            sdsl::util::init_support(this->m_topology_rank, &this->m_topology);
            sdsl::util::init_support(this->m_topology_select, &this->m_topology);
            sdsl::util::init_support(this->m_is_pointer_rank, &this->m_is_pointer);
            sdsl::util::bit_compress(this->m_level_ones);
            util::logger::log("2D Block Tree DONE!!!");
        }


        void copy(const block_tree_double_hybrid &p){
            block_tree<input_type >::copy(p);
            m_zeroes = p.m_zeroes;
            m_minimum_level = p.m_minimum_level;
            m_maximum_level = p.m_maximum_level;
        }


    public:

        const size_type &minimum_level = m_minimum_level;
        const size_type &maximum_level = m_maximum_level;
        block_tree_double_hybrid() = default;

        block_tree_double_hybrid(input_type &adjacency_lists, const size_type kparam, const size_type level) {
            size_type h, total_size;
            this->init_construction(h, total_size, adjacency_lists, kparam);
            size_type blocks = this->m_k2, block_size = total_size/this->m_k;
            construction(adjacency_lists, h, block_size, blocks, level);
        }


        inline bool taking_pointer_condition(const bool taking_pointer, const size_type level_taking_pointer, const size_type level){
            return level > m_minimum_level && level < m_maximum_level && !(taking_pointer && level <= level_taking_pointer);
        }

        inline size_type idx_leaf(const size_type idx){
            return idx - this->m_topology_rank(idx+1) - m_zeroes;
        }

         void leaf_node_info(const size_type pos_zero, const size_type level,
                                    size_type &pointer, value_type &offset_x, value_type &offset_y){

            //std::cout << "level: " << level << std::endl;
            //std::cout << "rank: " << this->m_is_pointer_rank(pos_zero+1) << std::endl;
            //std::cout << "pos: " << 2*(level-m_minimum_level-1)+1 << std::endl;
            //std::cout << "val: " << this->m_level_ones[2*(level-m_minimum_level-1)+1] << std::endl;
            size_type p_pointer = this->m_is_pointer_rank(pos_zero+1)- this->m_level_ones[2*(level-m_minimum_level-1)+1] -1;
            pointer = this->m_level_ones[2*(level-m_minimum_level)]+ this->m_pointers[level-m_minimum_level][p_pointer];
            offset_x = codes::alternative_code::decode(this->m_offsets[level-m_minimum_level][2*p_pointer]);
            offset_y = codes::alternative_code::decode(this->m_offsets[level-m_minimum_level][2*p_pointer+1]);
           // std::cout << "Pointer at: " << p_pointer << " with offset: (" << offset_x << ", " << offset_y << ") at level="
           //<< level << std::endl;
        }

        bool is_pointer(size_type idx, size_type level, size_type &pos_zero){
            if(level <= m_minimum_level || level >= m_maximum_level){
                return false;
            }
            pos_zero= this->idx_leaf(idx);
            return (idx > 0 && this->m_is_pointer[pos_zero]);
        }

        //! Copy constructor
        block_tree_double_hybrid(const block_tree_double_hybrid &p) {
            copy(p);
        }

        //! Move constructor
        block_tree_double_hybrid(block_tree_double_hybrid &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        block_tree_double_hybrid &operator=(block_tree_double_hybrid &&p) {
            if (this != &p) {
                block_tree<input_type>::operator=(p);
                m_minimum_level = std::move(p.m_minimum_level);
                m_maximum_level = std::move(p.m_maximum_level);
                m_zeroes = std::move(p.m_zeroes);
            }
            return *this;
        }

        //! Assignment operator
        block_tree_double_hybrid &operator=(const block_tree_double_hybrid &p) {
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
        void swap(block_tree_double_hybrid &p) {
            block_tree<input_type>::swap(p);
            std::swap(m_minimum_level, p.m_minimum_level);
            std::swap(m_maximum_level, p.m_maximum_level);
            std::swap(m_zeroes, p.m_zeroes);
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
            return written_bytes;
        }

        //! Loads the data structure from the given istream.
        void load(std::istream& in)
        {
            block_tree<input_type>::load(in);
            sdsl::read_member(m_minimum_level, in);
            sdsl::read_member(m_maximum_level, in);
            sdsl::read_member(m_zeroes, in);
        }


    };
}
#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_DOUBLE_HYBRID_HPP
