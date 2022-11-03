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

#ifndef INC_KARP_RABIN_ROLL_FASTER_V2_HPP
#define INC_KARP_RABIN_ROLL_FASTER_V2_HPP

#include <cstdint>
#include <vector>
#include <queue>
#include <heap_v2.hpp>
#include <iostream>
#include <bithacks.hpp>
#include <string>
#include "adjacency_list_helper.hpp"

namespace karp_rabin {

    template <class t_input = std::vector<std::vector<int64_t>>>
    class kr_roll_faster {


    public:
        typedef __uint128_t uint128_t;
        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef uint64_t hash_type;
        typedef t_input  input_type;
        typedef typename input_type::iterator iterator_list_type;
        typedef typename input_type::value_type::iterator iterator_value_type;
        typedef std::pair<iterator_value_type, size_type > pq_element_type;
        struct compare_element {
            inline bool operator()(const pq_element_type &x, const pq_element_type &y){
                auto a = *(x.first);
                auto b = *(y.first);
                if(a == b){
                    return x.second <= y.second;
                }
                return a <= b;
            }
        };

        struct local_element {
            inline size_type operator()(const pq_element_type* c, const size_type x, const size_type y){
                auto a = *(c[x].first);
                auto b = *(c[y].first);
                if(a == b){
                    return (c[x].second <= c[y].second) ? x : y;
                }
                return (a <= b) ? x : y;
            }
        };

        class compare_pq{
        public:
            bool operator()(const pq_element_type &x, const pq_element_type &y){
                if(*(x.first) == *(y.first)){
                    return x.second > y.second;
                }
                return *(x.first) > *(y.first);
            }
        };

        typedef util::heap_v2<pq_element_type, std::vector<pq_element_type>> heap_type;


    private:

        static constexpr size_type m_prime_pow = 61;
        static constexpr size_type m_prime = (1ull << 61) - 1;
        size_type m_x;

        size_type m_block_size;
        size_type m_total_shifts;

        hash_type m_hash;
        value_type m_col = 0;
        value_type m_row = -1;
        size_type m_number_ones = 0;
        size_type shifts = 0;
        size_type next_row = 0;


        iterator_list_type m_iterator_list;
        iterator_list_type m_end_list;
        std::vector<iterator_value_type > m_in_iterators;
        std::vector<iterator_value_type > m_out_iterators;

        std::vector<hash_type> m_h_in_right;
        std::vector<hash_type> m_h_out_right;
        std::vector<hash_type> m_h_length;
        heap_type m_heap_in;
        heap_type m_heap_out;
        //heap_4ary_type m_heap_in;
        //heap_4ary_type m_heap_out;

        inline iterator_value_type get_it_in(const size_type list_id){
            auto it = (m_iterator_list + list_id)->begin();
            while(it != (m_iterator_list + list_id)->end() && (std::abs(*it) < m_block_size)){
                ++it;
            }
            while(it != (m_iterator_list + list_id)->end() && (*it < 0)){
                ++it;
            }
            return it;
        }

        inline iterator_value_type get_it_out(const size_type list_id){
            auto it = (m_iterator_list + list_id)->begin();
            while(it != (m_iterator_list + list_id)->end() && (*it < 0)){
                ++it;
            }
            return it;
        }

        inline void next_value(iterator_value_type &it, const size_type list_id){
            ++it;
            while(it != (m_iterator_list + list_id)->end() && (*it < 0)){
                ++it;
            }
        }

        inline size_type cyclic_pos(const size_type list_id){
            return list_id % m_block_size;
        }

        void init_hs(){
            m_h_in_right[m_block_size-1] = 1;
            size_type first_out = 1;
            uint128_t hc;
            for(auto i = 1; i < m_block_size; ++i){
                hc = (uint128_t) first_out * m_x;
                first_out = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
            }
            m_h_out_right[m_block_size-1] = first_out;

            for(int64_t i = m_block_size-1; i > 0; --i){
                hc = (uint128_t) m_h_out_right[i] * m_x;
                m_h_in_right[i-1] = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                //m_h_in_right[i-1] = (m_h_out_right[i] * m_asize) % m_prime;
                m_h_out_right[i-1] = m_h_in_right[i-1];
                for(auto j = 1; j < m_block_size; ++j){
                    hc = (uint128_t) m_h_out_right[i-1] * m_x;
                    m_h_out_right[i-1] = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                    //m_h_out_right[i-1] = (m_h_out_right[i-1] * m_asize) % m_prime;
                }
            }
        }

