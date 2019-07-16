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
// Created by Adrián on 15/07/2019.
//

#ifndef INC_KARP_RABIN_ROLL_ADJACENT_LIST_HPP
#define INC_KARP_RABIN_ROLL_ADJACENT_LIST_HPP

#include <cstdint>
#include <vector>

namespace karp_rabin {

    template <bool roll = false, class t_input = std::vector<std::vector<uint64_t>>>
    class kr_roll_adjacent_list {

    public:
        typedef uint64_t size_type;
        typedef uint64_t hash_type;
        typedef t_input  input_type;
        typedef typename input_type::iterator iterator_list_type;
        typedef typename input_type::value_type::iterator iterator_value_type;

    private:

        size_type m_block_size;
        size_type m_prime;
        size_type m_asize = 2;
        size_type m_asize_pow_block; //asize^block_size

        hash_type m_hash;
        size_type m_ith_shift_in_row = 0;
        size_type m_total_shifts_in_row; // number of shifts

        iterator_list_type m_iterator_list;
        iterator_list_type m_end_list;

        std::vector<hash_type> m_h_in_down;
        std::vector<hash_type> m_h_out_down;
        std::vector<hash_type> m_h_in_right;
        std::vector<hash_type> m_h_out_right;

        std::vector<iterator_value_type> m_iterators_beg;
        std::vector<iterator_value_type> m_iterators_end;

        /*void init_iterators_first_row(const iterator_list_type &iterator){
            m_iterator_list = iterator;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_value[list_id] = (*(m_iterator_list+list_id)).begin();
            }
        }*/

        void init_iterators(const iterator_list_type &iterator){
            m_iterator_list = iterator;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_beg[list_id] = (*(m_iterator_list+list_id)).begin();
                m_iterators_end[list_id] = m_iterators_beg[list_id];
            }
            m_ith_shift_in_row = 0;
        }

        void init_hs(){


            m_h_in_down[m_block_size-1]=1;
            for(int64_t i = m_block_size-1; i > 0; --i){
                m_h_in_down[i-1] = (m_h_in_down[i] * m_asize) % m_prime;
            }

            m_h_in_right[m_block_size-1] = m_h_in_down[m_block_size-1];
            m_h_out_right[m_block_size-1] = m_h_in_down[0];
            for(int64_t i = m_block_size-1; i > 0; --i){
                m_h_in_right[i-1] = (m_h_out_right[i] * m_asize) % m_prime;
                m_h_out_right[i-1] = m_h_in_right[i-1];
                for(auto j = 1; j < m_block_size; ++j){
                    m_h_out_right[i-1] = (m_h_out_right[i-1] * m_asize) % m_prime;
                }
            }

            m_h_out_down[m_block_size-1] = m_h_in_right[0];
            for(int64_t i = m_block_size-1; i > 0; --i){
                m_h_out_down[i-1] = (m_h_out_down[i] * m_asize) % m_prime;
            }
            //Shift down out
            /*m_h_out_down[0]=1;
            for(auto i = 1; i < m_block_size; ++i){
                m_h_out_down[i] = (m_h_out_down[i-1] * m_asize) % m_prime;
            }*/
            //Shift right out and in
            /*m_h_in_right[0]=m_h_out_down[m_block_size-1];
            m_h_out_right[0]=1;
            for(auto i = 1; i < m_block_size; ++i){
                m_h_out_right[i] = (m_h_in_right[i-1] * m_asize) % m_prime;
                m_h_in_right[i] = m_h_out_right[i];
                for(auto j = 1; j < m_block_size; ++j){
                    m_h_in_right[i] = (m_h_in_right[i] * m_asize) % m_prime;
                }
            }

            //Shift down in
            m_h_in_down[0] = m_h_out_right[m_block_size-1];
            for(auto i = 1; i < m_block_size; ++i){
                m_h_in_down[i] = (m_h_in_down[i-1] * m_asize) % m_prime;
            }*/
        }

