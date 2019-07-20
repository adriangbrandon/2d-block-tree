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
// Created by Adrián on 17/07/2019.
//

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_HPP

#include <kr_block_adjacent_list.hpp>
#include <hash_table_chainning.hpp>
#include <zeta_order.hpp>
#include <iostream>

namespace block_tree_2d {

    class algorithm {

    public:
        typedef uint64_t size_type;

    private:

        static void range_block(size_type id_b, size_type &start_x, size_type &end_x, size_type &start_y, size_type &end_y,
                         const size_type block_size){
            auto pos_b = codes::zeta_order::decode(id_b);
            start_x = pos_b.first * block_size;
            end_x = start_x + block_size-1;
            start_y = pos_b.second * block_size;
            end_y = start_y + block_size-1;
        }




        template <class input_type, class iterators_type>
        static bool are_identical(const input_type &adjacent_lists,
                                  const size_type id_b1, const iterators_type &iterators_b1,
                                  const size_type id_b2, const iterators_type &iterators_b2,
                                  const size_type block_size){

            //Compute the range of both blocks
            size_type sx_b1, ex_b1, sy_b1, ey_b1, sx_b2, ex_b2, sy_b2, ey_b2;
            range_block(id_b1, sx_b1, ex_b1, sy_b1, ey_b1, block_size);
            range_block(id_b2, sx_b2, ex_b2, sy_b2, ey_b2, block_size);
            size_type d1 = 0, d2 = 0;
            auto row = 0;
            //both empty
            auto it_b1 = iterators_b1[row];
            auto it_b2 = iterators_b2[row];
            while(row < block_size){
                if((it_b1 == adjacent_lists[sy_b1+row].end() && it_b2 == adjacent_lists[sy_b2+row].end()) ||
                    (*it_b1 >= ex_b1 && *it_b2 >= ex_b2)){
                    //both empty
                    ++row; //next row
                    it_b1 = iterators_b1[row];
                    it_b2 = iterators_b2[row];
                }else if((it_b1 == adjacent_lists[sy_b1+row].end() && it_b2 != adjacent_lists[sy_b2+row].end())
                        || (it_b1 != adjacent_lists[sy_b1+row].end() && it_b2 == adjacent_lists[sy_b2+row].end())) {
                    return false;
                }else {
                    if(*it_b1 - sx_b1 == *it_b2 - sx_b2){
                        ++it_b1; ++it_b2;
                    }else{
                        return false;
                    }
                }
            }
            return true;
        }

        template <class input_type, class list_hash_values_type, class iterators_value_type>
        static bool exist_identical_block(const input_type &adjacent_lists,
                                          size_type current_col, size_type current_row,
                                          iterators_value_type &current_iterators,
                                          const list_hash_values_type &hash_values,
                                          const size_type block_size){

            auto current_z_order = codes::zeta_order::encode(current_col, current_row);
            for(auto &it : hash_values){
                if(are_identical(adjacent_lists, current_z_order, current_iterators, it.first, it.second, block_size)){
                    std::cout << "Current: " << current_z_order << " equal to: " << it.first << std::endl;
                    return true;
                }
            }
            return false;

        }

    public:
        static constexpr uint64_t asize = 2;
        static constexpr uint64_t prime = 3355443229;

        template <class input_type, class hash_table_type>
        static void get_fingerprint_blocks(input_type &adjacent_lists, hash_table_type &ht,
                                            const size_type dimensions, const size_type block_size){

            typedef karp_rabin::kr_block_adjacent_list<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            typedef std::pair<size_type, iterators_value_type> hash_element_type;
           // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            //typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            auto total_blocks_in_row = dimensions / block_size;
            kr_type kr_block(block_size, prime, asize);
            kr_block.init(adjacent_lists.begin(), adjacent_lists.end(), total_blocks_in_row);
            iterators_value_type iterators_value = kr_block.iterators_value;
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            auto i = 0;
            while(kr_block.next()){
                std::cout << "Hash: " << kr_block.hash << std::endl;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_block.hash, it_table, it_hash)){
                    if(exist_identical_block(adjacent_lists, i%total_blocks_in_row, i/total_blocks_in_row,
                            iterators_value, it_hash->second, block_size)){ //check if they are identical
                        //TODO: add pointer and offset [<p, <x,y>>]

                    }else{
                        //add <z_order, kr_block.iterators_value> to chain in map
                        size_type z_order = codes::zeta_order::encode(i%total_blocks_in_row, i/total_blocks_in_row);
                        auto hash_element = hash_element_type(z_order, iterators_value);
                        ht.insert_hash_collision(it_hash, hash_element);
                    }
                }else{
                    //add <z_order, kr_block.iterators_value> to map [chain should be empty]
                    size_type z_order = codes::zeta_order::encode(i%total_blocks_in_row, i/total_blocks_in_row);
                    auto hash_element = hash_element_type(z_order, iterators_value);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, hash_element);
                }
                iterators_value = kr_block.iterators_value;
                ++i;
            }

        }

        template <class t_input, class t_map, class t_size>
        void get_type_of_nodes(){

        }
    };
}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_HPP