        void init_heaps(){

            std::vector<pq_element_type> v_in, v_out;
            for(size_type i = 0; i < m_block_size; ++i){
                auto cyclic_i = cyclic_pos(m_row+i);
                if(m_out_iterators[cyclic_i] != m_iterator_list[m_row+i].end()){
                    v_out.emplace_back(m_out_iterators[cyclic_i], i);
                }
                if(m_in_iterators[cyclic_i] != m_iterator_list[m_row+i].end()){
                    v_in.emplace_back(m_in_iterators[cyclic_i], i);
                }
            }
            m_heap_in = heap_type(v_in, m_block_size);
            m_heap_out = heap_type(v_out, m_block_size);
        }

        /*void init_heaps(){
         std::vector<pq_element_type> v_in, v_out;
         for(size_type i = 0; i < m_block_size; ++i){
             auto it_out = m_iterator_list[m_row+i].begin();
             while(it_out != m_iterator_list[m_row+i].end() && *it_out < 0){
                 ++it_out;
             }
             if(it_out != m_iterator_list[m_row+i].end()){
                 v_out.emplace_back(it_out, i);
             }


             auto cyclic_i = (m_row + i) % m_block_size;
             auto it_in = m_iterators_end_first_block[cyclic_i];
             while(it_in != m_iterator_list[m_row+i].end() && *it_in < 0){
                 ++it_in;
             }
             if(it_in != m_iterator_list[m_row+i].end()){
                 v_in.emplace_back(it_in,  i);
             }
         }
         m_heap_in = heap_type(v_in, m_block_size);
         m_heap_out = heap_type(v_out, m_block_size);
     }*/

        //TODO: adrian cambiar nombre a compute_block
        int compute_block(){

            ++m_row;
            m_col = 0;
            //1. Prepare iterators
            for(size_type i = 0; i < m_block_size; ++i){
                auto it_in = get_it_in(row + i);
                auto it_out = get_it_out(row + i);
                auto cyclic_i = cyclic_pos(row+i);
                m_in_iterators[cyclic_i] = it_in;
                m_out_iterators[cyclic_i] = it_out;
            }

            //2. Compute hash
            uint128_t hc;
            size_type hash_value = 0;
            for(size_type i = 0; i < m_block_size; ++i){
                auto cyclic_i = cyclic_pos(row+i);
                int64_t prev_value = -1;
                auto it = m_out_iterators[cyclic_i];
                while(it != (m_iterator_list+row+i)->end() && std::abs(*it) < m_block_size){
                    if((*it) >= 0){
                        //1.2.1 Compute hash_value with 0s
                        auto length = (*it) - (prev_value+1);
                        //hash_row = (hash_row * m_h_length[length]) % m_prime;
                        hc = (uint128_t) m_h_length[length] * hash_value;
                        hash_value = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                        //1.2.2 Compute hash_value with 1
                        ++m_number_ones;
                        //hash_row = (hash_row * m_asize + 1) % m_prime;
                        hc = (uint128_t) hash_value * m_x + 1;
                        hash_value = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                        //1.2.3 Next element of adjacent list
                        prev_value = (*it);
                    }
                    ++it;
                }
                //1.3. Check the last element and compute hash_value with 0s
                //prev_value is always smaller than block_size
                auto length = m_block_size - (prev_value+1);
                //hash_row = (hash_row * m_h_length[length]) % m_prime;
                hc = (uint128_t) m_h_length[length] * hash_value;
                hash_value = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
            }

            m_hash = hash_value;
            //std::cout << "shift row: " << m_row << " col: " << m_col << std::endl;
            //2. Initialize heaps
            init_heaps();
            return (m_number_ones >= 2);
        }


