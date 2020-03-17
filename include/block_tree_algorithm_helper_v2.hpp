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

#include <kr_block_adjacent_list_v4.hpp>
#include <kr_roll_adjacent_list_v4.hpp>
#include <hash_table_chainning.hpp>
#include <sdsl/bit_vectors.hpp>
#include <zeta_order.hpp>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <bithacks.hpp>
#include <progress_bar.hpp>
#include <search_util.hpp>
#include "logger.hpp"
#include "intersection_lists.hpp"

#define NODE_EMPTY 0
#define NODE_LEAF 1
#define NODE_INTERNAL 2

namespace block_tree_2d {

    class algorithm {

    public:
        struct pair_hash {
            template <class T1, class T2>
            std::size_t operator() (const std::pair<T1,T2> &p) const {
                auto h1 = std::hash<T1>{}(p.first);
                auto h2 = std::hash<T2>{}(p.second);
                return h1 ^h2;
            }
        };
        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef std::unordered_map<size_type, size_type > hash_type;
        typedef std::pair<value_type , value_type > diff_cord_type;
        typedef std::pair<size_type , size_type > point_type;
        typedef typename std::vector< diff_cord_type > replacements_list_type;
        typedef typename std::vector< point_type > replacements_point_list_type;
        typedef typename std::vector<replacements_point_list_type> replacements_point_lists_type;
        typedef typename std::unordered_map<point_type, replacements_list_type, pair_hash> replacements_map_type;
        typedef std::unordered_map<point_type, std::vector<size_type>, pair_hash> sources_map_type;
        typedef typename std::unordered_map<size_type, char> blocks_replace_map_type;
        //typedef typename std::unordered_map<point_type, bool, pair_hash> replacements_map_type;
        typedef typename replacements_map_type::iterator replacements_map_iterator;
        typedef struct {
            size_type z_order = 0;
            size_type type = NODE_EMPTY;
            value_type offset_x = 0;
            value_type offset_y = 0;
            size_type ptr = 0;
            size_type hash = 0;
            size_type bits = 0;
        } node_type;
        class compare_abs_value {
        public:
            bool operator()(const int64_t &x, const int64_t &y){
                return std::abs(x) < std::abs(y);
            }
        };

    private:

        static void range_block(size_type id_b,
                                value_type &start_x, value_type &end_x, value_type &start_y, value_type &end_y,
                                const size_type block_size){
            auto pos_b = codes::zeta_order::decode(id_b);
            //start_x = pos_b.first * block_size;
            start_x = pos_b.first * block_size;
            end_x = start_x + block_size-1;
            //start_y = pos_b.second * block_size+1;
            start_y = pos_b.second * block_size;
            end_y = start_y + block_size-1;
        }

        //IMPORTANT: adjacent_lists without negative numbers
        template <class input_type>
        static bool is_empty(const input_type &adjacent_lists, size_type x, size_type y, size_type block_size){
                size_type row = 0;
                size_type ex = x + block_size-1;
                while(row < block_size){
                    if(y + row < adjacent_lists.size() && !adjacent_lists[y+row].empty()) {
                        auto pos = util::search::lower_or_equal_search(0, adjacent_lists[y+row].size()-1, adjacent_lists[y+row], ex);
                        if(pos != -1 && adjacent_lists[y+row][pos] >=x){
                            return false;
                        }
                    }
                    ++row;
                }
                return true;
        }

        template <class input_type, class iterators_type>
        static size_type bits_k2tree(const input_type &adjacent_lists, const value_type sx_b2, const value_type ex_b2,
                                      const value_type sy_b2, const value_type ey_b2, const iterators_type &iterators_b2,
                                      const size_type block_size, const size_type k){

            typedef std::tuple<size_type , size_type, size_type,size_type> t_part_tuple;

            std::vector<size_type> edges_z_order;
            size_type row = 0, bits = 0;
            size_type k_2 = k*k;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                while(it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    size_type col = *it_b2 - sx_b2;
                    auto z_order = codes::zeta_order::encode(col, row);
                    edges_z_order.push_back(z_order);
                }
                ++row;
            }
            //2. Sort edges z-order
            std::sort(edges_z_order.begin(), edges_z_order.end());

            //5. Split the front of q into its children
            size_type i, j, z_0;
            size_type l = block_size;
            std::queue<t_part_tuple> q;
            q.push(t_part_tuple(0, edges_z_order.size()-1, l/k , 0));
            while (!q.empty()) {
                std::tie(i, j, l, z_0) = q.front();
                q.pop();
                auto elements = l * l;
                for(size_type z_child = 0; z_child < k_2; ++z_child){
                    auto le = util::search::lower_or_equal_search(i, j, edges_z_order, z_0+elements-1);
                    if(le != -1 && edges_z_order[le] >= z_0){
                        ++bits;
                        if(l / k > 0){
                            q.push(t_part_tuple(i, le, l/k, z_0));
                        }
                        i = le + 1;
                    }else{
                        ++bits;
                    }

                    z_0 += elements;
                }
            }
            return bits;

        }

        template <class input_type, class iterators_type>
        static size_type bits_k2tree_v2(const input_type &adjacent_lists, const value_type sx_b2, const value_type ex_b2,
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
            return bits;

        }



        template <class input_type, class iterators_type>
        static size_type bits_subtree(const input_type &adjacent_lists, const value_type sx_b2, const value_type ex_b2,
                                     const value_type sy_b2, const value_type ey_b2, const iterators_type &iterators_b2,
                                     const size_type block_size, const size_type k){

            size_type row = 0, bits = 0;
            size_type k_pow_2 = k*k;
            hash_type hash0, hash1;
            while(row < block_size){

                /*auto beg_list_b1 = (adjacent_lists.begin() + sy_b1 + row)->begin();
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                auto it_b1 = std::upper_bound(beg_list_b1, (adjacent_lists.begin()+sy_b1+row)->end(), ex_b1, compare_abs_value());
                iterators_b1[(sy_b1 + row) % block_size] = it_b1;
                while(it_b2 != beg_list_b2 && std::abs(*(--it_b2)) >= sx_b2){
                    if(*it_b2 >= 0){
                        size_type d2 = *it_b2 - sx_b2;
                        if(it_b1 == beg_list_b1 || *(--it_b1) < sx_b1) return false;
                        size_type d1 = *it_b1 - sx_b1;
                        if(d1 != d2) return false;
                    }
                }*/

                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                //std::cout << "it_b2 = terators_b2[" << (sy_b2 + row) % block_size << "]" << std::endl;
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
                size_type ones = 0;
                for(const auto &k_v : hash0){
                    auto z_order = k_v.first / k_pow_2;
                    auto it = hash1.find(z_order);
                    if(it == hash1.end()){
                        hash1.insert(it, {z_order,1});
                    }
                    ++ones;
                }
                hash0.clear();
                auto temp = std::move(hash0);
                hash0 = std::move(hash1);
                hash1 = std::move(temp);
                auto total_elements = hash0.size() * k_pow_2;
                auto zeroes = total_elements - ones;
                bits += total_elements + ones;
                b = b/k;
            }
            return bits;

        }

        template <class input_type, class iterators_type>
        static size_type bits_subtree_v2(const input_type &adjacent_lists, const value_type sx_b2, const value_type ex_b2,
                                      const value_type sy_b2, const value_type ey_b2, const iterators_type &iterators_b2,
                                      const size_type block_size, const size_type k){

            size_type row = 0, bits = 0;
            size_type k_pow_2 = k*k;
            hash_type hash0, hash1;
            while(row < block_size){

                /*auto beg_list_b1 = (adjacent_lists.begin() + sy_b1 + row)->begin();
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                auto it_b1 = std::upper_bound(beg_list_b1, (adjacent_lists.begin()+sy_b1+row)->end(), ex_b1, compare_abs_value());
                iterators_b1[(sy_b1 + row) % block_size] = it_b1;
                while(it_b2 != beg_list_b2 && std::abs(*(--it_b2)) >= sx_b2){
                    if(*it_b2 >= 0){
                        size_type d2 = *it_b2 - sx_b2;
                        if(it_b1 == beg_list_b1 || *(--it_b1) < sx_b1) return false;
                        size_type d1 = *it_b1 - sx_b1;
                        if(d1 != d2) return false;
                    }
                }*/

                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                //std::cout << "it_b2 = terators_b2[" << (sy_b2 + row) % block_size << "]" << std::endl;
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
                size_type ones = 0;
                for(const auto &k_v : hash0){
                    auto z_order = k_v.first / k_pow_2;
                    auto it = hash1.find(z_order);
                    if(it == hash1.end()){
                        hash1.insert(it, {z_order,1});
                    }
                    ++ones;
                }
                hash0.clear();
                auto temp = std::move(hash0);
                hash0 = std::move(hash1);
                hash1 = std::move(temp);
                auto total_elements = hash0.size() * k_pow_2;
                auto zeroes = total_elements - ones;
                bits += total_elements + ones;
                b = b/k;
            }
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
                                  iterators_type &iterators_b1, const bool check_overlapping = true){

            //Compute the range of z-block
            range_block(id_b1, sx_b1, ex_b1, sy_b1, ey_b1, block_size);
            //both blocks are overlapping each other
            if(check_overlapping && !(sx_b1 > ex_b2 || ex_b1 < sx_b2 || sy_b1 > ey_b2 || ey_b1 < sy_b2)){
                return false;
            }

            size_type row = 0;
            while(row < block_size){
                auto beg_list_b1 = (adjacent_lists.begin() + sy_b1 + row)->begin();
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = iterators_b2[(sy_b2 + row) % block_size];
                auto it_b1 = std::upper_bound(beg_list_b1, (adjacent_lists.begin()+sy_b1+row)->end(), ex_b1, compare_abs_value());
                iterators_b1[(sy_b1 + row) % block_size] = it_b1;
                while(it_b2 != beg_list_b2 && std::abs(*(--it_b2)) >= sx_b2){
                    if(*it_b2 >= 0){
                        size_type d2 = *it_b2 - sx_b2;
                        if(it_b1 == beg_list_b1 || *(--it_b1) < sx_b1) return false;
                        size_type d1 = *it_b1 - sx_b1;
                        if(d1 != d2) return false;
                    }
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

        template <class input_type, class list_hash_values_type, class iterators_type>
        static bool exist_identical_mult(input_type &adjacent_lists,
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
                                 block_size, sx_b1, sy_b1, ex_b1, ey_b1, iterators_b1, false)){
                    result = it;
                    return true;
                }
                ++it;
            }
            return false;

        }

