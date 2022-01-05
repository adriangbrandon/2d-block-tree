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

#ifndef INC_KARP_RABIN_BLOCK_FASTER_HPP
#define INC_KARP_RABIN_BLOCK_FASTER_HPP

#include <cstdint>
#include <vector>
#include <iostream>
#include <type_traits>
#include <math_util.hpp>
#include <bithacks.hpp>

namespace karp_rabin {

    template <class t_input = std::vector<std::vector<int64_t>>>
    class kr_block_faster {

    public:
        typedef __uint128_t uint128_t;
        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef uint64_t hash_type;
        typedef t_input  input_type;
        typedef typename input_type::iterator iterator_list_type;
        typedef typename input_type::value_type::iterator iterator_value_type;

    private:

        static constexpr size_type m_prime_pow = 61;
        static constexpr size_type m_prime = (1ull << 61) - 1;

        size_type m_x;
        size_type m_block_size;
        hash_type m_hash;
        value_type m_row = 0;
        value_type m_col = 0;
        size_type m_number_ones = 0;
        value_type m_x_point = 0;
        value_type m_y_point = 0;
        iterator_list_type m_iterator_list;
        iterator_list_type m_end_list;
        std::vector<iterator_value_type> m_iterators_value;
        std::vector<hash_type> m_h_length;

        void copy(const kr_block_faster &p){
            m_block_size = p.m_block_size;
            m_hash = p.m_hash;
            m_row = p.m_row;
            m_col = p.m_col;
            m_iterator_list = p.m_iterator_list;
            m_end_list = p.m_end_list;
            m_iterators_value = p.m_iterators_value;
            m_h_length = p.m_h_length;
        }

        void init_iterators_first_row(const iterator_list_type &iterator){
            m_iterator_list = iterator;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_value[list_id] = (m_iterator_list+list_id)->begin();
            }
        }