        /**
         * @return 0 = empty, 1 = ok, 2 = next row
         */
        int state_shift_right(){

            uint128_t hc;
            ++m_col;

            //No more ones to process, so we advance to the next row
            if(m_heap_out.empty() || m_col >= m_total_shifts){
                return 2;
            }
            //Skip
            if(!m_heap_in.empty() && m_number_ones < 2){
                if(m_number_ones == 1){
                    auto length = *(m_heap_in.top().first) - *(m_heap_out.top().first);
                    if(length < m_block_size){ //sliding window overlaps both ones
                        auto shift = *(m_heap_in.top().first) - (m_col + m_block_size-1);
                        hc = (uint128_t) m_hash * m_h_length[shift];
                        m_hash = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                        //m_hash = (m_hash * m_h_length[shift]) % m_prime; //previous hash
                    }else{
                        m_hash = 0; //previous hash
                        m_number_ones = 0; //previous number ones
                        auto out_top = m_heap_out.top();
                        next_value(out_top.first, m_row + out_top.second);  //Skip deleted elements
                        m_out_iterators[cyclic_pos(m_row + out_top.second)] = out_top.first;
                        if(out_top.first != (m_iterator_list + m_row + out_top.second)->end()){
                            m_heap_out.update_top({out_top.first, out_top.second});
                            //m_heap_out.update_top(out_top);
                        }else{
                            m_heap_out.pop();
                        }
                    }
                }
                m_col = *(m_heap_in.top().first) - m_block_size + 1;
            }

            auto new_hash = m_hash;
            //Delete previous part
            while(!m_heap_out.empty() && *(m_heap_out.top().first) == m_col-1){
                auto out_top = m_heap_out.top();
                hc = (uint128_t) new_hash - m_h_out_right[out_top.second];
                new_hash = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                //new_hash += (m_prime - m_h_out_right[out_top.second]);
                //new_hash = new_hash % m_prime;
                //m_heap_out.pop();
                --m_number_ones;
                next_value(out_top.first, m_row + out_top.second);  //Skip deleted elements
                m_out_iterators[cyclic_pos(m_row + out_top.second)] = out_top.first;
                if(out_top.first != (m_iterator_list + m_row + out_top.second)->end()){
                    m_heap_out.update_top({out_top.first, out_top.second});
                    //m_heap_out.update_top(out_top);
                }else{
                    m_heap_out.pop();
                }
            }
            //new_hash = (new_hash * m_asize) % m_prime;
            hc = (uint128_t) new_hash * m_x;
            new_hash = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
            //Add new info
            while(!m_heap_in.empty() && *(m_heap_in.top().first) == m_col+m_block_size-1){
                auto in_top = m_heap_in.top();
                hc = (uint128_t) new_hash + m_h_in_right[in_top.second];
                new_hash = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
                //new_hash = (new_hash + m_h_in_right[in_top.second]) % m_prime;
                ++m_number_ones;
                //m_heap_in.pop();
                next_value(in_top.first, m_row + in_top.second);  //Skip deleted elements
                m_in_iterators[cyclic_pos(m_row + in_top.second)] = in_top.first;
                if(in_top.first != (m_iterator_list + m_row + in_top.second)->end()){
                    //m_heap_in.update_top(in_top);
                    m_heap_in.update_top({in_top.first, in_top.second});
                }else{
                    m_heap_in.pop();
                }
            }
            m_hash = new_hash;
            ++shifts;
            return (m_number_ones >= 2);
        }


        void copy(const kr_roll_faster &o){
            m_x = o.m_x;
            m_block_size = o.m_block_size;
            m_prime = o.m_prime;
            m_total_shifts = o.m_total_shifts;

            m_hash = o.m_hash;
            m_col = o.m_col;
            m_row = o.m_row;
            m_iterator_list = o.m_iterator_list;
            m_end_list = o.m_end_list;
            m_in_iterators = o.m_in_iterators;
            m_out_iterators = o.m_out_iterators;

            m_h_in_right = o.m_h_in_right;
            m_h_out_right= o.m_h_out_right;
            m_h_length = o.m_h_length;
            m_heap_in = o.m_heap_in;
            m_heap_out = o.m_heap_out;

            m_number_ones = o.m_number_ones;

        }

    public:

        const hash_type &hash = m_hash;
        const value_type &row = m_row;
        const value_type &col = m_col;
        std::vector<iterator_value_type > &in_iterators = m_in_iterators;
        std::vector<iterator_value_type > &out_iterators = m_out_iterators;
        heap_type &heap_in = m_heap_in;
        heap_type &heap_out = m_heap_out;

        kr_roll_faster(size_type bs, size_type x, input_type &input){
            m_block_size = bs;
            m_x = x;
            m_total_shifts = std::distance(input.begin(), input.end()) - m_block_size + 1;
            m_in_iterators = std::vector<iterator_value_type>(m_block_size);
            m_out_iterators = std::vector<iterator_value_type>(m_block_size);
            m_h_in_right = std::vector<hash_type>(m_block_size);
            m_h_out_right = std::vector<hash_type>(m_block_size);
            m_end_list = input.end();
            m_iterator_list = input.begin();
            init_hs();
            m_h_length = std::vector<hash_type >(m_block_size+1);
            m_h_length[0]=1;
            uint128_t hc;
            for(size_type i = 1; i <= m_block_size; ++i){
                hc = (uint128_t) m_h_length[i-1] * m_x;
                m_h_length[i] = util::bithacks::mersenne_mod(hc, m_prime, m_prime_pow);
            }
        }

        //! Copy constructor
        kr_roll_faster(const kr_roll_faster& o)
        {
            copy(o);
        }

        //! Move constructor
        kr_roll_faster(kr_roll_faster&& o)
        {
            *this = std::move(o);
        }


        kr_roll_faster &operator=(const kr_roll_faster &o) {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }

