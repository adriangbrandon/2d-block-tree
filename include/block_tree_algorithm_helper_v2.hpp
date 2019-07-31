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
// Created by Adrián on 23/07/2019.
//

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_V2_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_V2_HPP

#include <kr_block_adjacent_list_v2.hpp>
#include <kr_roll_adjacent_list_v2.hpp>
#include <hash_table_chainning.hpp>
#include <sdsl/bit_vectors.hpp>
#include <zeta_order.hpp>
#include <unordered_map>
#include <iostream>
#include <bithacks.hpp>

#define NODE_EMPTY 0
#define NODE_LEAF 1
#define NODE_INTERNAL 2

namespace block_tree_2d {

    class algorithm {

    public:
        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef std::unordered_map<size_type, size_type > hash_type;
        typedef struct {
            size_type z_order = 0;
            size_type type = NODE_EMPTY;
            value_type offset_x = 0;
            value_type offset_y = 0;
            size_type ptr = 0;
            size_type hash = 0;
        } node_type;
        class compare_abs_value {
        public:
            bool operator()(const int64_t &x, const int64_t &y){
                return util::bithacks::abs(x) < util::bithacks::abs(y);
            }
        };

    private:

        static void range_block(size_type id_b,
                                value_type &start_x, value_type &end_x, value_type &start_y, value_type &end_y,
                                const size_type block_size){
            auto pos_b = codes::zeta_order::decode(id_b);
            //start_x = pos_b.first * block_size;
            start_x = pos_b.first * block_size;
            end_x = start_x + block_size;
            //start_y = pos_b.second * block_size+1;
            start_y = pos_b.second * block_size;
            end_y = start_y + block_size;
        }

        template <class input_type, class iterators_type>
        static size_type bits_subtree(const input_type &adjacent_lists, const value_type sx_b2, const value_type ex_b2,
                                      const value_type sy_b2, const value_type ey_b2, const iterators_type &iterators_b2,
                                      const size_type block_size, const size_type k){

            size_type row = 0, bits = 0;
            size_type k_pow_2 = k*k;
            hash_type hash0, hash1;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                while(it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    size_type x = *it_b2 - sx_b2;
                    auto z_order = codes::zeta_order::encode(x, sy_b2+row)/k_pow_2;
                    auto it = hash0.find(z_order);
                    if(it == hash0.end()){
                        hash0.insert(it, {z_order,1});
                    }
                }
                ++row;
            }
            bits += hash0.size() * k_pow_2;
            auto b = block_size/k;
            while(b > 1){
                for(const auto &k_v : hash0){
                    auto z_order = k_v.first / k_pow_2;
                    auto it = hash1.find(z_order);
                    if(it == hash1.end()){
                        hash1.insert(it, {z_order,1});
                    }
                }
                hash0.clear();
                auto temp = std::move(hash0);
                hash0 = std::move(hash1);
                hash1 = std::move(temp);
                bits += hash0.size() * k_pow_2;
                b = b/k;
            }
            bits += 1;
            return bits;

        }



        template <class input_type, class iterators_type>
        static bool are_identical(input_type &adjacent_lists,
                                  const size_type id_b1,
                                  const value_type sx_b2, const value_type ex_b2,
                                  const value_type sy_b2, const value_type ey_b2,
                                  const iterators_type &iterators_b2,
                                  const size_type block_size,
                                  value_type &sx_b1, value_type &sy_b1,
                                  value_type &ex_b1, value_type &ey_b1,
                                  iterators_type &iterators_b1){

            //Compute the range of z-block
            range_block(id_b1, sx_b1, ex_b1, sy_b1, ey_b1, block_size);
            //if(sx_b1 == sx_b2 && sy_b1 == sy_b2) return false; //are the same
            if(!(sx_b1 > ex_b2 || ex_b1 < sx_b2 || sy_b1 > ey_b2 || ey_b1 < sy_b2)){
                return false;
            }
            size_type row = 0;
            while(row < block_size){
                auto beg_list_b1 = (adjacent_lists.begin() + sy_b1 + row)->begin();
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                auto it_b1 = std::upper_bound(beg_list_b1, (adjacent_lists.begin()+sy_b1+row)->end(), ex_b1, compare_abs_value());
                iterators_b1[(sy_b1 + row) % block_size] = it_b1;
                while(it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    size_type d2 = *it_b2 - sx_b2;
                    if(it_b1 == beg_list_b1 || *(--it_b1) < sx_b1) return false;
                    size_type d1 = *it_b1 - sx_b1;
                    if(d1 != d2) return false;
                }
                if(it_b1 != beg_list_b1 && *(--it_b1) >= sx_b1){
                    return false;
                }
                ++row;
            }
            return true;
        }



