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

#ifndef INC_KARP_RABIN_ROLL_ADJACENT_LIST_V2_HPP
#define INC_KARP_RABIN_ROLL_ADJACENT_LIST_V2_HPP

#include <cstdint>
#include <vector>
#include <queue>

namespace karp_rabin {

    template <class t_input = std::vector<std::vector<uint64_t>>>
    class kr_roll_adjacent_list_v2 {

    private:


    public:
        typedef uint64_t size_type;
        typedef uint64_t hash_type;
        typedef t_input  input_type;
        typedef typename input_type::iterator iterator_list_type;
        typedef typename input_type::value_type::iterator iterator_value_type;
        typedef std::pair<iterator_value_type, size_type > pq_element_type;
        class compare_pq{
        public:
            bool operator()(const pq_element_type &x, const pq_element_type &y){
                if(*(x.first) == *(y.first)){
                    return x.second > y.second;
                }
                return *(x.first) > *(y.first);
            }
        };

        typedef std::priority_queue<pq_element_type, std::vector<pq_element_type>, compare_pq> heap_type;


    private:

        size_type m_block_size;
        size_type m_prime;
        size_type m_asize = 2;
        size_type m_total_shifts;

        hash_type m_hash;
        size_type m_col = 0;
        size_type m_row = -1;

        iterator_list_type m_iterator_list;
        iterator_list_type m_end_list;
        std::vector<iterator_value_type> m_iterators_end_first_block;
        std::vector<iterator_value_type > m_iterators;

        std::vector<hash_type> m_h_in_right;
        std::vector<hash_type> m_h_out_right;
        heap_type m_heap_in;
        heap_type m_heap_out;

        hash_type m_prev_hash;
        std::vector<hash_type> m_prev_kr;


        /*void init_iterators_first_row(const iterator_list_type &iterator){
            m_iterator_list = iterator;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_value[list_id] = (*(m_iterator_list+list_id)).begin();
            }
        }*/



        void init_iterators(const iterator_list_type &iterator){
            m_iterator_list = iterator;
            for(auto list_id = 0; list_id < m_block_size; ++list_id){
                m_iterators_end_first_block[list_id] = (m_iterator_list+list_id)->begin();
            }

        }

        void init_hs(){
            m_h_in_right[m_block_size-1] = 1;
            size_type first_out = 1;
            for(auto i = 1; i < m_block_size; ++i){
                first_out = (first_out * m_asize) % m_prime;
            }
            m_h_out_right[m_block_size-1] = first_out;

            for(int64_t i = m_block_size-1; i > 0; --i){
                m_h_in_right[i-1] = (m_h_out_right[i] * m_asize) % m_prime;
                m_h_out_right[i-1] = m_h_in_right[i-1];
                for(auto j = 1; j < m_block_size; ++j){
                    m_h_out_right[i-1] = (m_h_out_right[i-1] * m_asize) % m_prime;
                }
            }
        }

        void init_heaps(){

            std::vector<pq_element_type> v_in, v_out;
            for(size_type i = 0; i < m_block_size; ++i){
                if(m_iterator_list[m_row+i].begin() != m_iterator_list[m_row+i].end()){
                    v_out.emplace_back(m_iterator_list[m_row+i].begin(), i);
                }
                auto cyclic_i = (m_row + i) % m_block_size;
                if(m_iterators_end_first_block[cyclic_i] != m_iterator_list[m_row+i].end()){
                    v_in.emplace_back(m_iterators_end_first_block[cyclic_i],  i);
                }
            }
            m_heap_in = heap_type(v_in.begin(), v_in.end());
            m_heap_out = heap_type(v_out.begin(), v_out.end());

        }


        bool compute_initial_hash_block(){

            m_row = 0;
            size_type list_id = 0;
            hash_type hash_value = 0;
            //1. Iterate over adjacency lists
            for(auto it_list = m_iterator_list; it_list != m_iterator_list + m_block_size; ++it_list){
                int64_t prev_value = -1;
                //1.2. Iterate over the elements of an adjacent list
                auto &it_element = m_iterators_end_first_block[list_id];
                size_type hash_row = 0;
                while(it_element != it_list->end() && (*it_element) < m_block_size){
                    //2.1 Compute hash_value with 0s
                    for(auto v = prev_value+1; v < (*it_element); ++v){
                        hash_row = (hash_row * m_asize) % m_prime;
                    }
                    //1.2.2 Compute hash_value with 1
                    hash_row = (hash_row * m_asize + 1) % m_prime;
                    //1.2.3 Next element of adjacent list
                    prev_value = (*it_element);
                    ++it_element;
                }
                //1.3. Check the last element and compute hash_value with 0s
                //prev_value is always smaller than block_size
                for(auto i = prev_value + 1; i < m_block_size; ++i){
                    hash_row = (hash_row * m_asize) % m_prime;
                }
                //1.4 Init the hash value per row and compute the hash of the submatrix
                m_prev_kr[list_id] = hash_row;
                hash_value += (hash_row * m_h_in_right[list_id]) % m_prime;
                ++list_id;
            }
            m_hash = hash_value;
            m_prev_hash = m_hash;
            //Initial iterators at the end of first block
            m_iterators = m_iterators_end_first_block;

            //2. Initialize heaps
            init_heaps();
            return true;
        }



