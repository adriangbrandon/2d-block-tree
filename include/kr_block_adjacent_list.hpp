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

#ifndef INC_KARP_RABIN_BLOCK_ADJACENT_LIST_HPP
#define INC_KARP_RABIN_BLOCK_ADJACENT_LIST_HPP

#include <cstdint>
#include <vector>

namespace karp_rabin {

    template <class t_input = std::vector<std::vector<uint64_t>>>
    class kr_block_adjacent_list {

    public:
        typedef uint64_t size_type;
        typedef uint64_t hash_type;
        typedef t_input  input_type;
        typedef typename input_type::iterator iterator_list_type;
        typedef typename input_type::value_type::iterator iterator_value_type;

    private:

        size_type m_block_size;
        size_type m_prime;
        size_type m_asize;

        hash_type m_hash;
        size_type m_ith_block_in_row = -1;
        size_type m_total_blocks_in_row; //side in number of blocks

        iterator_list_type m_iterator_list;
        iterator_list_type m_end_list;
        std::vector<iterator_value_type> m_iterators_value;

        void copy(const kr_block_adjacent_list &p){
            m_block_size = p.m_block_size;
            m_prime = p.m_prime;
            m_asize = p.m_asize;
            m_hash = p.m_hash;
            m_ith_block_in_row = p.m_ith_block_in_row;
            m_total_blocks_in_row = p.m_total_blocks_in_row;
            m_iterator_list = p.m_iterator_list;
            m_end_list = p.m_end_list;
            m_iterators_value = p.m_iterators_value;
        }

        void init_iterators_first_row(const iterator_list_type &iterator){
            m_iterator_list = iterator;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_value[list_id] = (*(m_iterator_list+list_id)).begin();
            }
        }

        bool init_iterators_next_row(){
            m_iterator_list = m_iterator_list + m_block_size;
            if(m_iterator_list == m_end_list) return false;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_value[list_id] = (*(m_iterator_list+list_id)).begin();
            }
            m_ith_block_in_row = 0;
            return true;
        }

        hash_type compute_hash_block(){

            size_type list_id = 0;
            hash_type hash_value = 0;
            //1. Iterate over the adjacent lists
            for(auto it_list = m_iterator_list; it_list != m_iterator_list + m_block_size; ++it_list){
                int64_t prev_value = m_ith_block_in_row * m_block_size -1;
                //2. Iterate over the elements of an adjacent list
                auto &it_element = m_iterators_value[list_id];
                while(it_element != (*it_list).end() && (*it_element) < (m_ith_block_in_row+1)* m_block_size){
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
                for(auto i = prev_value + 1; i < (m_ith_block_in_row+1)*m_block_size; ++i){
                    hash_value = (hash_value * m_asize) % m_prime;
                }
                ++list_id;
            }
            return hash_value;
        }

    public:

        const hash_type &hash = m_hash;
        const std::vector<iterator_value_type> &iterators_value = m_iterators_value;

        kr_block_adjacent_list() = default;

        kr_block_adjacent_list(size_type bs, size_type q, size_type as){
            m_block_size = bs;
            m_prime = q;
            m_asize = as;
            m_iterators_value = std::vector<iterator_value_type>(m_block_size);
        }

        //pre: list with size at least block_size
        void init(const iterator_list_type &iterator, const iterator_list_type &end_list, size_type total_blocks_in_row){
            m_total_blocks_in_row = total_blocks_in_row;
            m_end_list = end_list;
            init_iterators_first_row(iterator);
            //m_hash = compute_hash_block();
        }


        bool next(){
            ++m_ith_block_in_row;
            if(m_ith_block_in_row == m_total_blocks_in_row){
                if(!init_iterators_next_row()) return false;
            }
            m_hash = compute_hash_block();
            return true;
        }

    };


}

#endif //INC_KARP_RABIN_BLOCK_ADJACENT_LIST_HPP