        template <class input_type, class list_hash_values_type, class iterators_type>
        static bool exist_identical_mult_v2(input_type &adjacent_lists,
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
                if(!it->second.empty() && are_identical(adjacent_lists, it->second[0], sx_b2, ex_b2, sy_b2, ey_b2, current_iterators,
                                                        block_size, sx_b1, sy_b1, ex_b1, ey_b1, iterators_b1, false)){
                    result = it;
                    return true;
                }
                ++it;
            }
            return false;

        }

        template <class input_type, class iterators_type, class heap_type>
        static bool delete_info_shift(input_type &adjacent_lists,
                                      const value_type sx_b2, const value_type sy_b2,
                                      const value_type ex_b2, const value_type ey_b2,
                                      const iterators_type &iterators_b2,
                                      const value_type sy_b1, const value_type ey_b1,
                                      iterators_type &kr_iterators,
                                      heap_type &heap_in,
                                      heap_type &heap_out,
                                      const size_type block_size){

            size_type row = 0;
            bool redo_heap_in = false;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto cyclic_b2 = (sy_b2 + row) % block_size;
                auto it_b2 = iterators_b2[cyclic_b2];
                auto last = it_b2;
                auto count = 0;
                while(!(adjacent_lists.begin() + sy_b2 + row)->empty() &&
                       it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2) {
                    //auto new_it = (adjacent_lists.begin() + sy_b2 + row)->erase(it_b2);
                    //std::cout << "Remove at " << (sy_b2+row) << " value: " << *it_b2 << std::endl;
                    *it_b2 = -(*it_b2+1);
                    last = it_b2;
                    ++count;
                }

                //IMPORTANT: what happen when the next 1 is going to be deleted?
                //- We have to update the iterator of kr_iterator to the next one
                //- We have to update the heap with the new kr_iterator (if it is pointing to a number)
                if(sy_b1 <= sy_b2+row && sy_b2+row <= ey_b1 && count > 0
                    && last == kr_iterators[cyclic_b2]){
                    auto it = last + count; //in case of being negative, redo_heap_in updates it to the next positive
                    kr_iterators[cyclic_b2] = it;
                    redo_heap_in = true;
                }
                ++row;
            }
            return redo_heap_in;

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
                    //std::cout << "Remove at " << (sy_b2+row) << " value: " << *it_b2 << std::endl;
                    *it_b2 = -(*it_b2+1);
                    //std::cout << "Row: " << sy_b2 + row << std::endl;
                   // std::cout << "Deleted " << *it_b2 << std::endl;
                   // std::cout << "New pointer to " << *(iterators_b2[(sy_b2 + row) % block_size]) << std::endl;
                }
                ++row;
            }
        }

        template <class input_type>
        static void delete_info_block(input_type &adjacent_lists,
                                      const value_type sx_b2, const value_type sy_b2,
                                      const value_type ex_b2, const value_type ey_b2,
                                      const size_type block_size){

            size_type row = 0;
            while(row < block_size){
                auto beg_list_b2 = (adjacent_lists.begin() + sy_b2 + row)->begin();
                auto it_b2 = std::upper_bound(beg_list_b2, (adjacent_lists.begin()+sy_b2+row)->end(), ex_b2, compare_abs_value());
                //++iterators_b2[(sy_b2 + row) % block_size];
                while(!(adjacent_lists.begin() + sy_b2 + row)->empty() &&
                      it_b2 != beg_list_b2 && *(--it_b2) >= sx_b2){
                    *it_b2 = -(*it_b2+1);
                }
                ++row;
            }
        }

        static inline void compute_topleft_and_offsets(const value_type col, const value_type row, const size_type block_size,
                                                       size_type &c, size_type &r, value_type &off_x, value_type &off_y){

            c = col/block_size;
            r = row/block_size;
            off_x = col - c*block_size;
            off_y = row - r*block_size;
        }



        /*static size_type compute_position_block(const size_type z_order,
                                                const rank_type &rank){
            return rank(z_order+1)-1;
        }*/








    public:
        //first prime lower than 2**32
        //const uint64_t prime = 3355443229;     // next prime(2**31+2**30+2**27)
        //first prime lower than 2**55
        //const uint64_t prime = 27162335252586509; // next prime (2**54 + 2**53 + 2**47 + 2**13)

        static constexpr uint64_t prime = 3355443229;

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
        static bool contains_identical_blocks(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                           const size_type dimensions, const size_type block_size){

            typedef karp_rabin::kr_block_adjacent_list_v4<input_type> kr_type;
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

            auto blocks_per_row = adjacent_lists.size() / block_size;
            //Total number of blocks
            auto total_blocks = blocks_per_row * blocks_per_row;
            util::progress_bar m_progress_bar(total_blocks);
            //IMPORTANT: kr_block, skips every empty block. For this reason, every node of the current level has to be
            //           initialized as empty node.
            while(kr_block.next()){
#if BT_VERBOSE
                std::cout << "Target: (" << kr_block.col << ", " << kr_block.row << ")" << std::endl;
                std::cout << "Hash: " << kr_block.hash << std::endl;
#endif
                //check if kr_block.hash exists in map
                //Target z-order
                auto processed_blocks = kr_block.row * blocks_per_row + kr_block.col+1;
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
#if BT_VERBOSE
                        std::cout << "Pointer to source in z-order: " << (source->first) << " offset: <0,0>" << std::endl;
#endif

                        m_progress_bar.done();
                        return true;
                    }
                }else{
                    size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, z_order);
                }
                m_progress_bar.update(processed_blocks);
#if BT_VERBOSE
                std::cout << std::endl;