        template <class input_type, class list_hash_values_type, class iterators_type>
        static bool exist_identical(input_type &adjacent_lists,
                                  const value_type sx_b2, const value_type sy_b2,
                                  const value_type ex_b2, const value_type ey_b2,
                                  const iterators_type &current_iterators,
                                  list_hash_values_type &hash_values,
                                  const size_type block_size,
                                  value_type &sx_b1, value_type &sy_b1,
                                  value_type &ex_b1, value_type &ey_b1,
                                  iterators_type &iterators_b1,
                                  typename list_hash_values_type::iterator &result){

            auto it = hash_values.begin();
            while(it != hash_values.end()){
                if(are_identical(adjacent_lists, it->first, sx_b2, ex_b2, sy_b2, ey_b2, current_iterators,
                        block_size, sx_b1, sy_b1, ex_b1, ey_b1, iterators_b1)){
                    it->second = true;
                    result = it;
                    return true;
                }
                ++it;
            }
            return false;

        }

        template <class input_type, class iterators_type, class heap_type>
        static void delete_info_shift(input_type &adjacent_lists,
                                      const value_type sx_b2, const value_type sy_b2,
                                      const value_type ex_b2, const value_type ey_b2,
                                      const iterators_type &iterators_b2,
                                      const value_type sy_b1,
                                      iterators_type &kr_iterators,
                                      heap_type &heap_in,
                                      heap_type &heap_out,
                                      const size_type block_size){

            size_type row = 0;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                auto last = it_b2;
                auto count = 0;
                while(!(adjacent_lists.begin() + sy_b2 + row)->empty() &&
                       it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2) {
                    //auto new_it = (adjacent_lists.begin() + sy_b2 + row)->erase(it_b2);
                    *it_b2 = -(*it_b2+1);
                    last = it_b2;
                    ++count;
                }

                //IMPORTANT: what happen when the next 1 is going to be deleted?
                //- We have to update the iterator of kr_iterator to the next one
                //- We have to update the heap with the new kr_iterator (if it is pointing to a number)
                auto cyclic_b1 = (sy_b1 + row) % block_size;
                if(count > 0 && last == kr_iterators[cyclic_b1]){
                    kr_iterators[cyclic_b1] = last + count;
                    if(kr_iterators[cyclic_b1] != (adjacent_lists.begin() + sy_b1 + row)->end()){
                        heap_in.update_top({kr_iterators[cyclic_b1], heap_in.top().second});
                    }else{
                        heap_in.pop();
                    }
                }
                ++row;
            }

        }


        template <class input_type, class iterators_type>
        static void delete_info_block(input_type &adjacent_lists,
                                      const value_type sx_b2, const value_type sy_b2,
                                      const value_type ex_b2, const value_type ey_b2,
                                      iterators_type &iterators_b2,
                                      const size_type block_size){

            size_type row = 0;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                //++iterators_b2[(sy_b2 + row) % block_size];
                while(!(adjacent_lists.begin() + sy_b2 + row)->empty() &&
                      it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    //it_b2 = (adjacent_lists.begin() + sy_b2 + row)->erase(it_b2);
                    *it_b2 = -(*it_b2+1);
                    //std::cout << "Row: " << sy_b2 + row << std::endl;
                   // std::cout << "Deleted " << *it_b2 << std::endl;
                   // std::cout << "New pointer to " << *(iterators_b2[(sy_b2 + row) % block_size]) << std::endl;
                }
                ++row;
            }
        }

        static inline void compute_topleft_and_offsets(const value_type col, const value_type row, const size_type block_size,
                                                       size_type &c, size_type &r, size_type &off_x, size_type &off_y){

            c = col/block_size;
            r = row/block_size;
            off_x = col - c*block_size;
            off_y = row - r*block_size;
        }

        template<class input_type>
        static void print_ajdacent_list(input_type &adjacent_list){
            auto it = adjacent_list.begin();
            auto list_id = 0;
            while(it != adjacent_list.end()){
                std::cout << "List " << list_id << " {";
                auto it_e = it->begin();
                while(it_e != it->end()){
                    std::cout << (*it_e) << ", ";
                    ++it_e;
                }
                std::cout << "}" << std::endl;
                ++it;
                ++list_id;
            }
        }

        /*static size_type compute_position_block(const size_type z_order,
                                                const rank_type &rank){
            return rank(z_order+1)-1;
        }*/








    public:
        static constexpr uint64_t prime = 3355443229;

        template <class input_type>
        static void clear_adjacency_lists(input_type &adjacency_lists){
            for(auto &a : adjacency_lists){
                auto write = 0;
                for(auto i = 0; i < a.size(); ++i){
                    if(a[i]>=0){
                        a[write] = a[i];
                        ++write;
                    }
                }
                a.resize(write);
            }
        }

        template <class input_type>
        static void prepare_next_level(input_type &adjacency_lists, hash_type &hash,
                                       const size_type k_pow_2,
                                       std::vector<node_type> &nodes){

            size_type n = hash.size();
            hash.clear();
            size_type number_nodes = 0;
            for(const auto &node : nodes){
                if(node.type == NODE_INTERNAL){
                    for(size_type i = node.z_order * k_pow_2; i < (node.z_order+1)*k_pow_2; ++i){
                        hash.insert({i, number_nodes});
                        ++number_nodes;
                    }
                }
            }
            nodes.clear();
            nodes = std::vector<node_type>(number_nodes);

        }

        template <class input_type, class hash_table_type>
        static void get_fingerprint_blocks(input_type &adjacent_lists, hash_table_type &ht,
                                           const size_type dimensions, const size_type block_size,
                                           hash_type &hash, std::vector<node_type> &nodes){

            typedef karp_rabin::kr_block_adjacent_list_v2<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            kr_type kr_block(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_source = iterators_value_type(block_size);
            //IMPORTANT: kr_block, skips every empty block. For this reason, every node of the current level has to be
            //           initialized as empty node.
            while(kr_block.next()){
#if VERBOSE
                std::cout << "Target: (" << kr_block.col << ", " << kr_block.row << ")" << std::endl;
                std::cout << "Hash: " << kr_block.hash << std::endl;
#endif
                //check if kr_block.hash exists in map
                //Target z-order
                auto z_order_target = codes::zeta_order::encode(kr_block.col, kr_block.row);
                auto pos_target = hash.find(z_order_target)->second;
                if(ht.hash_collision(kr_block.hash, it_table, it_hash)){
                    value_type sx_source, sy_source, ex_source, ey_source;
                    value_type sx_target = kr_block.col * block_size;
                    value_type sy_target = kr_block.row * block_size;
                    value_type ex_target = sx_target + block_size-1;
                    value_type ey_target = sy_target + block_size-1;
                    iterator_hash_value_type source;
                    if(exist_identical(adjacent_lists, sx_target, sy_target, ex_target, ey_target,
                                       kr_block.iterators, it_hash->second, block_size,
                                       sx_source, sy_source, ex_source, ey_source, iterators_source, source)){ //check if they are identical
#if VERBOSE
                        std::cout << "Pointer to source in z-order: " << (source->first) << " offset: <0,0>" << std::endl;
#endif

                        //Add pointer and offset [<p, <x,y>>]
                        //hash.erase(it);
                        auto pos_source = hash.find(source->first)->second;
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = pos_source;
                        nodes[pos_target].hash = kr_block.hash;
                        nodes[pos_target].z_order = z_order_target;
                        hash.erase(z_order_target);
                        //Delete info of <target> from adjacency list
                        delete_info_block(adjacent_lists, sx_target, sy_target, ex_target, ey_target, kr_block.iterators,  block_size);
                    }else{
                        //add <z_order> to chain in map
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);
                        nodes[pos_target].type = NODE_INTERNAL;
                        nodes[pos_target].hash = kr_block.hash;
                        nodes[pos_target].z_order = z_order_target;
                    }
                }else{
                    //add <z_order> to map [chain should be empty]
                    size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, z_order);
                    nodes[pos_target].type = NODE_INTERNAL;
                    nodes[pos_target].hash = kr_block.hash;
                    nodes[pos_target].z_order = z_order_target;
                }
#if VERBOSE
                std::cout << std::endl;
#endif
            }
            //Delete sources of hash_table
            ht.remove_marked();
            //print_ajdacent_list(adjacent_lists);
        }

        template <class input_type, class hash_table_type>
        static void get_type_of_nodes(input_type &adjacent_lists, hash_table_type &ht,
                               const size_type dimensions, const size_type block_size,
                               hash_type &hash, std::vector<node_type> &nodes){

            typedef karp_rabin::kr_roll_adjacent_list_v2<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            kr_type kr_roll(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_target = iterators_value_type(block_size);
            while(kr_roll.next()){
#if VERBOSE
                std::cout << "Source: (" << kr_roll.col << ", " << kr_roll.row << ")" << std::endl;
                std::cout << "Hash: " << kr_roll.hash << std::endl;
#endif
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_roll.hash, it_table, it_hash)){
                    value_type sx_target, sy_target, ex_target, ey_target;
                    value_type ex_source = kr_roll.col + block_size-1;
                    value_type ey_source = kr_roll.row + block_size-1;
                    iterator_hash_value_type target;
                    if(exist_identical(adjacent_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                             kr_roll.iterators, it_hash->second, block_size,
                                             sx_target, sy_target, ex_target, ey_target, iterators_target, target)){ //check if they are identical
#if VERBOSE
                        std::cout << "Target: (" << sx_target << ", " << sy_target << ")" << std::endl;
                        std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
#endif
                        //Compute offsets and top-left block
                        size_type x_block, y_block;
                        size_type source_ptr, source_off_x, source_off_y, off_x, off_y;
                        compute_topleft_and_offsets(kr_roll.col, kr_roll.row, block_size, x_block, y_block, off_x, off_y);
                        //Delete <target> from hash_table
                        ht.remove_value(it_table, it_hash, target);
                        //Delete sources from hash_table
                        //bottom-right block
                        if(off_x && off_y){
                            auto z_bottom_right = codes::zeta_order::encode(x_block + 1, y_block + 1);
                            auto it_bottom_right = hash.find(z_bottom_right);
                            if(it_bottom_right != hash.end()){
                                ht.remove_value(nodes[it_bottom_right->second].hash, z_bottom_right);
                                source_ptr = it_bottom_right->second;
                                source_off_x = kr_roll.col - (x_block + 1)*block_size;
                                source_off_y = kr_roll.row - (y_block + 1)*block_size;
                            }
                        }

                        //bottom-left block
                        if(off_y){
                            auto z_bottom_left = codes::zeta_order::encode(x_block, y_block + 1);
                            auto it_bottom_left = hash.find(z_bottom_left);
                            if(it_bottom_left != hash.end()){
                                ht.remove_value(nodes[it_bottom_left->second].hash, z_bottom_left);
                                source_ptr = it_bottom_left->second;
                                source_off_x = kr_roll.col - (x_block)*block_size;
                                source_off_y = kr_roll.row - (y_block + 1)*block_size;
                            }
                        }

                        //top-right block
                        if(off_x){
                            auto z_top_right = codes::zeta_order::encode(x_block+1, y_block);
                            auto it_top_right = hash.find(z_top_right);
                            if(it_top_right != hash.end()){
                                ht.remove_value(nodes[it_top_right->second].hash, z_top_right);
                                source_ptr = it_top_right->second;
                                source_off_x = kr_roll.col - (x_block+1)*block_size;
                                source_off_y = kr_roll.row - (y_block)*block_size;
                            }
                        }

                        //top-left block
                        auto z_top_left = codes::zeta_order::encode(x_block, y_block);
                        auto it_top_left = hash.find(z_top_left);
                        if(it_top_left != hash.end()){
                            ht.remove_value(nodes[it_top_left->second].hash, z_top_left);
                            source_ptr = it_top_left->second;
                            source_off_x = off_x;
                            source_off_y = off_y;
                        }


                        //Delete info of <target> from adjacency list
                        delete_info_shift(adjacent_lists, sx_target, sy_target, ex_target, ey_target, iterators_target,
                                kr_roll.row, kr_roll.iterators, kr_roll.heap_in, kr_roll.heap_out, block_size);

                        //IMPORTANT: we delete the first block of the same row
                        // - We have to update the prev_hash to 0
                        // Notice that kr_roll.row is always kr_roll.row % block_size = 0
                        if(sx_target == 0 && sy_target == kr_roll.prev_block_row){
                            kr_roll.update_prev_hash_prev_row();
                        }

                        //IMPORTANT: we delete the first block of the next row
                        // - We have to update the prev_hash by removing from it those values of the
                        //   first block in the next row
                        if(sx_target == 0 && sy_target == kr_roll.next_block_row){
                            kr_roll.update_prev_hash_next_row();
                        }

                        auto z_order_target = codes::zeta_order::encode(sx_target / block_size, sy_target / block_size);
                        auto it = hash.find(z_order_target);
                        auto pos_target = it->second;
                        hash.erase(it);
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = source_ptr;
                        nodes[pos_target].offset_x = source_off_x;
                        nodes[pos_target].offset_y = source_off_y;
                        nodes[pos_target].z_order = z_order_target;
                    }
                }
#if VERBOSE
                std::cout << std::endl;
#endif
            }

            //print_ajdacent_list(adjacent_lists);
        }
    };
}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_V2_HPP