        hash_type compute_initial_hash_block(){

            size_type list_id = 0;
            hash_type hash_value = 0;
            //1. Iterate over the adjacent lists
            for(auto it_list = m_iterator_list; it_list != m_iterator_list + m_block_size; ++it_list){
                int64_t prev_value = -1;
                //2. Iterate over the elements of an adjacent list
                auto &it_element = m_iterators_end[list_id];
                while(it_element != (*it_list).end() && (*it_element) < m_block_size){
                    //2.1 Compute hash_value with 0s
                    for(auto v = prev_value+1; v < (*it_element); ++v){
                        hash_value = (hash_value * m_asize) % m_prime;
                    }
                    //2.2 Compute hash_value with 1
                    hash_value = (hash_value * m_asize + 1) % m_prime;
                    //2.3 Next element of adjacent list
                    prev_value = (*it_element);
                    ++it_element;
                }
                //3. Check the last element and compute hash_value with 0s
                //prev_value is always smaller than block_size
                for(auto i = prev_value + 1; i < m_block_size; ++i){
                    hash_value = (hash_value * m_asize) % m_prime;
                }
                ++list_id;
            }
            return hash_value;
        }

    public:

        const hash_type &hash = m_hash;

        kr_roll_adjacent_list(size_type bs, size_type q, size_type as){
            m_block_size = bs;
            m_prime = q;
            m_asize = as;
            m_asize_pow_block = m_asize;
            for(size_type i = 1; i < m_block_size; ++i){
                m_asize_pow_block = (m_asize_pow_block * m_asize) % m_prime;
            }
            m_iterators_beg = std::vector<iterator_value_type>(m_block_size);
            m_iterators_end = std::vector<iterator_value_type>(m_block_size);
            m_h_in_down = std::vector<hash_type>(m_block_size);
            m_h_in_right = std::vector<hash_type>(m_block_size);
            m_h_out_down = std::vector<hash_type>(m_block_size);
            m_h_out_right = std::vector<hash_type>(m_block_size);
        }

        //pre: list with size at least block_size
        void init(const iterator_list_type &iterator, const iterator_list_type &end_list, size_type total_shifts){
            m_total_shifts_in_row = total_shifts;
            m_end_list = end_list;
            init_iterators(iterator);
            init_hs();
            m_hash = compute_initial_hash_block();
        }

        void shift_right(){
            auto hash = m_hash;
            //Delete previous part
            for(auto i = 0; i < m_block_size; ++i){
                if(m_iterators_beg[i] != (m_iterator_list+i)->end() && *(m_iterators_beg[i]) == m_ith_shift_in_row){
                    hash += (m_prime - m_h_out_right[i]) % m_prime;
                    ++m_iterators_beg[i];
                }
            }
            hash = (hash * m_asize) % m_prime;
            for(auto i = 0; i < m_block_size; ++i){
                if(m_iterators_end[i] != (m_iterator_list+i)->end() && *(m_iterators_end[i]) == m_ith_shift_in_row + m_block_size){
                    hash = (hash + m_h_in_right[i]) % m_prime;
                    ++m_iterators_end[i];
                }
            }
            m_hash = hash;
            ++m_ith_shift_in_row;
        }

        void next_row(hash_type h){
            m_hash = h;
            auto hash = m_hash;
            //Delete previous part
            for(auto it_element = m_iterator_list->begin();
                it_element != m_iterator_list->end() && (*it_element) < m_block_size;
                ++it_element){
                hash += (m_prime - m_h_out_down[(*it_element)]) % m_prime;
            }
            //Next list and init the block_size pointers
            ++m_iterator_list;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_beg[list_id] = (m_iterator_list+list_id)->begin();
                m_iterators_end[list_id] = m_iterators_beg[list_id];
            }
            m_ith_shift_in_row = 0;
            hash = (hash * m_asize_pow_block) % m_prime;
            for(auto &it_element = m_iterators_end[m_block_size-1];
                it_element != (m_iterator_list+m_block_size-1)->end() && (*it_element) < m_block_size;
                ++it_element){

                hash = (hash + m_h_in_down[(*it_element)]) % m_prime;
            }
            m_hash = hash;
            //Skip the location of the ones within the first block
            for(auto i = 0; i < m_block_size-1; ++i){
                for(auto &it_element = m_iterators_end[i];
                    it_element != (m_iterator_list+i)->end() && (*it_element) < m_block_size;
                    ++it_element){
                    //nothing to do. Pointers are updated in ++it_element
                }
            }
        }

        /*bool next(){
            ++m_ith_block_in_row;
            if(m_ith_block_in_row == m_total_blocks_in_row){
                if(!init_iterators_next_row()) return false;
            }
            m_hash = compute_hash_block();
            return true;
        }*/

    };


}

#endif //INC_KARP_RABIN_ROLL_ADJACENT_LIST_HPP