#endif
            }
            m_progress_bar.done();
            return false;
        }

        template <class input_type, class hash_table_type>
        static bool contains_identical_rolls(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                      const size_type dimensions, const size_type block_size){

            typedef karp_rabin::kr_roll_adjacent_list_v4<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            auto rolls_per_row = adjacent_lists.size() - block_size + 1;
            //Total number of blocks
            auto total_rolls = rolls_per_row * rolls_per_row;
            util::progress_bar m_progress_bar(total_rolls);

            kr_type kr_roll(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_target = iterators_value_type(block_size);
            while(kr_roll.next()){

#if BT_VERBOSE
                std::cout << "Source: (" << kr_roll.col << ", " << kr_roll.row << ")" << std::endl;
                std::cout << "Hash: " << kr_roll.hash << std::endl;
#endif
                auto processed_rolls = kr_roll.row * rolls_per_row + kr_roll.col+1;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_roll.hash, it_table, it_hash)){
                    value_type sx_target, sy_target, ex_target, ey_target;
                    value_type ex_source = kr_roll.col + block_size-1;
                    value_type ey_source = kr_roll.row + block_size-1;
                    iterator_hash_value_type target;
                    if(exist_identical(adjacent_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                       kr_roll.iterators, it_hash->second, block_size,
                                       sx_target, sy_target, ex_target, ey_target, iterators_target, target)){ //check if they are identical
#if BT_VERBOSE
                        std::cout << "Target: (" << sx_target << ", " << sy_target << ")" << std::endl;
                        std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
#endif
                        m_progress_bar.done();
                        return true;
                    }
                }
                m_progress_bar.update(processed_rolls);
#if BT_VERBOSE
                std::cout << std::endl;
#endif
            }
            m_progress_bar.done();
            return false;
            //print_ajdacent_list(adjacent_lists);
        }



        template <class input_type, class hash_table_type>
        static void get_fingerprint_blocks_stack_lite(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                           const size_type dimensions, const size_type block_size,
                                           hash_type &hash, std::vector<node_type> &nodes, const bool compute_bits = false){

            typedef karp_rabin::kr_block_adjacent_list_v4<input_type> kr_type;
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

            size_type hashes = 0;
            auto blocks_per_row = adjacent_lists.size() / block_size;
            //Total number of blocks
            auto total_blocks = blocks_per_row * blocks_per_row;
            util::progress_bar m_progress_bar(total_blocks);
            //IMPORTANT: kr_block, skips every empty block. For this reason, every node of the current level has to be
            //           initialized as empty node.
            while(kr_block.next()){
#if BT_VERBOSE
                std::cout << "Target: (" << kr_block.col << ", " << kr_block.row << ")" << std::endl;
                std::cout << "Hash: " << kr_block.hash << std::endl;
#endif
                //check if kr_block.hash exists in map
                //Target z-order
                auto z_order_target = codes::zeta_order::encode(kr_block.col, kr_block.row);
                auto processed_blocks = kr_block.row * blocks_per_row + kr_block.col+1;
                auto it_target = hash.find(z_order_target);
                auto pos_target = it_target->second;
                value_type sx_target = kr_block.col * block_size;
                value_type sy_target = kr_block.row * block_size;
                value_type ex_target = sx_target + block_size-1;
                value_type ey_target = sy_target + block_size-1;
                if(ht.hash_collision(kr_block.hash, it_table, it_hash)){
                    value_type sx_source, sy_source, ex_source, ey_source;
                    iterator_hash_value_type source;
                    if(exist_identical(adjacent_lists, sx_target, sy_target, ex_target, ey_target,
                                       kr_block.iterators, it_hash->second, block_size,
                                       sx_source, sy_source, ex_source, ey_source, iterators_source, source)){ //check if they are identical
#if BT_VERBOSE
                        std::cout << "Pointer to source in z-order: " << (source->first) << " offset: <0,0>" << std::endl;
#endif

                        //Add pointer and offset [<p, <x,y>>]
                        //hash.erase(it);
                        auto pos_source = hash.find(source->first)->second;
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = pos_source;
                        nodes[pos_target].hash = kr_block.hash;
                        nodes[pos_target].z_order = z_order_target;
                        if(compute_bits){
                            //nodes[pos_target].bits = bits_k2tree(adjacent_lists, sx_target, ex_target, sy_target, ey_target,
                            //                                  kr_block.iterators, block_size, k);
                            nodes[pos_target].bits = nodes[pos_source].bits;
                        }
                        hash.erase(it_target);
                        //Delete info of <target> from adjacency list
                        delete_info_block(adjacent_lists, sx_target, sy_target, ex_target, ey_target, kr_block.iterators,  block_size);
                    }else{
                        //add <z_order> to chain in map
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);
                        nodes[pos_target].type = NODE_INTERNAL;
                        nodes[pos_target].hash = kr_block.hash;
                        nodes[pos_target].z_order = z_order_target;
                        if(compute_bits){
                            nodes[pos_target].bits = bits_k2tree(adjacent_lists, sx_target, ex_target, sy_target, ey_target,
                                                              kr_block.iterators, block_size, k);
                            //nodes[pos_target].bits = nodes[pos_source].bits;
                        }
                    }
                }else{
                    //add <z_order> to map [chain should be empty]
                    size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, z_order);
                    nodes[pos_target].type = NODE_INTERNAL;
                    nodes[pos_target].hash = kr_block.hash;
                    nodes[pos_target].z_order = z_order_target;
                    if(compute_bits) {
                        nodes[pos_target].bits = bits_k2tree(adjacent_lists, sx_target, ex_target, sy_target, ey_target,
                                                             kr_block.iterators, block_size, k);
                    }
                }
                m_progress_bar.update(processed_blocks);
                ++hashes;
#if BT_VERBOSE
                std::cout << std::endl;