        kr_roll_faster &operator=(kr_roll_faster &&o) {
            if (this != &o) {
                m_block_size = std::move(o.m_block_size);
                m_x = std::move(o.m_x);
                m_total_shifts = std::move(o.m_total_shifts);

                m_hash = std::move(o.m_hash);
                m_col = std::move(o.m_col);
                m_row = std::move(o.m_row);

                m_iterator_list = std::move(o.m_iterator_list);
                m_end_list = std::move(o.m_end_list);
                m_in_iterators = std::move(o.m_in_iterators);
                m_out_iterators = std::move(o.m_out_iterators);

                m_h_in_right = std::move(o.m_h_in_right);
                m_h_out_right= std::move(o.m_h_out_right);
                m_h_length = std::move(o.m_h_length);
                m_heap_in = std::move(o.m_heap_in);
                m_heap_out = std::move(o.m_heap_out);

                m_number_ones = std::move(o.m_number_ones);
            }
            return *this;
        }

        void swap(kr_roll_faster &o) {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_block_size, o.m_block_size);
            std::swap(m_x, o.m_x);
            std::swap(m_total_shifts, o.m_total_shifts);

            std::swap(m_hash, o.m_hash);
            std::swap(m_col, o.m_col);
            std::swap(m_row, o.m_row);

            std::swap(m_iterator_list, o.m_iterator_list);
            std::swap(m_end_list, o.m_end_list);
            std::swap(m_in_iterators, o.m_in_iterators);
            std::swap(m_out_iterators, o.m_out_iterators);

            std::swap(m_h_in_right, o.m_h_in_right);
            std::swap(m_h_out_right, o.m_h_out_right);
            std::swap(m_h_length, o.m_h_length);
            std::swap(m_heap_in, o.m_heap_in);
            std::swap(m_heap_out, o.m_heap_out);

            std::swap(m_number_ones, o.m_number_ones);
        }

        bool next(){
            //std::cout << "calling next" << std::endl;
            /*if(m_row == 11807 && m_col >= 9347) {
                std::cout << "Row: " << m_row << " col: " << m_col << std::endl;
                m_heap_out.printa();
            }*/
            /*std::cout << "Row: " << m_row << " Col: " << m_col << std::endl;
            std::cout << "Out" << std::endl;
            m_heap_out.printa();
            std::cout << "In" << std::endl;
            m_heap_in.printa();
            std::cout << std::endl;*/
            // if(!m_heap_in.check()) exit(1);
            // if(!m_heap_out.check()) exit(2);
            int state;
            if(m_row == -1){
                state = compute_block();
                //if(m_block_size == 4) std::cout << "initial_block: " << m_hash << "n_ones: " << m_number_ones << std::endl;
            }else {
                state = state_shift_right();
                //if(m_block_size == 4) std::cout << "shift_right: " << m_hash << "n_ones: " << m_number_ones << std::endl;
            }
            while(state != 1){
                if(state == 0){
                    state = state_shift_right();
                    //if(m_block_size == 4) std::cout << "shift_right2: " << m_hash << "n_ones: " << m_number_ones << std::endl;
                }else{
                    if(m_iterator_list + m_row + m_block_size == m_end_list) return false;
                    state = compute_block();
                    //if(m_block_size == 64) std::cout << "next_row: " << m_hash << "n_ones: " << m_number_ones << std::endl;
                    //std::cout << "b2: " << m_number_ones << std::endl;
                }
            }
            return true;
        }

        /*void update_prev_hash_same_row(){
            m_prev_hash = 0;
            for(auto cyclic_i = 0; cyclic_i < m_prev_kr.size(); ++cyclic_i){
                m_prev_kr[cyclic_i] = 0;
            }
        }*/



        void redo_heap(heap_type &heap, std::vector<iterator_value_type> &iterators) {
            std::vector<pq_element_type> v;
            for (size_type i = 0; i < m_block_size; ++i) {
                auto cyclic_i = cyclic_pos(m_row + i);
                auto &it = iterators[cyclic_i];
                if (it != (m_iterator_list + m_row + i)->end() && *it < 0) {
                    next_value(it, m_row + i);
                }
                if (it != (m_iterator_list + m_row + i)->end()) {
                    v.emplace_back(it, i);
                }

            }
            heap.clear();
            heap = heap_type(v, m_block_size);
        }

        void redo_heap_in(){
            redo_heap(m_heap_in, m_in_iterators);
        }


        void redo_heap_out(){
            redo_heap(m_heap_out, m_out_iterators);
        }


        void print(){
            std::cout << "shifts: " << shifts << " next_row: " << next_row << std::endl;
        }

    };


}

#endif //INC_KARP_RABIN_ROLL_ADJACENT_LIST_HPP