        bool shift_right(){
            ++m_col;
            //No more ones to process, so we advance to the next row
            if(m_heap_out.empty() || m_col == m_total_shifts){
                return next_row();
            }
            //Skip
            if(!m_heap_in.empty() && *(m_heap_out.top().first) == *(m_heap_in.top().first)){
                m_col = *(m_heap_out.top().first) - m_block_size + 1;
            }
            auto hash = m_hash;
            //Delete previous part
            while(!m_heap_out.empty() && *(m_heap_out.top().first) == m_col-1){
                auto out_top = m_heap_out.top();
                hash += (m_prime - m_h_out_right[out_top.second]) % m_prime;
                m_heap_out.pop();
                ++out_top.first;
                if(out_top.first != (m_iterator_list + m_row + out_top.second)->end()){
                    m_heap_out.push({out_top.first, out_top.second});
                }
            }
            hash = (hash * m_asize) % m_prime;
            //Add new info
            while(!m_heap_in.empty() && *(m_heap_in.top().first) == m_col+m_block_size-1){
                auto in_top = m_heap_in.top();
                hash = (hash + m_h_in_right[in_top.second]) % m_prime;
                m_heap_in.pop();
                m_iterators[in_top.second] = ++in_top.first;
                if(in_top.first != (m_iterator_list + m_row + in_top.second)->end()){
                    m_heap_in.push({in_top.first, in_top.second});
                }
            }
            m_hash = hash;

            return true;
        }

        bool next_row(){
            if(m_iterator_list + m_row + m_block_size == m_end_list) return false;
            ++m_row;
            m_col = 0;
            auto hash = m_prev_hash;
            //Delete previous part
            auto cyclic_i = (m_row-1) % m_block_size;
            auto first_row = (m_prev_kr[cyclic_i] * m_h_in_right[0]) % m_prime;
            hash += (m_prime - first_row) % m_prime;
            hash = (hash * m_h_in_right[m_block_size-2]) % m_prime;
            // Compute hash of last row
            hash_type hash_row = 0;
            size_type last_row = m_row + m_block_size-1;
            auto prev_value = (size_type) -1;
            auto it_element = (m_iterator_list + last_row)->begin();
            while(it_element != (m_iterator_list + last_row)->end() && (*it_element) < m_block_size){
                // Compute hash_value with 0s
                for(auto v = prev_value+1; v < (*it_element); ++v){
                    hash_row = (hash_row * m_asize) % m_prime;
                }
                // Compute hash_value with 1
                hash_row = (hash_row * m_asize + 1) % m_prime;
                // Next element of adjacent list
                prev_value = (*it_element);
                ++it_element;
            }
            // Check the last element and compute hash_value with 0s
            //prev_value is always smaller than block_size
            for(auto i = prev_value + 1; i < m_block_size; ++i){
                hash_row = (hash_row * m_asize) % m_prime;
            }
            //1.4 Init the hash value per row and compute the hash of the submatrix
            hash = (hash + hash_row) % m_prime;
            m_hash = hash;
            m_prev_hash = m_hash;

            //1.5 Store prev hash and position of next one
            m_prev_kr[cyclic_i] = hash_row;
            m_iterators_end_first_block[cyclic_i] = it_element;
            m_iterators = m_iterators_end_first_block;

            //1.6 Init heaps
            init_heaps();
            //1.7 Check if the first area is empty
            if(m_hash == 0 && (m_heap_in.empty() || *(m_heap_in.top().first) >= m_block_size)){
                return shift_right();
            }
            return true;

        }

    public:

        const hash_type &hash = m_hash;
        const size_type &row = m_row;
        const size_type &col = m_col;
        const std::vector<iterator_value_type > &iterators = m_iterators;

        kr_roll_adjacent_list_v2(size_type bs, size_type q, input_type &input){
            m_block_size = bs;
            m_prime = q;
            m_total_shifts = std::distance(input.begin(), input.end()) - m_block_size + 1;
            m_iterators_end_first_block = std::vector<iterator_value_type>(m_block_size);
            m_iterators = std::vector<iterator_value_type>(m_block_size);
            m_h_in_right = std::vector<hash_type>(m_block_size);
            m_h_out_right = std::vector<hash_type>(m_block_size);
            m_prev_kr = std::vector<hash_type>(m_block_size);
            m_end_list = input.end();
            init_iterators(input.begin());
            init_hs();
        }



        bool next(){
            if(m_row == -1){
                return compute_initial_hash_block();
            }else{
                return shift_right();
            }
        }


    };


}

#endif //INC_KARP_RABIN_ROLL_ADJACENT_LIST_HPP