#endif
            }
            //Delete sources of hash_table
            ht.remove_marked();
            m_progress_bar.done();
            std::cout << "Total hashes: " << hashes << std::endl;
            //print_ajdacent_list(adjacent_lists);
        }

        template <class input_type>
        static void remove_target(input_type &adjacent_lists, sources_map_type &sources, size_type z_order, size_type block_size){
            auto point = codes::zeta_order::decode(z_order);
            auto sx = point.first * block_size;
            auto sy = point.second * block_size;
            auto ex = sx + block_size-1;
            auto ey = sy + block_size-1;
            delete_info_block(adjacent_lists, sx, sy, ex, ey, block_size);
            /*for(auto y = sy; y <= ey; ++y){
                for(auto x = sx; x <= ex; ++x){
                    sources.erase({x, y});
                }
            }*/
        }

        template <class input_type>
        static void get_block_replacements(input_type &adjacent_lists, const size_type k,
                                                      sources_map_type &sources,
                                                      const size_type dimensions, const size_type block_size,
                                                      hash_type &hash, std::vector<node_type> &nodes,
                                                      std::unordered_map<size_type, char> &untouchable_block){

            // typedef uint64_t iterators_type;
            size_type hashes = 0;
            //Total number of blocks
            std::vector<point_type> points_to_check;
            for(const auto &s : sources){
                auto p = s.first;
                if(p.first % block_size == 0 && p.second % block_size == 0){
                    points_to_check.push_back(p);
                }
            }
            std::sort(points_to_check.begin(), points_to_check.end());
            util::progress_bar m_progress_bar(points_to_check.size());
            size_type processed_blocks = 0;
           //for(size_type y = 0; y < dimensions; y += block_size){
           //     for(size_type x = 0; x < dimensions; x += block_size){
            for(const auto &p : points_to_check){
                auto it_src = sources.find({p.first, p.second});
                if(it_src != sources.end()){
                    auto z_order_source = codes::zeta_order::encode(p.first / block_size, p.second / block_size);
                    auto pos_source = hash.find(z_order_source)->second;
                    if(nodes[pos_source].type == NODE_LEAF){
                        continue;
                    }
                    untouchable_block.insert({z_order_source, 1});
                    for(const auto &b : it_src->second){
                        //auto it_repl = blocks_replace_map.find(b);
                        //if(it_repl != blocks_replace_map.end()){
                        auto it_target = hash.find(b);
                        auto pos_target = it_target->second;
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = pos_source;
                        nodes[pos_target].z_order = b;
                        remove_target(adjacent_lists, sources, b, block_size);
                        //hash.erase(it_target);
                        //}
                    }

                }
                //}
                ++processed_blocks;
                m_progress_bar.update(processed_blocks);
            }

            m_progress_bar.done();
        }

        template <class input_type>
        static void get_roll_replacements(input_type &adjacent_lists, const size_type k,
                                           sources_map_type &sources,
                                           const size_type dimensions, const size_type block_size,
                                           hash_type &hash, std::vector<node_type> &nodes, std::unordered_map<size_type, char> &untouchable_block){

            // typedef uint64_t iterators_type;
            size_type hashes = 0;
            auto blocks_per_row = adjacent_lists.size() / block_size;
            //Total number of blocks
            auto total_blocks = (dimensions-block_size+1) * (dimensions-block_size+1);
            std::vector<point_type> points_to_check;
            for(const auto &s : sources){
                auto p = s.first;
                if(p.first % block_size != 0 || p.second % block_size != 0){
                    points_to_check.push_back(p);
                }
            }
            std::sort(points_to_check.begin(), points_to_check.end());
            util::progress_bar m_progress_bar(points_to_check.size());
            size_type processed_blocks = 0;
            for(const auto &p : points_to_check){
                //for(size_type x = 0; x < dimensions-block_size+1; ++x){
                auto x = p.first;
                auto y = p.second;
                auto it_src = sources.find({x, y});
                //std::cout << "Sources find: " << (it_src != sources.end()) << std::endl;
                if(it_src != sources.end()){
                    for(const auto &b : it_src->second){
                        if(untouchable_block.find(b) != untouchable_block.end() || hash.find(b) == hash.end()) continue;
                        //Compute offsets and top-left block
                        size_type x_block, y_block;
                        size_type source_ptr;
                        value_type source_off_x, source_off_y, off_x, off_y;
                        compute_topleft_and_offsets(x, y, block_size, x_block, y_block, off_x, off_y);
                        //bottom-right block
                        std::vector<size_type> blocks_to_untouchable;
                        //bool exists = false;
                        if(off_x && off_y){
                            auto z_bottom_right = codes::zeta_order::encode(x_block + 1, y_block + 1);
                            auto it_bottom_right = hash.find(z_bottom_right);
                            if(it_bottom_right != hash.end()){
                                auto pos_bottom_right = it_bottom_right->second;
                                if(nodes[pos_bottom_right].type == NODE_LEAF){
                                    continue;
                                }
                                source_ptr = pos_bottom_right;
                                source_off_x = x - (x_block + 1)*block_size;
                                source_off_y = y - (y_block + 1)*block_size;
                                blocks_to_untouchable.push_back(z_bottom_right);
                            }

                        }

                        //bottom-left block
                        if(off_y){
                            auto z_bottom_left = codes::zeta_order::encode(x_block, y_block + 1);
                            auto it_bottom_left = hash.find(z_bottom_left);
                            if(it_bottom_left != hash.end()){
                                auto pos_bottom_left = it_bottom_left->second;
                                if(nodes[pos_bottom_left].type == NODE_LEAF){
                                    continue;
                                }
                                source_ptr = pos_bottom_left;
                                source_off_x = x - x_block *block_size;
                                source_off_y = y - (y_block + 1)*block_size;
                                blocks_to_untouchable.push_back(z_bottom_left);
                            }
                        }

                        //top-right block
                        if(off_x){
                            auto z_top_right = codes::zeta_order::encode(x_block+1, y_block);
                            auto it_top_right = hash.find(z_top_right);
                            if(it_top_right != hash.end()){
                                auto pos_top_right = it_top_right->second;
                                if(nodes[pos_top_right].type == NODE_LEAF){
                                    continue;
                                }
                                source_ptr = pos_top_right;
                                source_off_x = x - (x_block+1 )*block_size;
                                source_off_y = y - y_block *block_size;
                                blocks_to_untouchable.push_back(z_top_right);
                            }
                        }

                        //Since all of them has to be internal or empty, we point to the top-left block
                        //top-left block
                        auto z_top_left = codes::zeta_order::encode(x_block, y_block);
                        auto it_top_left = hash.find(z_top_left);
                        if(it_top_left != hash.end()){
                            auto pos_top_left = it_top_left->second;
                            if(nodes[pos_top_left].type == NODE_LEAF){
                                continue;
                            }
                            source_ptr = pos_top_left;
                            source_off_x = x - x_block*block_size;
                            source_off_y = y - y_block *block_size;
                            blocks_to_untouchable.push_back(z_top_left);
                        }

                        //untouchable_block.insert({z_top_left, 1});

                        auto it_target = hash.find(b);
                        auto pos_target = it_target->second;
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = source_ptr;
                        nodes[pos_target].z_order = b;
                        nodes[pos_target].offset_x = source_off_x;
                        nodes[pos_target].offset_y = source_off_y;
                        remove_target(adjacent_lists, sources, b, block_size);
                        //hash.erase(it_target);
                        for(const auto &b_to_ut : blocks_to_untouchable){
                            if(untouchable_block.find(b_to_ut) == untouchable_block.end()){
                                untouchable_block.insert({b_to_ut, 1});
                            }
                        }

                    }
                }
                ++processed_blocks;
                m_progress_bar.update(processed_blocks);
            }
            m_progress_bar.done();
        }

        template <class input_type>
        static void get_roll_replacements_v2(input_type &adjacent_lists, const size_type k,
                                          sources_map_type &sources,
                                          const size_type dimensions, const size_type block_size,
                                          hash_type &hash, std::vector<node_type> &nodes, std::unordered_map<size_type, char> &untouchable_block){

            // typedef uint64_t iterators_type;
            size_type hashes = 0;
            auto blocks_per_row = adjacent_lists.size() / block_size;
            //Total number of blocks
            auto total_blocks = (dimensions-block_size+1) * (dimensions-block_size+1);
            std::vector<point_type> points_to_check;
            for(const auto &s : sources){
                auto p = s.first;
                if(p.first % block_size != 0 || p.second % block_size != 0){
                    points_to_check.push_back(p);
                }
            }
            std::sort(points_to_check.begin(), points_to_check.end());
            util::progress_bar m_progress_bar(points_to_check.size());
            size_type processed_blocks = 0;
            for(const auto &p : points_to_check){
                //for(size_type x = 0; x < dimensions-block_size+1; ++x){
                auto x = p.first;
                auto y = p.second;
                auto it_src = sources.find({x, y});
                //std::cout << "Sources find: " << (it_src != sources.end()) << std::endl;
                if(it_src != sources.end()){
                    //Compute offsets and top-left block
                    size_type x_block, y_block;
                    size_type source_ptr;
                    value_type source_off_x, source_off_y, off_x, off_y;
                    compute_topleft_and_offsets(x, y, block_size, x_block, y_block, off_x, off_y);
                    //bottom-right block
                    std::vector<size_type> blocks_to_untouchable;
                    //bool exists = false;
                    if(off_x && off_y){
                        auto z_bottom_right = codes::zeta_order::encode(x_block + 1, y_block + 1);
                        auto it_bottom_right = hash.find(z_bottom_right);
                        if(it_bottom_right != hash.end()){
                            auto pos_bottom_right = it_bottom_right->second;
                            if(nodes[pos_bottom_right].type == NODE_LEAF){
                                continue;
                            }
                            source_ptr = pos_bottom_right;
                            source_off_x = x - (x_block + 1)*block_size;
                            source_off_y = y - (y_block + 1)*block_size;
                            blocks_to_untouchable.push_back(z_bottom_right);
                        }

                    }

                    //bottom-left block
                    if(off_y){
                        auto z_bottom_left = codes::zeta_order::encode(x_block, y_block + 1);
                        auto it_bottom_left = hash.find(z_bottom_left);
                        if(it_bottom_left != hash.end()){
                            auto pos_bottom_left = it_bottom_left->second;
                            if(nodes[pos_bottom_left].type == NODE_LEAF){
                                continue;
                            }
                            source_ptr = pos_bottom_left;
                            source_off_x = x - x_block *block_size;
                            source_off_y = y - (y_block + 1)*block_size;
                            blocks_to_untouchable.push_back(z_bottom_left);
                        }
                    }

                    //top-right block
                    if(off_x){
                        auto z_top_right = codes::zeta_order::encode(x_block+1, y_block);
                        auto it_top_right = hash.find(z_top_right);
                        if(it_top_right != hash.end()){
                            auto pos_top_right = it_top_right->second;
                            if(nodes[pos_top_right].type == NODE_LEAF){
                                continue;
                            }
                            source_ptr = pos_top_right;
                            source_off_x = x - (x_block+1 )*block_size;
                            source_off_y = y - y_block *block_size;
                            blocks_to_untouchable.push_back(z_top_right);
                        }
                    }

                    //Since all of them has to be internal or empty, we point to the top-left block
                    //top-left block
                    auto z_top_left = codes::zeta_order::encode(x_block, y_block);
                    auto it_top_left = hash.find(z_top_left);
                    if(it_top_left != hash.end()){
                        auto pos_top_left = it_top_left->second;
                        if(nodes[pos_top_left].type == NODE_LEAF){
                            continue;
                        }
                        source_ptr = pos_top_left;
                        source_off_x = x - x_block*block_size;
                        source_off_y = y - y_block *block_size;
                        blocks_to_untouchable.push_back(z_top_left);
                    }

                    for(const auto &b : it_src->second){
                        if(untouchable_block.find(b) != untouchable_block.end() || hash.find(b) == hash.end()) continue;


                        //untouchable_block.insert({z_top_left, 1});

                        auto it_target = hash.find(b);
                        auto pos_target = it_target->second;
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = source_ptr;
                        nodes[pos_target].z_order = b;
                        nodes[pos_target].offset_x = source_off_x;
                        nodes[pos_target].offset_y = source_off_y;
                        remove_target(adjacent_lists, sources, b, block_size);
                    }

                    //hash.erase(it_target);
                    for(const auto &b_to_ut : blocks_to_untouchable){
                        if(untouchable_block.find(b_to_ut) == untouchable_block.end()){
                            untouchable_block.insert({b_to_ut, 1});
                        }
                    }
                }
                ++processed_blocks;
                m_progress_bar.update(processed_blocks);
            }
            m_progress_bar.done();
        }

        /*static void block_replacement_lists(replacements_map_type &replacements_map, replacements_list_type &list_new_block,
                                             const size_type sx_source, const size_type sy_source,
                                             const size_type sx_target, const size_type sy_target){

            //std::cout << "adding new list: " << "<" << (value_type) (sx_source-sx_target) << ", " << (value_type) (sy_source-sy_target) << ">" << std::endl;
            list_new_block.emplace_back(sx_source-sx_target, sy_source-sy_target);
            auto it = replacements_map.find({sx_source, sy_source});
            if(it != replacements_map.end()){
                //std::cout << "adding to list <" << sx_source << ", " << sy_source << "> : " << "<" << (value_type) (sx_target-sx_source) << ", " << (value_type) (sy_target-sy_source) << ">" << std::endl;
                it->second.emplace_back(sx_target-sx_source, sy_target-sy_source);
            }else{
                replacements_list_type list = {{sx_target-sx_source, sy_target-sy_source}};
                replacements_map.insert({{sx_source, sy_source}, list});
                //std::cout << "creating list <" << sx_source << ", " << sy_source << "> : " << "<" << (value_type) (sx_target-sx_source) << ", " << (value_type) (sy_target-sy_source) << ">" << std::endl;
            }
        }*/

        static void roll_replacement_lists(replacements_map_type &replacements_map,
                                            const size_type sx_source, const size_type sy_source,
                                            const size_type ex_source, const size_type ey_source,
                                            const size_type sx_target, const size_type sy_target,
                                            const size_type ex_target, const size_type ey_target){

            //both blocks are overlapping each other
            if(!(sx_source > ex_target || ex_source < sx_target || sy_source > ey_target || ey_source < sy_target)){
                return;
            }

            //std::cout << "adding new list: " << "<" << (value_type) (sx_source-sx_target) << ", " << (value_type) (sy_source-sy_target) << ">" << std::endl;
            auto it = replacements_map.find({sx_target, sy_target});
            if(it != replacements_map.end()){
                //std::cout << "adding to list <" << sx_source << ", " << sy_source << "> : " << "<" << (value_type) (sx_target-sx_source) << ", " << (value_type) (sy_target-sy_source) << ">" << std::endl;
                it->second.emplace_back(sx_source-sx_target, sy_source-sy_target);
            }
        }


        template <class input_type, class hash_table_type>
        static void list_blocks(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                                      const size_type dimensions, const size_type block_size,
                                                      replacements_map_type &replacements_map){

            typedef karp_rabin::kr_block_adjacent_list_v4<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;
            //typedef std::vector<std::pair< size_type , size_type > > replacements_list_type;

            kr_type kr_block(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_source = iterators_value_type(block_size);

            size_type hashes = 0;
            auto blocks_per_row = adjacent_lists.size() / block_size;
            //Total number of blocks
            auto total_blocks = blocks_per_row * blocks_per_row;
            util::progress_bar m_progress_bar(total_blocks);
            //IMPORTANT: kr_block, skips every empty block. For this reason, every node of the current level has to be
            //           initialized as empty node.
            while(kr_block.next()){
#if BT_VERBOSE
                std::cout << "Target: (" << kr_block.col << ", " << kr_block.row << ")" << std::endl;
                std::cout << "Hash: " << kr_block.hash << std::endl;
#endif
                //check if kr_block.hash exists in map
                //Target z-order
                auto z_order_target = codes::zeta_order::encode(kr_block.col, kr_block.row);
                auto processed_blocks = kr_block.row * blocks_per_row + kr_block.col+1;
                /*if(kr_block.row == 1){
                    std::cout << "Block at (" << kr_block.col * block_size << ", " << kr_block.row * block_size << ") with hash: " << kr_block.hash << std::endl;
                }*/
                if(ht.hash_collision(kr_block.hash, it_table, it_hash)){
                    value_type sx_source, sy_source, ex_source, ey_source;
                    value_type sx_target = kr_block.col * block_size;
                    value_type sy_target = kr_block.row * block_size;
                    value_type ex_target = sx_target + block_size-1;
                    value_type ey_target = sy_target + block_size-1;
                    iterator_hash_value_type bucket;
                    if(exist_identical_mult(adjacent_lists, sx_target, sy_target, ex_target, ey_target,
                                       kr_block.iterators, it_hash->second, block_size,
                                       sx_source, sy_source, ex_source, ey_source, iterators_source, bucket)){ //check if they are identical
#if BT_VERBOSE
                        std::cout << "Pointer to source in z-order: " << (source->first) << " offset: <0,0>" << std::endl;
#endif
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_value_collision(bucket, z_order);
                        replacements_map.insert({{sx_target, sy_target}, replacements_list_type()});
                        /*size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);*/
                    }else {
                        //add <z_order> to chain in map
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);
                        replacements_map.insert({{sx_target, sy_target}, replacements_list_type()});
                    }
                }else{
                    //add <z_order> to map [chain should be empty]
                    size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, z_order);
                    replacements_map.insert({{kr_block.col*block_size, kr_block.row*block_size}, replacements_list_type()});
                }
                m_progress_bar.update(processed_blocks);
                ++hashes;
#if BT_VERBOSE
                std::cout << std::endl;
#endif
            }
            //Delete sources of hash_table
            m_progress_bar.done();
            std::cout << "Total hashes: " << hashes << std::endl;
            //print_ajdacent_list(adjacent_lists);
        }

        template <class input_type, class hash_table_type>
        static void list_blocks_v2(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                const size_type dimensions, const size_type block_size){

            typedef karp_rabin::kr_block_adjacent_list_v4<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;
            //typedef std::vector<std::pair< size_type , size_type > > replacements_list_type;

            kr_type kr_block(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_source = iterators_value_type(block_size);

            size_type hashes = 0;
            auto blocks_per_row = adjacent_lists.size() / block_size;
            //Total number of blocks
            auto total_blocks = blocks_per_row * blocks_per_row;
            util::progress_bar m_progress_bar(total_blocks);
            //IMPORTANT: kr_block, skips every empty block. For this reason, every node of the current level has to be
            //           initialized as empty node.
            while(kr_block.next()){
#if BT_VERBOSE
                std::cout << "Target: (" << kr_block.col << ", " << kr_block.row << ")" << std::endl;
                std::cout << "Hash: " << kr_block.hash << std::endl;
#endif
                //check if kr_block.hash exists in map
                //Target z-order
                auto z_order_target = codes::zeta_order::encode(kr_block.col, kr_block.row);
                auto processed_blocks = kr_block.row * blocks_per_row + kr_block.col+1;
                /*if(kr_block.row == 1){
                    std::cout << "Block at (" << kr_block.col * block_size << ", " << kr_block.row * block_size << ") with hash: " << kr_block.hash << std::endl;
                }*/
                if(ht.hash_collision(kr_block.hash, it_table, it_hash)){
                    value_type sx_source, sy_source, ex_source, ey_source;
                    value_type sx_target = kr_block.col * block_size;
                    value_type sy_target = kr_block.row * block_size;
                    value_type ex_target = sx_target + block_size-1;
                    value_type ey_target = sy_target + block_size-1;
                    iterator_hash_value_type bucket;
                    if(exist_identical_mult_v2(adjacent_lists, sx_target, sy_target, ex_target, ey_target,
                                            kr_block.iterators, it_hash->second, block_size,
                                            sx_source, sy_source, ex_source, ey_source, iterators_source, bucket)){ //check if they are identical
#if BT_VERBOSE
                        std::cout << "Pointer to source in z-order: " << (source->first) << " offset: <0,0>" << std::endl;
#endif
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_value_collision(bucket, z_order);


                        //replacements_map.insert({{sx_target, sy_target}, replacements_list_type()});
                        /*size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);*/
                    }else {
                        //add <z_order> to chain in map
                        size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                        ht.insert_hash_collision(it_hash, z_order);
                        //replacements_map.insert({{sx_target, sy_target}, replacements_list_type()});
                    }
                }else{
                    //add <z_order> to map [chain should be empty]
                    size_type z_order = codes::zeta_order::encode(kr_block.col, kr_block.row);
                    ht.insert_no_hash_collision(it_table, kr_block.hash, z_order);
                    //replacements_map.insert({{kr_block.col*block_size, kr_block.row*block_size}, replacements_list_type()});
                }
                m_progress_bar.update(processed_blocks);
                ++hashes;
#if BT_VERBOSE
                std::cout << std::endl;
#endif
            }
            //Delete sources of hash_table
            m_progress_bar.done();
            std::cout << "Total hashes: " << hashes << std::endl;
            //print_ajdacent_list(adjacent_lists);
        }


        template <class input_type, class hash_table_type>
        static void get_type_of_nodes_stack_lite(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                      const size_type dimensions, const size_type block_size,
                                      hash_type &hash, std::vector<node_type> &nodes){

            typedef karp_rabin::kr_roll_adjacent_list_v4<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            size_type hashes = 0;
            auto rolls_per_row = adjacent_lists.size() - block_size + 1;
            //Total number of blocks
            auto total_rolls = rolls_per_row * rolls_per_row;
            util::progress_bar m_progress_bar(total_rolls);

            kr_type kr_roll(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_target = iterators_value_type(block_size);
            while(kr_roll.next()){
#if BT_VERBOSE
                std::cout << "Source: (" << kr_roll.col << ", " << kr_roll.row << ")" << std::endl;
                std::cout << "Hash: " << kr_roll.hash << std::endl;
#endif


                auto processed_rolls = kr_roll.row * rolls_per_row + kr_roll.col+1;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_roll.hash, it_table, it_hash)){
                    value_type sx_target, sy_target, ex_target, ey_target;
                    value_type ex_source = kr_roll.col + block_size-1;
                    value_type ey_source = kr_roll.row + block_size-1;
                    iterator_hash_value_type target;
                    if(exist_identical(adjacent_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                       kr_roll.iterators, it_hash->second, block_size,
                                       sx_target, sy_target, ex_target, ey_target, iterators_target, target)){ //check if they are identical
#if BT_VERBOSE
                        std::cout << "Target: (" << sx_target << ", " << sy_target << ")" << std::endl;
                        std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
#endif
                        //Compute offsets and top-left block
                        size_type x_block, y_block;
                        size_type source_ptr;
                        value_type source_off_x, source_off_y, off_x, off_y;
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
                                source_off_x = kr_roll.col - (x_block + 1)*block_size;
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

                        size_type bits_k2 = 0;
                        //Delete info of <target> from adjacency list
                        auto redo_heap_in = delete_info_shift(adjacent_lists, sx_target, sy_target, ex_target, ey_target, iterators_target,
                                                              kr_roll.row, kr_roll.row + block_size-1, kr_roll.iterators, kr_roll.heap_in, kr_roll.heap_out, block_size);

                        //IMPORTANT: some elements inside the heap (heap_in) were removed
                        // - Update heap_in
                        if(redo_heap_in){
                            kr_roll.redo_heap_in();
                        }

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
                        size_type pos_target = it->second;
                        hash.erase(it);
                        nodes[pos_target].type = NODE_LEAF;
                        nodes[pos_target].ptr = source_ptr;
                        nodes[pos_target].offset_x = source_off_x;
                        nodes[pos_target].offset_y = source_off_y;
                        nodes[pos_target].z_order = z_order_target;
                    }
                }
#if BT_VERBOSE
                std::cout << std::endl;
#endif
                m_progress_bar.update(processed_rolls);
                ++hashes;
            }
            m_progress_bar.done();
            std::cout << "Total hashes: " << hashes << std::endl;
            //print_ajdacent_list(adjacent_lists);
        }

        template <class input_type, class hash_table_type>
        static void list_rolls(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                                                 const size_type dimensions, const size_type block_size,
                                                 replacements_map_type &replacements_map){

            typedef karp_rabin::kr_roll_adjacent_list_v4<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            size_type hashes = 0;
            auto rolls_per_row = adjacent_lists.size() - block_size + 1;
            //Total number of blocks
            auto total_rolls = rolls_per_row * rolls_per_row;
            util::progress_bar m_progress_bar(total_rolls);

            kr_type kr_roll(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_target = iterators_value_type(block_size);
            while(kr_roll.next()){
#if BT_VERBOSE
                std::cout << "Source: (" << kr_roll.col << ", " << kr_roll.row << ")" << std::endl;
                std::cout << "Hash: " << kr_roll.hash << std::endl;
#endif

                /*if(kr_roll.row == 512 && kr_roll.col % 512 == 0){
                    std::cout << "Roll at (" << kr_roll.col << ", " << kr_roll.row << ") with hash: " << kr_roll.hash << std::endl;
                }

                if(kr_roll.col == 241664 && kr_roll.row == 512){
                    std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
                }*/
                //util::logger::log("next done:" + std::to_string(kr_roll.hash));
                auto processed_rolls = kr_roll.row * rolls_per_row + kr_roll.col+1;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_roll.hash, it_table, it_hash)){
                    value_type sx_target, sy_target, ex_target, ey_target;
                    value_type ex_source = kr_roll.col + block_size-1;
                    value_type ey_source = kr_roll.row + block_size-1;
                    iterator_hash_value_type target;
                    if(exist_identical_mult(adjacent_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                       kr_roll.iterators, it_hash->second, block_size,
                                       sx_target, sy_target, ex_target, ey_target, iterators_target, target)){ //check if they are identical

#if BT_VERBOSE
                        std::cout << "Target: (" << sx_target << ", " << sy_target << ")" << std::endl;
                        std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
#endif
                        roll_replacement_lists(replacements_map, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                sx_target, sy_target, ex_target, ey_target);
                        //util::logger::log("roll_replacement.");
                        for(auto it_target = target->second.begin(); it_target != target->second.end(); ++it_target){
                            auto point = codes::zeta_order::decode(*it_target);
                            sx_target = point.first * block_size;
                            sy_target = point.second * block_size;
                            ex_target = sx_target + block_size-1;
                            ey_target = sy_target + block_size-1;
                            roll_replacement_lists(replacements_map, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                                   sx_target, sy_target, ex_target, ey_target);
                        }
                        //util::logger::log("for ends.");

                    }
                    //util::logger::log("no identical.");
                }
#if BT_VERBOSE
                std::cout << std::endl;
#endif
               // util::logger::log("no collision.");
                m_progress_bar.update(processed_rolls);
                ++hashes;
            }
            m_progress_bar.done();
            for(auto &it : replacements_map){
                std::sort(it.second.begin(), it.second.end());
            }
            std::cout << "Total hashes: " << hashes << std::endl;
            //print_ajdacent_list(adjacent_lists);
        }

        template <class input_type, class hash_table_type>
        static void list_rolls_v2(input_type &adjacent_lists, const size_type k, hash_table_type &ht,
                               const size_type dimensions, const size_type block_size,
                               replacements_point_lists_type &replacements_point_lists){

            typedef karp_rabin::kr_roll_adjacent_list_v4<input_type> kr_type;
            typedef typename kr_type::hash_type hash_type;
            typedef std::vector<typename kr_type::iterator_value_type> iterators_value_type;
            // typedef uint64_t iterators_type;
            typedef typename hash_table_type::iterator_table_type iterator_table_type;
            typedef typename hash_table_type::iterator_hash_type  iterator_hash_type;
            typedef typename hash_table_type::iterator_value_type iterator_hash_value_type;

            size_type hashes = 0;
            auto rolls_per_row = adjacent_lists.size() - block_size + 1;
            //Total number of blocks
            auto total_rolls = rolls_per_row * rolls_per_row;
            util::progress_bar m_progress_bar(total_rolls);

            kr_type kr_roll(block_size, prime, adjacent_lists);
            iterator_table_type it_table;
            iterator_hash_type it_hash;
            iterators_value_type iterators_target = iterators_value_type(block_size);
            while(kr_roll.next()){
#if BT_VERBOSE
                std::cout << "Source: (" << kr_roll.col << ", " << kr_roll.row << ")" << std::endl;
                std::cout << "Hash: " << kr_roll.hash << std::endl;
#endif

                /*if(kr_roll.row == 512 && kr_roll.col % 512 == 0){
                    std::cout << "Roll at (" << kr_roll.col << ", " << kr_roll.row << ") with hash: " << kr_roll.hash << std::endl;
                }

                if(kr_roll.col == 241664 && kr_roll.row == 512){
                    std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
                }*/
                //util::logger::log("next done:" + std::to_string(kr_roll.hash));
                auto processed_rolls = kr_roll.row * rolls_per_row + kr_roll.col+1;
                //check if kr_block.hash exists in map
                if(ht.hash_collision(kr_roll.hash, it_table, it_hash)){
                    value_type sx_target, sy_target, ex_target, ey_target;
                    value_type ex_source = kr_roll.col + block_size-1;
                    value_type ey_source = kr_roll.row + block_size-1;
                    iterator_hash_value_type target;
                    if(exist_identical_mult_v2(adjacent_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                                            kr_roll.iterators, it_hash->second, block_size,
                                            sx_target, sy_target, ex_target, ey_target, iterators_target, target)){ //check if they are identical

#if BT_VERBOSE
                        std::cout << "Target: (" << sx_target << ", " << sy_target << ")" << std::endl;
                        std::cout << "Pointer to source in (x,y): " << kr_roll.col << ", " << kr_roll.row << std::endl;
#endif
                        auto pos_replacement_list = target->first;
                        replacements_point_lists[pos_replacement_list].emplace_back(kr_roll.col, kr_roll.row);
                        //roll_replacement_lists(replacements_point_lists, kr_roll.col, kr_roll.row, ex_source, ey_source,
                        //                       sx_target, sy_target, ex_target, ey_target);
                        //util::logger::log("roll_replacement.");
                        /*for(auto it_target = target->second.begin(); it_target != target->second.end(); ++it_target){
                            auto point = codes::zeta_order::decode(*it_target);
                            sx_target = point.first * block_size;
                            sy_target = point.second * block_size;
                            if(kr_roll.col % block_size == 0 && kr_roll.row % block_size == 0){

                            }

                        }*/

                        //util::logger::log("for ends.");

                    }
                    //util::logger::log("no identical.");
                }
#if BT_VERBOSE
                std::cout << std::endl;
#endif
                // util::logger::log("no collision.");
                m_progress_bar.update(processed_rolls);
                ++hashes;
            }
            m_progress_bar.done();
            for(auto &it : replacements_point_lists){
                std::sort(it.begin(), it.end());
            }
            std::cout << "Total hashes: " << hashes << std::endl;
            //print_ajdacent_list(adjacent_lists);
        }

        static void compute_lists_to_check(replacements_map_type &replacements_map,
                                          std::vector<replacements_map_iterator> &lists_to_check,
                                          std::vector<point_type> &empty_positions,
                                          const size_type x, const size_type y, const size_type block_size,
                                          const size_type lower_level_block_size){

            size_type k2 = 0;
            for(size_type y_i = y; y_i < y + block_size; y_i += lower_level_block_size){
                for(size_type x_i = x; x_i < x + block_size; x_i += lower_level_block_size){
                    auto it = replacements_map.find({x_i, y_i});
                    if(it != replacements_map.end()){
                        lists_to_check.push_back(it);
                    }else{
                        empty_positions.emplace_back(x_i, y_i);
                    }
                    ++k2;
                }
            }
        }

        static void lists_intersection(const replacements_map_type &replacements_map,
                                       const std::vector<replacements_map_iterator> &lists_to_check, replacements_list_type &sol){

            if(lists_to_check.empty()) return;
            if(lists_to_check.size() == 1){
                sol = lists_to_check[0]->second;
                return;
            }
            std::vector<diff_cord_type> aux_sol;
            std::set_intersection(lists_to_check[0]->second.begin(), lists_to_check[0]->second.end(),
                    lists_to_check[1]->second.begin(), lists_to_check[1]->second.end(),
                    std::back_inserter(sol));

            size_type i = 2;
            while(i < lists_to_check.size() && !sol.empty()){
                aux_sol = std::move(sol);
                std::set_intersection(aux_sol.begin(), aux_sol.end(),
                                      lists_to_check[i]->second.begin(), lists_to_check[i]->second.end(),
                                      std::back_inserter(sol));
                ++i;
            }

        }

        static void lists_intersection_v2(replacements_point_lists_type &replacements_point_lists,
                                          const std::vector<size_type> &lists_to_check,
                                          replacements_point_list_type &sol){

            if(lists_to_check.empty()) return;
            if(lists_to_check.size() == 1){
                sol = replacements_point_lists[lists_to_check[0]];
                return;
            }

            util::intersection_lists_no_reps(replacements_point_lists[lists_to_check[0]],
                    replacements_point_lists[lists_to_check[1]], sol);
            size_type i = 2;
            while(i < lists_to_check.size() && !sol.empty()){
                util::intersection_lists_no_reps(sol, replacements_point_lists[lists_to_check[2]]);
                ++i;
            }

        }

        /*static void replacements_init(replacements_map_type &replacements_map, size_type block_size, size_type matrix_size, size_type k){
            for(size_type y = 0; y < matrix_size; y += block_size){
                for(size_type x = 0; x < matrix_size; x += block_size){
                    std::cout << "Current block: (" << x << ", " << y << ")" << std::endl;
                    std::vector<replacements_map_iterator> lists_to_check;
                    replacements_list_type sol;
                    compute_lists_to_check(replacements_map, lists_to_check, x, y, block_size);
                }
            }
        }*/

        template <class input_type>
        static void filter_solution_with_empty_areas(input_type &adjacency_lists, const size_type block_size,
                                                     const std::vector<point_type> &empty_positions,
                                                     replacements_list_type &sol){
            replacements_list_type aux_sol;
            for (const auto &s_i : sol) {
                bool sol_ok = false;
                for (const auto &e_i : empty_positions) {
                    value_type x = e_i.first + s_i.first;
                    value_type y = e_i.second + s_i.second;
                    if(x < 0 || y < 0){
                        sol_ok = false;
                        break;
                    }
                    if(is_empty(adjacency_lists, x, y, block_size)) {
                        sol_ok = true;
                    } else {
                        sol_ok = false;
                        break;
                    }
                }
                if (sol_ok) {
                    aux_sol.push_back(s_i);
                }
            }
            sol = std::move(aux_sol);
        };

        template <class input_type>
        static void filter_solution_with_empty_areas_v2(input_type &adjacency_lists,
                                                     const size_type x, const size_type y,
                                                     const size_type block_size,
                                                     const std::vector<size_type > &empty_positions,
                                                     replacements_point_list_type &sol){
            replacements_point_list_type aux_sol;
            for (const auto &s_i : sol) {
                bool sol_ok = false;
                for (const auto &e_i : empty_positions) {
                    auto p_i = codes::zeta_order::decode(e_i);
                    value_type x_i = (p_i.first - x) + s_i.first;
                    value_type y_i = (p_i.second - y) + s_i.second;
                    if(x_i < 0 || y_i < 0){
                        sol_ok = false;
                        break;
                    }
                    if(is_empty(adjacency_lists, x_i, y_i, block_size)) {
                        sol_ok = true;
                    } else {
                        sol_ok = false;
                        break;
                    }
                }
                if (sol_ok) {
                    aux_sol.push_back(s_i);
                }
            }
            sol = std::move(aux_sol);
        };

        static void filter_solution_overlapped_areas(const size_type block_size,
                                                     replacements_list_type &sol){
            replacements_list_type aux_sol;
            for (const auto &s_i : sol) {
                if(std::abs(s_i.first) >= block_size || std::abs(s_i.second) >= block_size){
                    aux_sol.push_back(s_i);
                }
            }
            sol = std::move(aux_sol);

        }

        static void filter_solution_overlapped_areas_v2(const size_type x, const size_type y,
                                                        const size_type block_size,
                                                        replacements_point_list_type &sol){
            replacements_point_list_type aux_sol;
            for (const auto &s_i : sol) {
                if(std::abs(static_cast<long>(s_i.first - x)) >= block_size ||
                   std::abs(static_cast<long>(s_i.second - y)) >= block_size){
                    aux_sol.push_back(s_i);
                }
            }
            sol = std::move(aux_sol);

        }

        template <class input_type>
        static bool replacements_to_prev_level(input_type &adjacency_lists, replacements_map_type &replacements_map,
                                               const size_type block_size,
                                               const size_type matrix_size, const size_type k){



            replacements_map_type new_replacements_map;
            size_type lower_level_block_size = block_size / k;
            bool b = false;
            //std::string file_name = "checking_blocks" + std::to_string(block_size) + ".out";
            //std::ofstream check_blocks(file_name);
            for(size_type y = 0; y < matrix_size; y += block_size){
                for(size_type x = 0; x < matrix_size; x += block_size){
                    //check_blocks << "Check block: (" << x << ", " << y << ") " << std::endl;
                    std::vector<replacements_map_iterator> lists_to_check;
                    replacements_list_type sol;
                    std::vector<point_type> empty_positions;
                    compute_lists_to_check(replacements_map, lists_to_check, empty_positions,
                            x, y, block_size, lower_level_block_size);
                    if(!lists_to_check.empty()){ //Not empty area
                        //check_blocks << "Not empty" << std::endl;
                        lists_intersection(replacements_map, lists_to_check, sol);
                        //check_blocks << "Intersection: " << sol.size() << std::endl;
                        filter_solution_overlapped_areas(block_size, sol);
                        //check_blocks << "Overlapping: " << sol.size() << std::endl;
                        filter_solution_with_empty_areas(adjacency_lists, lower_level_block_size, empty_positions, sol);
                        /*if(lists_to_check.size() < k*k ) {  //There are empty areas
                            check_blocks << "Empty: " << sol.size() << std::endl;
                        }*/
                        //check_blocks << "Solution: " << !sol.empty() << std::endl;
                        new_replacements_map.insert({{x, y}, sol});
                        b = b || !sol.empty();
                    }
                    //check_blocks << "Done." << std::endl;

                }
            }
            //check_blocks.close();
            replacements_map = std::move(new_replacements_map);
            return b;
        }

        template <class input_type>
        static bool exist_replacements(input_type &adjacency_lists, replacements_map_type &replacements_map,
                                        const size_type block_size, const size_type block_size_replacements,
                                        const size_type matrix_size, const size_type k){



            bool b = false;
            //std::string file_name = "checking_blocks" + std::to_string(block_size) + ".out";
            //std::ofstream check_blocks(file_name);
            for(size_type y = 0; y < matrix_size; y += block_size){
                for(size_type x = 0; x < matrix_size; x += block_size){
                    //check_blocks << "Check block: (" << x << ", " << y << ") " << std::endl;
                    std::vector<replacements_map_iterator> lists_to_check;
                    replacements_list_type sol;
                    std::vector<point_type> empty_positions;
                    compute_lists_to_check(replacements_map, lists_to_check, empty_positions, x, y,
                            block_size, block_size_replacements);
                    if(!lists_to_check.empty()){ //Not empty area
                        //check_blocks << "Not empty:" << lists_to_check.size() << std::endl;
                        lists_intersection(replacements_map, lists_to_check, sol);
                        //check_blocks << "Intersection: " << sol.size() << std::endl;
                        filter_solution_overlapped_areas(block_size, sol);
                        //check_blocks << "Overlapping: " << sol.size() << std::endl;
                        filter_solution_with_empty_areas(adjacency_lists, block_size_replacements, empty_positions, sol);
                        //check_blocks << "Solution: " << !sol.empty() << std::endl;
                        //check_blocks << "{";
                        //for(const auto &s : sol){
                        //    check_blocks << "(" << s.first << ", " << s.second << "), ";
                        //}
                        //check_blocks << "}" << std::endl;
                        b = b || !sol.empty();
                    }
                    //check_blocks << "Done." << std::endl;
                }
            }
            //check_blocks.close();
            return b;
        }

        static void compute_lists_to_check_v2(std::unordered_map<size_type, size_type> &map_blocks_keys,
                                           std::vector<size_type> &lists_to_check,
                                           std::vector<size_type> &empty_positions,
                                           const size_type x, const size_type y, const size_type block_size,
                                           const size_type lower_level_block_size){

            size_type k2 = 0;
            for(size_type y_i = y; y_i < y + block_size; y_i += lower_level_block_size){
                for(size_type x_i = x; x_i < x + block_size; x_i += lower_level_block_size){
                    auto z_order = codes::zeta_order::encode(x / lower_level_block_size, y/lower_level_block_size);
                    auto it = map_blocks_keys.find(z_order);
                    if(it != map_blocks_keys.end()){
                        lists_to_check.push_back(it->second);
                    }else{
                        empty_positions.emplace_back(z_order);
                    }
                    ++k2;
                }
            }
        }


        template <class input_type>
        static bool exist_replacements_v2(input_type &adjacency_lists,
                                       std::unordered_map<size_type, size_type> &map_blocks_keys,
                                       replacements_point_lists_type &replacements_point_lists,
                                       const size_type block_size, const size_type block_size_replacements,
                                       const size_type matrix_size, const size_type k){

            //std::string file_name = "checking_blocks" + std::to_string(block_size) + ".out";
            //std::ofstream check_blocks(file_name);
            for(size_type y = 0; y < matrix_size; y += block_size){
                for(size_type x = 0; x < matrix_size; x += block_size){
                    //check_blocks << "Check block: (" << x << ", " << y << ") " << std::endl;
                    std::vector<size_type > lists_to_check, empty_positions;
                    replacements_point_list_type sol;
                    compute_lists_to_check_v2(map_blocks_keys, lists_to_check, empty_positions, x, y,
                                           block_size, block_size_replacements);
                    if(!lists_to_check.empty()){ //Not empty area
                        //check_blocks << "Not empty:" << lists_to_check.size() << std::endl;
                        lists_intersection_v2(replacements_point_lists, lists_to_check, sol);
                        //check_blocks << "Intersection: " << sol.size() << std::endl;
                        filter_solution_overlapped_areas_v2(x, y, block_size, sol);
                        //check_blocks << "Overlapping: " << sol.size() << std::endl;
                        filter_solution_with_empty_areas_v2(adjacency_lists, x, y, block_size_replacements, empty_positions, sol);
                        //check_blocks << "Solution: " << !sol.empty() << std::endl;
                        //check_blocks << "{";
                        //for(const auto &s : sol){
                        //    check_blocks << "(" << s.first << ", " << s.second << "), ";
                        //}
                        //check_blocks << "}" << std::endl;
                        if(!sol.empty()) return true;
                    }
                    //check_blocks << "Done." << std::endl;
                }
            }
            //check_blocks.close();
            return false;
        }

        template <class input_type>
        static bool update_replacements(input_type &adjacency_lists, replacements_map_type &replacements_map,
                                               const size_type block_size, const size_type block_size_replacements,
                                               const size_type matrix_size, const size_type k){



            replacements_map_type new_replacements_map;
            bool b = false;
            for(size_type y = 0; y < matrix_size; y += block_size){
                for(size_type x = 0; x < matrix_size; x += block_size){
                    std::vector<replacements_map_iterator> lists_to_check;
                    replacements_list_type sol;
                    std::vector<point_type> empty_positions;
                    compute_lists_to_check(replacements_map, lists_to_check, empty_positions, x, y, block_size, block_size_replacements);
                    if(!lists_to_check.empty()){ //Not empty area
                        lists_intersection(replacements_map, lists_to_check, sol);
                        filter_solution_overlapped_areas(block_size, sol);
                        filter_solution_with_empty_areas(adjacency_lists, block_size_replacements, empty_positions, sol);
                        new_replacements_map.insert({{x, y}, sol});
                        b = b || !sol.empty();
                    }
                }
            }
            replacements_map = std::move(new_replacements_map);
            return b;
        }
                    /*if(compute_lists_to_check(replacements_map, lists_to_check, empty_positions, x, y, block_size, lower_level_block_size)
                            && lists_intersection(replacements_map, lists_to_check, sol)){
                        //TODO: limpiar este codigo
                        if(lists_to_check.size() < k*k ){  //There are empty areas
                            replacements_list_type aux_sol;
                            for(const auto &s_i : sol){
                                bool sol_ok = false;
                                for(const auto &e_i : empty_positions){
                                    auto p_emp = (point_type) {e_i.first + s_i.first, e_i.second + s_i.second};
                                    if(replacements_map.find(p_emp) == replacements_map.end()){
                                        sol_ok = true;
                                    }else{
                                        sol_ok = false;
                                        break;
                                    }
                                }
                                if(sol_ok) {
                                    aux_sol.push_back(s_i);
                                    std::cout << "Empty positions: " << empty_positions.size() << std::endl;
                                    std::cout << "With empty areas ok" << std::endl;
                                }
                            }
                            if(!aux_sol.empty()){
                                new_replacements_map.insert({{x, y}, aux_sol});
                            }
                        }else{
                            new_replacements_map.insert({{x, y}, sol});
                            std::cout << "No empty areas ok" << std::endl;
                        };
                    }
                }
            }
            replacements_map = std::move(new_replacements_map);
        }*/


        template <class input_type>
        static void compute_last_level(input_type &adjacent_lists, hash_type &hash, std::vector<node_type> &nodes){
            auto it = adjacent_lists.begin();
            auto y = 0;
            while(it != adjacent_lists.end()){
                auto it_e = it->begin();
                while(it_e != it->end()){
                    auto z_order = codes::zeta_order::encode(*it_e, y);
                    auto pos_target = hash.find(z_order)->second;
                    nodes[pos_target].type = NODE_INTERNAL;
                    ++it_e;
                }
                ++it;
                ++y;
            }
        }



        template <class input_type, class hash_type>
        static size_type build_k2_tree(const input_type &adjacent_lists, const size_type k,
                                  const size_type height, const size_type block_size_stop,
                                  sdsl::bit_vector &bits_t, hash_type &hash){


            typedef std::tuple<size_type , size_type, size_type,size_type> t_part_tuple;
            auto k_2 = k * k;

            //1. Edges z-order
            std::vector<size_type> edges_z_order;
            for(size_type y = 0; y < adjacent_lists.size(); ++y){
                for(size_type x : adjacent_lists[y]){
                    edges_z_order.push_back(codes::zeta_order::encode(x, y));
                }
            }

            //2. Sort edges z-order
            std::sort(edges_z_order.begin(), edges_z_order.end());

            //4. Init bitmap
            bits_t = sdsl::bit_vector(k_2 * (height-1) * edges_z_order.size(), 0);
            bits_t[0] = 1;

            size_type l = adjacent_lists.size();
            std::queue<t_part_tuple> q;
            if(l > block_size_stop){
                q.push(t_part_tuple(0, edges_z_order.size()-1, l/k , 0));
            }else{
                //Preparing next level
                size_type n_elem = 0;
                for(size_type z_order = 0; z_order < k_2; ++z_order){
                    hash.insert({z_order, n_elem});
                    ++n_elem;
                }
            }
            size_type i, j, z_0;
            size_type t = k_2;
            size_type n_elem = 0, zeroes = k_2 -1;

            //5. Split the front of q into its children
            while (!q.empty()) {
                std::tie(i, j, l, z_0) = q.front();
                q.pop();
                auto elements = l * l;
                for(size_type z_child = 0; z_child < k_2; ++z_child){
                    auto le = util::search::lower_or_equal_search(i, j, edges_z_order, z_0+elements-1);
                    if(le != -1 && edges_z_order[le] >= z_0){
                        bits_t[t] = 1;
                        if(l > block_size_stop){
                            q.push(t_part_tuple(i, le, l/k, z_0));
                        }else{
                            //Preparing next level
                            for(size_type z_order = z_0/elements * k_2; z_order < (z_0/elements+1)*k_2; ++z_order){
                                hash.insert({z_order, n_elem});
                                ++n_elem;
                            }
                        }
                        i = le + 1;
                    }else{
                        ++zeroes;
                    }
                    ++t;
                    z_0 += elements;
                }
            }
            bits_t.resize(t);
            return zeroes;

        }



        template <class input_type>
        static size_type bits_last_k2_tree(const input_type &adjacent_lists, const size_type k,
                                           const size_type block_size_start){


            typedef std::tuple<size_type , size_type, size_type,size_type> t_part_tuple;
            auto k_2 = k * k;

            //1. Edges z-order
            std::vector<size_type> edges_z_order;
            for(size_type y = 0; y < adjacent_lists.size(); ++y){
                for(value_type x : adjacent_lists[y]){
                    //taking into account that some values are marked to delete
                    if(x < 0){
                        x=(-x)-1;
                    }
                    edges_z_order.push_back(codes::zeta_order::encode(static_cast<size_type>(x), y));
                }
            }

            //2. Sort edges z-order
            std::sort(edges_z_order.begin(), edges_z_order.end());

            //4. Resize topology bitmap
            size_type bits = 0;

            //5. Split the front of q into its children
            size_type i, j, z_0;
            size_type l = adjacent_lists.size();
            std::queue<t_part_tuple> q;
            q.push(t_part_tuple(0, edges_z_order.size()-1, l/k , 0));
            while (!q.empty()) {
                std::tie(i, j, l, z_0) = q.front();
                q.pop();
                auto elements = l * l;
                for(size_type z_child = 0; z_child < k_2; ++z_child){
                    auto le = util::search::lower_or_equal_search(i, j, edges_z_order, z_0+elements-1);
                    if(le != -1 && edges_z_order[le] >= z_0){
                        if(l <= block_size_start){
                            ++bits;
                        }
                        if(l / k > 0){
                            q.push(t_part_tuple(i, le, l/k, z_0));
                        }
                        i = le + 1;
                    }else{
                        if(l <= block_size_start) {
                           ++bits;
                        }
                    }

                    z_0 += elements;
                }
            }
            return bits;

        }


        template <class input_type>
        static void build_last_k2_tree(const input_type &adjacent_lists, const size_type k,
                                            const size_type height, const size_type block_size_start,
                                            sdsl::bit_vector &t_bits, sdsl::bit_vector &l_bits){


            typedef std::tuple<size_type , size_type, size_type,size_type> t_part_tuple;
            auto k_2 = k * k;

            //1. Edges z-order
            std::vector<size_type> edges_z_order;
            for(size_type y = 0; y < adjacent_lists.size(); ++y){
                for(value_type x : adjacent_lists[y]){
                    //taking into account that some values are marked to delete
                    if(x < 0){
                        x=(-x)-1;
                    }
                    edges_z_order.push_back(codes::zeta_order::encode(static_cast<size_type>(x), y));
                }
            }

            //2. Sort edges z-order
            std::sort(edges_z_order.begin(), edges_z_order.end());

            //4. Resize topology bitmap
            size_type t = t_bits.size(), l = 0;
            t_bits.resize(t + k_2 * (height-1) * edges_z_order.size());
            l_bits.resize(k_2 * edges_z_order.size());

            //5. Split the front of q into its children
            size_type i, j, z_0;
            size_type block_size = adjacent_lists.size();
            std::queue<t_part_tuple> q;
            q.push(t_part_tuple(0, edges_z_order.size()-1, block_size/k , 0));
            while (!q.empty()) {
                std::tie(i, j, block_size, z_0) = q.front();
                q.pop();
                auto elements = block_size * block_size;
                for(size_type z_child = 0; z_child < k_2; ++z_child){
                    auto le = util::search::lower_or_equal_search(i, j, edges_z_order, z_0+elements-1);
                    if(le != -1 && edges_z_order[le] >= z_0){
                        if(block_size <= block_size_start){
                            if(block_size > 1){
                                t_bits[t] = 1;
                                ++t;
                            }else{
                                l_bits[l]=1;
                                ++l;
                            }

                        }
                        if(block_size / k > 0){
                            q.push(t_part_tuple(i, le, block_size/k, z_0));
                        }
                        i = le + 1;
                    }else{
                        if(block_size <= block_size_start) {
                            if(block_size > 1){
                                t_bits[t] = 0;
                                ++t;
                            }else{
                                l_bits[l]= 0;
                                ++l;
                            }
                        }
                    }

                    z_0 += elements;
                }
            }
            t_bits.resize(t);
            l_bits.resize(l);

        }

        static size_type max_bits_k2_tree(const size_type height, const size_type k_2){
            return static_cast<size_type >(std::pow(k_2, height) - 1) / (k_2 - 1);
        }

        static size_type max_bits_pointer(const size_type nodes, const size_type block_size){
            return std::ceil(std::log2(nodes)) + 2*std::ceil(std::log2(block_size));
        }
    };
}

#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_ALGORITHM_HELPER_V2_HPP
