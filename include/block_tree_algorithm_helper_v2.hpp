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
#include <zeta_order.hpp>
#include <iostream>

namespace block_tree_2d {

    class algorithm {

    public:
        typedef uint64_t size_type;

    private:

        static void range_block(size_type id_b,
                                size_type &start_x, size_type &end_x, size_type &start_y, size_type &end_y,
                                const size_type block_size){
            auto pos_b = codes::zeta_order::decode(id_b);
            start_x = pos_b.first * block_size;
            end_x = start_x + block_size-1;
            start_y = pos_b.second * block_size;
            end_y = start_y + block_size-1;
        }

        template <class input_type, class iterators_type>
        static bool are_identical(input_type &adjacent_lists,
                                  const size_type id_b1,
                                  const size_type sx_b2, const size_type ex_b2,
                                  const size_type sy_b2, const size_type ey_b2,
                                  const iterators_type &iterators_b2,
                                  const size_type block_size,
                                  size_type &sx_b1, size_type &sy_b1,
                                  size_type &ex_b1, size_type &ey_b1,
                                  iterators_type &iterators_b1){

            //Compute the range of z-block
            range_block(id_b1, sx_b1, ex_b1, sy_b1, ey_b1, block_size);
            if(sx_b1 == sx_b2 && sy_b1 == sy_b2) return false; //are the same
            size_type row = 0;
            while(row < block_size){
                auto beg_list_b1 = (adjacent_lists.begin() + sy_b1 + row)->begin();
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                auto it_b1 = std::upper_bound(beg_list_b1, (adjacent_lists.begin()+sy_b1+row)->end(), ex_b1);
                iterators_b1[(sy_b1 + row) % block_size] = it_b1;
                while(it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    size_type d2 = *it_b2 - sx_b2;
                    if(it_b1 == beg_list_b1) return false;
                    --it_b1;
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
                                  const size_type sx_b2, const size_type sy_b2,
                                  const size_type ex_b2, const size_type ey_b2,
                                  const iterators_type &current_iterators,
                                  list_hash_values_type &hash_values,
                                  const size_type block_size,
                                  size_type &sx_b1, size_type &sy_b1,
                                  size_type &ex_b1, size_type &ey_b1,
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

        template <class input_type, class iterators_type>
        static void delete_info(input_type &adjacent_lists,
                                const size_type sx_b2, const size_type sy_b2,
                                const size_type ex_b2, const size_type ey_b2,
                                const iterators_type &iterators_b2,
                                const size_type block_size){

            size_type row = 0;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                while(it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    (adjacent_lists.begin() + sy_b2 + row)->erase(it_b2);
                }
                ++row;
            }

        }

        static inline void compute_topleft_and_offsets(const size_type col, const size_type row, const size_type block_size,
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







    public:
        static constexpr uint64_t prime = 3355443229;

        template <class input_type, class hash_table_type>
        static void get_fingerprint_blocks(input_type &adjacent_lists, hash_table_type &ht,
                                           const size_type dimensions, const size_type block_size){

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
                std::cout << "Target: (" << kr_block.col << ", " << kr_block.row << ")" << std::endl;
                std::cout << "Hash: " << kr_block.hash << std::endl;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_block.hash, it_table, it_hash)){
                    size_type sx_source, sy_source, ex_source, ey_source;
                    size_type sx_target = kr_block.col * block_size;
                    size_type sy_target = kr_block.row * block_size;
                    size_type ex_target = sx_target + block_size-1;
                    size_type ey_target = sy_target + block_size-1;
                    iterator_hash_value_type source;
                    if(exist_identical(adjacent_lists, sx_target, sy_target, ex_target, ey_target,
                                       kr_block.iterators, it_hash->second, block_size,
                                       sx_source, sy_source, ex_source, ey_source, iterators_source, source)){ //check if they are identical
                        std::cout << "Pointer to source in z-order: " << (source->first) << " offset: <0,0>" << std::endl;
                        //TODO: add pointer and offset [<p, <x,y>>]
                        //Delete info of <target> from adjacency list
                        delete_info(adjacent_lists, sx_target, sy_target, ex_target, ey_target, kr_block.iterators, block_size);
                    }else{
                        //add <z_order> to chain in map
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);
                    }
                }else{
                    //add <z_order> to map [chain should be empty]
                    size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, z_order);
                }
                std::cout << std::endl;
            }
            //Delete sources of hash_table
            ht.remove_marked();
            print_ajdacent_list(adjacent_lists);
        }

        template <class input_type, class hash_table_type>
        static void get_type_of_nodes(input_type &adjacent_lists, hash_table_type &ht,
                               const size_type dimensions, const size_type block_size){

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
                std::cout << "Source: (" << kr_roll.col << ", " << kr_roll.row << ")" << std::endl;
                std::cout << "Hash: " << kr_roll.hash << std::endl;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_roll.hash, it_table, it_hash)){
                    size_type sx_target, sy_target, ex_target, ey_target;
                    size_type ex_source = kr_roll.col + block_size-1;
                    size_type ey_source = kr_roll.row + block_size-1;
                    iterator_hash_value_type target;
                    if(exist_identical(adjacent_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                             kr_roll.iterators, it_hash->second, block_size,
                                             sx_target, sy_target, ex_target, ey_target, iterators_target, target)){ //check if they are identical
                        std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
                        //Compute offsets and top-left block
                        size_type x_block, y_block, off_x, off_y;
                        compute_topleft_and_offsets(kr_roll.col, kr_roll.row, block_size, x_block, y_block, off_x, off_y);
                        //top-left block
                        codes::zeta_order::encode(x_block, y_block);
                        //top-right block
                        if(off_x){
                            codes::zeta_order::encode(x_block + 1, y_block);
                        }
                        //bottom-left block
                        if(off_y){
                            codes::zeta_order::encode(x_block, y_block + 1);
                        }
                        //bottom-right block
                        if(off_x && off_y){
                            codes::zeta_order::encode(x_block + 1, y_block + 1);
                        }

                        //TODO: delete <block sources> from hash_table ERROR!!

                        //TODO: SOLUTION:
                        //TODO: if any  <block sources> is untouchable
                        //TODO:     remove it from hash and discard pointer
                        //TODO: else
                        //TODO:     add pointer and offset [<p, <x,y>>]

                        //Delete <target> from hash_table
                        ht.remove_value(it_table, it_hash, target);
                        //Delete info of <target> from adjacency list
                        delete_info(adjacent_lists, sx_target, sy_target, ex_target, ey_target, iterators_target, block_size);
                    }
                }
                std::cout << std::endl;
            }




        }
    };
}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_V2_HPP
