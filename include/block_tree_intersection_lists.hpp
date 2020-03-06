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

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_INTERSECTION_LISTS_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_INTERSECTION_LISTS_HPP

#include <block_tree_algorithm_helper_v2.hpp>
#include "alternative_code.hpp"
#include <logger.hpp>
#include <block_tree.hpp>
#include <vector>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree_intersection_lists : public block_tree<input_t> {

    public:

        typedef input_t input_type;
        typedef typename block_tree<input_t>::value_type value_type;
        typedef typename block_tree<input_t>::size_type size_type;
        typedef typename block_tree<input_t>::node_type node_type;
        typedef typename block_tree<input_t>::htc_type htc_type;
        typedef typename block_tree<input_t>::htc_multiple_type htc_multiple_type;

    private:

        size_type m_minimum_level;
        size_type m_zeroes;
        static constexpr size_type large_block_size = 1024;

        std::pair<size_type, size_type> minimum_block_size(input_type &adjacency_lists, const size_type height){

            std::cout << "minimum_block_size_aaa" << std::endl;
            if(height <= 1) return {this->k, 1};

            size_type l = height-1;
            size_type rows = adjacency_lists.size() / this->k;
            size_type blocks =  rows * rows;
            size_type block_size = this->k;
            //1. Looking for repetitions by using Karp-Rabin algorithm
            while(block_size <= large_block_size){
                //std::cout << "htc" << std::endl;
                //std::cout << "blocks: " << blocks << std::endl;
                //htc_type m_htc(2*blocks);
                htc_type m_htc(std::min(static_cast<size_type>(10240), 2*blocks));
                util::logger::log("Checking blocks at level=" + std::to_string(l) + " with block_size=" + std::to_string(block_size));
                if(!block_tree_2d::algorithm::contains_identical_blocks(adjacency_lists, this->k, m_htc, this->dimensions, block_size)){
                    util::logger::log("Checking rolls at level=" + std::to_string(l) + " with block_size=" + std::to_string(block_size));
                    if(!block_tree_2d::algorithm::contains_identical_rolls(adjacency_lists, this->k, m_htc, this->dimensions, block_size)){
                        return {block_size, l};
                    }
                }
                block_size = block_size * this->m_k;
                blocks = blocks / this->m_k2;
                --l;
            }
            if(l == 0) return {block_size, l};
            //2. Looking for repetitions by using intersection of lists
            std::cout << "blocks: " << blocks * this->m_k2 << std::endl;
            htc_multiple_type m_htc_multiple(std::min(static_cast<size_type>(10240), 2*blocks * this->m_k2));
            //htc_multiple_type m_htc_multiple( blocks * this->m_k2);
            block_tree_2d::algorithm::replacements_map_type replacements_map;
            //block_tree_2d::algorithm::replacements_point_lists_type replacements_point_lists;
            util::logger::log("Checking replacements at blocks=" + std::to_string(l+1) + " with block_size=" + std::to_string(block_size/ this->k));
            block_tree_2d::algorithm::list_blocks(adjacency_lists, this->k, m_htc_multiple, this->dimensions, large_block_size, replacements_map);
            //block_tree_2d::algorithm::list_blocks_v2(adjacency_lists, this->k, m_htc_multiple, this->dimensions, large_block_size);
            m_htc_multiple.print_stats();

            util::logger::log("Checking replacements at rolls=" + std::to_string(l+1) + " with block_size=" + std::to_string(block_size / this->k));
            block_tree_2d::algorithm::list_rolls(adjacency_lists, this->k, m_htc_multiple, this->dimensions, large_block_size, replacements_map);
            //block_tree_2d::algorithm::list_rolls_v2(adjacency_lists, this->k, m_htc_multiple, this->dimensions, large_block_size, replacements_point_lists);
            while(l > 0){
                bool b = block_tree_2d::algorithm::exist_replacements(adjacency_lists, replacements_map, block_size,
                         large_block_size, this->dimensions, this->k);
                if(!b){
                    return {block_size, l};
                }
                block_size = block_size * this->m_k;
                blocks = blocks / this->m_k2;
                --l;
            }
            return {block_size, l};
        }


        template <class input_type>
        size_type processing_level_with_large_blocks(size_type level, input_type &adjacency_lists, size_type block_size,
                                                size_type &topology_index, size_type &is_pointer_index,
                                                typename block_tree_2d::algorithm::hash_type &hash, std::vector<node_type> &nodes) {


            htc_multiple_type m_htc_multiple(std::min(static_cast<size_type>(10240), static_cast<size_type>(2 * nodes.size())));
            block_tree_2d::algorithm::replacements_map_type replacements_map;
            block_tree_2d::algorithm::sources_map_type sources_map;
            block_tree_2d::algorithm::blocks_replace_map_type blocks_replace_map;
            util::logger::log("Checking blocks at level=" + std::to_string(level) + " with block_size=" +
                              std::to_string(large_block_size));
            block_tree_2d::algorithm::list_blocks(adjacency_lists, this->k, m_htc_multiple, this->dimensions,
                                                  large_block_size, replacements_map);
            util::logger::log("Checking rolls at level=" + std::to_string(level) + " with block_size=" +
                              std::to_string(large_block_size));
            block_tree_2d::algorithm::list_rolls(adjacency_lists, this->k, m_htc_multiple, this->dimensions,
                                                 large_block_size, replacements_map);

            util::logger::log("Replacements with block_size=" +std::to_string(block_size));

            block_tree_2d::algorithm::update_replacements(adjacency_lists, replacements_map, block_size, large_block_size,
                                                          this->dimensions, this->k);
            /*auto b_size = large_block_size;
            while(b_size < block_size){
                b_size = b_size * this->k;
                block_tree_2d::algorithm::replacements_to_prev_level(adjacency_lists, replacements_map, b_size,
                                                                     adjacency_lists.size(), this->k);
            }*/

            //Building sources hashtable
            for(const auto &r : replacements_map){
                auto x_target = r.first.first;
                auto y_target = r.first.second;
                auto z_order = codes::zeta_order::encode(x_target / block_size, y_target/block_size);
                //blocks_replace_map.insert({z_order, 1});
                auto pos_source = hash.find(z_order)->second;
                nodes[pos_source].type = NODE_INTERNAL;
                nodes[pos_source].z_order = z_order;
                for(const auto &s : r.second){
                    size_type x_src = x_target + s.first;
                    size_type y_src = y_target + s.second;
                    auto it = sources_map.find({x_src, y_src});
                    if(it != sources_map.end()){
                        it->second.push_back(z_order);
                    }else{
                        std::vector<size_type> z_vec = {z_order};
                        sources_map.insert({{x_src, y_src}, z_vec});
                    }
                }
            }
            replacements_map.clear();

            std::unordered_map<size_type, char> untouchable_block;
            block_tree_2d::algorithm::get_block_replacements(adjacency_lists, this->k, sources_map,
                    this->dimensions, block_size, hash, nodes, untouchable_block);
            block_tree_2d::algorithm::get_roll_replacements_v2(adjacency_lists, this->k, sources_map,
                                                             this->dimensions, block_size, hash, nodes, untouchable_block);
            block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
            util::logger::log("Compacting level=" + std::to_string(level));
            auto pointers = this->compact_current_level(nodes, level, topology_index, is_pointer_index);
            util::logger::log("Number of new pointers=" + std::to_string(pointers));
            util::logger::log("Preparing next level");
            block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, this->m_k2, nodes);
            return pointers;
        }


        void construction(input_type &adjacency_lists, size_type h, size_type block_size, size_type blocks){


            std::cout << "Construction intersection lists" << std::endl;
            //0. Init the structure
            this->init_structure();
            this->m_offsets[0].resize(0);
            this->m_pointers[0].resize(0);

            //1. Obtaining minimum block size where there are identical blocks
            //size_type min_block_size = 16;
            //m_minimum_level = 16;
            size_type min_block_size;
            std::tie(min_block_size, m_minimum_level) = this->minimum_block_size(adjacency_lists, h);
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
            size_type level = m_minimum_level;
            block_size = min_block_size / this->m_k;
            //Traiting large blocks
            while (block_size > large_block_size){
                ++level;
                util::logger::log("Processing level " + std::to_string(level) + " of " + std::to_string(h));
                util::logger::log("Block size: " + std::to_string(block_size));
                auto pointers = processing_level_with_large_blocks(level - m_minimum_level, adjacency_lists, block_size,
                        topology_index, is_pointer_index, hash, nodes);
                util::logger::log("Pointers: " + std::to_string(pointers));
                block_size = block_size / this->k;
            }
            //std::cout << "Level: " << level << std::endl;
            while (block_size > 1) {
                ++level;
                util::logger::log("Processing level " + std::to_string(level) + " of " + std::to_string(h));
                util::logger::log("Block size: " + std::to_string(block_size));
                auto pointers = this->processing_level(level - m_minimum_level, adjacency_lists, block_size,
                        topology_index, is_pointer_index, hash, nodes);
                util::logger::log("Pointers: " + std::to_string(pointers));
                block_size = block_size / this->k;

            }
            ++level;
            util::logger::log("Processing last level (" + std::to_string(level) + ")");
            block_tree_2d::algorithm::compute_last_level(adjacency_lists, hash, nodes);
            util::logger::log("Compacting last level (" + std::to_string(level) + ")");
            this->compact_last_level(nodes, topology_index);
            this->m_height = level;
            this->m_topology.resize(topology_index);
            this->m_is_pointer.resize(is_pointer_index);
            this->m_level_ones.resize(2*(this->m_height - m_minimum_level));
            sdsl::util::init_support(this->m_topology_rank, &this->m_topology);
            sdsl::util::init_support(this->m_topology_select, &this->m_topology);
            sdsl::util::init_support(this->m_is_pointer_rank, &this->m_is_pointer);
            sdsl::util::bit_compress(this->m_level_ones);
            util::logger::log("2D Block Tree DONE!!!");
        }

        void copy(const block_tree_intersection_lists &p){
            block_tree<input_type >::copy(p);
            m_zeroes = p.m_zeroes;
            m_minimum_level = p.m_minimum_level;
        }


    public:

        block_tree_intersection_lists() = default;

        block_tree_intersection_lists(input_type &adjacency_lists, const size_type kparam) {
            size_type h, total_size;
            this->init_construction(h, total_size, adjacency_lists, kparam);
            size_type blocks = this->m_k2, block_size = total_size/this->m_k;
            construction(adjacency_lists, h, block_size, blocks);
        }


        inline bool taking_pointer_condition(const bool taking_pointer, const size_type level_taking_pointer, const size_type level){
            return level > m_minimum_level && !(taking_pointer && level <= level_taking_pointer);
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
            if(level <= m_minimum_level){
                return false;
            }
            pos_zero= this->idx_leaf(idx);
            return (idx > 0 && this->m_is_pointer[pos_zero]);
        }

        //! Copy constructor
        block_tree_intersection_lists(const block_tree_intersection_lists &p) {
            copy(p);
        }

        //! Move constructor
        block_tree_intersection_lists(block_tree_intersection_lists &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        block_tree_intersection_lists &operator=(block_tree_intersection_lists &&p) {
            if (this != &p) {
                block_tree<input_type>::operator=(p);
                m_minimum_level = std::move(p.m_minimum_level);
                m_zeroes = std::move(p.m_zeroes);
            }
            return *this;
        }

        //! Assignment operator
        block_tree_intersection_lists &operator=(const block_tree_intersection_lists &p) {
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
        void swap(block_tree_intersection_lists &p) {
            block_tree<input_type>::swap(p);
            std::swap(m_minimum_level, p.m_minimum_level);
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
            written_bytes += sdsl::write_member(m_zeroes, out, child, "zeroes");
            return written_bytes;
        }

        //! Loads the data structure from the given istream.
        void load(std::istream& in)
        {
            block_tree<input_type>::load(in);
            sdsl::read_member(m_minimum_level, in);
            sdsl::read_member(m_zeroes, in);
        }


    };
}
#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_HYBRID_SUBTREE_HPP