        bool init_iterators_next_row(){
            m_iterator_list = m_iterator_list + m_block_size;
            if(m_iterator_list == m_end_list) return false;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_value[list_id] = (m_iterator_list+list_id)->begin();
                while(m_iterators_value[list_id] != (m_iterator_list+list_id)->end() && *m_iterators_value[list_id] < 0){
                    ++m_iterators_value[list_id];
                }
            }
            ++m_row;
            return true;
        }

        /**
         *
         * @return 0 = next row 1 = ok
         */
        int compute_ith_block_in_row(){
            auto min = (size_type) -1;
            bool next_row = true;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                if(m_iterators_value[list_id] != (m_iterator_list+list_id)->end()){
                    if(min == -1){
                        min = *(m_iterators_value[list_id]);
                    }else if(min > *(m_iterators_value[list_id])){
                        min = *(m_iterators_value[list_id]);
                    }
                    next_row = false;
                }
            }
            if(next_row){
                return 0;
                /*if(!init_iterators_next_row()){
                    return false;
                };
                return compute_ith_block_in_row();*/
            }
            m_col = min / m_block_size;
            return 1;
        }

        hash_type compute_hash_block(){

            size_type list_id = 0;
            hash_type hash_value = 0;
            m_number_ones = 0;
            //1. Iterate over the adjacent lists
            for(auto it_list = m_iterator_list; it_list != m_iterator_list + m_block_size; ++it_list){
                int64_t prev_value = m_col * m_block_size -1;
                //2. Iterate over the elements of an adjacent list
                auto &it_element = m_iterators_value[list_id];
                while(it_element != (*it_list).end() && (*it_element) < (m_col+1)* m_block_size){
                    //2.1 Compute hash_value with 0s
                    auto length = (*it_element) - (prev_value+1);
                   // hash_value = (hash_value * m_h_length[length]) % m_prime;
                    uint128_t hc = (uint128_t) m_h_length[length] * hash_value;
                    hash_value = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);

                    //2.2 Compute hash_value with 1
                    ++m_number_ones;
                    m_x_point = (*it_element);
                    m_y_point = m_row * m_block_size + list_id;
                    //hash_value = (hash_value * m_asize + 1) % m_prime;
                    hc = (uint128_t) hash_value * m_x + 1;
                    hash_value = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                    //2.3 Next element of adjacent list
                    prev_value = (*it_element);
                    ++it_element;
                }
                //3. Check the last element and compute hash_value with 0s
                //prev_value is always smaller than block_size
                auto length = (m_col+1)*m_block_size - (prev_value+1);
                //hash_value = (hash_value * m_h_length[length]) % m_prime;
                uint128_t hc = (uint128_t) hash_value * m_h_length[length];
                hash_value = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                ++list_id;
            }
           // std::cout << "block row: " << row << " col: " << col << " hash: " << hash_value << std::endl;
            return hash_value;
        }

    public:

        std::vector<iterator_value_type > &iterators = m_iterators_value;
        const hash_type &hash = m_hash;
        const value_type &row = m_row;
        const value_type &col = m_col;
        const size_type &number_ones = m_number_ones;
        const value_type &x_point = m_x_point;
        const value_type &y_point = m_y_point;

        kr_block_faster() = default;

        kr_block_faster(size_type bs, size_type x, input_type &input){
            m_block_size = bs;
            m_x = x;
            m_iterators_value = std::vector<iterator_value_type>(m_block_size);
            init_iterators_first_row(input.begin());
            m_end_list = input.end();
            m_h_length = std::vector<hash_type >(m_block_size+1);
            m_h_length[0]=1;
            for(size_type i = 1; i <= m_block_size; ++i){
                uint128_t hc = (uint128_t) m_h_length[i-1] * m_x;
                m_h_length[i] = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
            }
        }

        //pre: list with size at least block_size
        /*void init(const iterator_list_type &iterator, const iterator_list_type &end_list, size_type total_blocks_in_row){
            m_total_blocks_in_row = total_blocks_in_row;
            m_end_list = end_list;
            init_iterators_first_row(iterator);
            //m_hash = compute_hash_block();
        }*/

        //! Copy constructor
        kr_block_faster(const kr_block_faster& o)
        {
            copy(o);
        }

        //! Move constructor
        kr_block_faster(kr_block_faster&& o)
        {
            *this = std::move(o);
        }


        kr_block_faster &operator=(const kr_block_faster &o) {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }
        kr_block_faster &operator=(kr_block_faster &&o) {
            if (this != &o) {

                m_x = std::move(o.m_x);
                m_block_size = std::move(o.m_block_size);
                m_hash = std::move(o.m_hash);
                m_row = std::move(o.m_row);
                m_col = std::move(o.m_col);

                m_iterator_list = std::move(o.m_iterator_list);
                m_end_list = std::move(o.m_end_list);
                m_iterators_value = std::move(o.m_iterators_value);
                m_h_length = std::move(o.m_h_length);

            }
            return *this;
        }

        void swap(kr_block_faster &o) {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_x, o.m_x);
            std::swap(m_block_size, o.m_block_size);
            std::swap(m_hash, o.m_hash);
            std::swap(m_row, o.m_row);
            std::swap(m_col, o.m_col);

            std::swap(m_iterator_list, o.m_iterator_list);
            std::swap(m_end_list, o.m_end_list);
            std::swap(m_iterators_value, o.m_iterators_value);
            std::swap(m_h_length, o.m_h_length);
        }


        bool next(){
            //std::cout << *((m_iterator_list+1)->end()) << std::endl;
            int state = compute_ith_block_in_row();
            while(state != 1){
                if(!init_iterators_next_row()){
                    return false;
                }
                state = compute_ith_block_in_row();
            }
            m_hash = compute_hash_block();
            return true;
        }

    };


}

#endif //INC_KARP_RABIN_BLOCK_ADJACENT_LIST_HPP
