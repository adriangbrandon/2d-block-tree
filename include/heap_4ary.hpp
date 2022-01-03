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
// Created by Adrián on 2/1/22.
//

#ifndef INC_UTIL_HEAP_4ARY_HPP
#define INC_UTIL_HEAP_4ARY_HPP

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <algorithm>
#include <iostream>

namespace util {

    template<class T, class local_t, class compare_t>
    class heap_4ary {

        typedef T value_type;
        typedef uint64_t size_type;

    private:

        size_type m_bytes = 0;
        size_type m_size = 0;
        value_type* m_array;
        local_t m_local;
        compare_t m_compare;

        void copy(heap_4ary &o){
            m_bytes = o.m_bytes;
            m_size = o.m_size;
            std::memcpy(m_array, o.m_array, o.m_bytes);
        }

        /*inline bool compare(value_type x, value_type y){
            if(*(x.first) == *(y.first)){
                return x.second > y.second;
            }
            return *(x.first) > *(y.first);
        }*/


        inline size_type get_local(size_type children, size_type pos){
            switch (children) {
                case 4: {
                    auto p1 = m_local(m_array, 4 * pos + 1, 4 * pos + 2);
                    auto p2 = m_local(m_array, 4 * pos + 3, 4 * pos + 4);
                    return m_local(m_array, p1, p2);
                }
                case 3: {
                    auto p1 = m_local(m_array, 4 * pos + 1, 4 * pos + 2);
                    return m_local(m_array, p1, 4 * pos + 3);
                }
                case 2:
                    return m_local(m_array, 4 * pos + 1, 4 * pos + 2);
                default:
                    return 4*pos+1;
            }
        }

        inline size_type get_local2(size_type children, size_type pos){
            auto max = 4*pos+1;
            auto index = max+1;
            while(index <= 4*pos+children){
                max = m_local(m_array, max, index);
                ++index;
            }
            return max;
        }


        void sift_down(size_type pos, size_type last){

            if(last < 1 || 4*pos >= last) return;  //Nothing to do

            size_type children = (4*pos+4 <= last) ? 4 : last - 4*pos;
            if(!children) return;
            size_type loc = get_local2(children, pos);
            if(m_compare(m_array[pos], m_array[loc])){
                return;
            }

            value_type val = std::move(m_array[pos]);
            do{
                m_array[pos] = std::move(m_array[loc]);
                pos = loc;
                if(4*pos >= last) break; //No children
                children = (4*pos+4 <= last) ? 4 : last - 4*pos;
                if(!children) break;
                loc = get_local2(children, pos);

            }while(!m_compare(val, m_array[loc]));
            m_array[pos] = std::move(val);

        }

        void sift_up(size_type pos){

            if(m_size > 1) {

                size_type parent = (pos-1) / 4;
                if(!m_compare(m_array[parent], m_array[pos])){
                    value_type val = std::move(m_array[pos]);
                    do {
                        m_array[pos] = std::move(m_array[parent]);
                        pos = parent;
                        if(pos == 0) break;
                        parent = (pos-1) / 4;

                    }while(!m_compare(m_array[parent], val));
                    m_array[pos] = std::move(val);
                }
            }
        }

        void make_heap(){
            if (m_size > 1){
                for (int32_t i = (m_size - 1) / 4; i >= 0; --i){
                    sift_down(i, m_size-1);
                }
            }
        }

        inline void pop_heap(){
            if (m_size > 1){
                std::swap(m_array[0], m_array[m_size-1]);
                sift_down(0, m_size - 2);
            }
        }

        inline void push_heap(){
            sift_up(m_size-1);
        }

    public:

        heap_4ary() = default;

       /* template<class Container>
        explicit heap_4ary(const Container &values){
            m_size = values.size();
            m_bytes = (m_size * sizeof(T) + 63) / 64 * 64;
            m_array = aligned_alloc(64, m_bytes);
            make_heap(0, m_size-1);
        }*/

        explicit heap_4ary(const size_type max_size){
            m_size = 0;
            m_bytes = (max_size * sizeof(value_type) + 63) / 64 * 64;
            m_array = (value_type*) aligned_alloc(64, m_bytes);
        }

        explicit heap_4ary(const size_type max_size, const std::vector<value_type> &c){
            m_size = c.size();
            m_bytes = (max_size * sizeof(value_type) + 63) / 64 * 64;
            m_array = (value_type*) aligned_alloc(64, m_bytes);
            std::memcpy(m_array, &c[0], sizeof(value_type) * c.size());
            make_heap();
        }

        inline value_type top(){
            return m_array[0];
        }

        inline void pop(){
            pop_heap();
            --m_size;
        }

        inline void push(const value_type &v){
            m_array[m_size] = v;
            ++m_size;
            push_heap();
        }

        inline void update_top(const value_type &v){
            m_array[0] = v;
            sift_down(0, m_size-1);
        }

        inline void clear(){
            m_size = 0;
            std::free(m_array);
        }

        inline bool empty(){
            return m_size == 0;
        }

        inline size_type size(){
            return m_size;
        }

        //! Copy constructor
        heap_4ary(const heap_4ary& o)
        {
            copy(o);
        }

        //! Move constructor
        heap_4ary(heap_4ary&& o)
        {
            *this = std::move(o);
        }


        heap_4ary &operator=(const heap_4ary &o) {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }

        heap_4ary &operator=(heap_4ary &&o) {
            if (this != &o) {
                m_bytes = std::move(o.m_bytes);
                m_size = std::move(o.m_size);
                m_array = std::move(o.m_array);
            }
            return *this;
        }

        void swap(heap_4ary &o) {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_bytes, o.m_bytes);
            std::swap(m_size, o.m_size);
            std::swap(m_array, o.m_array);
        }

        void print(){
            std::cout << "size: " << m_size << std::endl;
            std::cout << "{ ";
            /*for(auto i = 0; i < m_size ; ++i){
                std::cout << m_array[i] << ", ";
            }*/
            std::cout << "}" << std::endl;
        }

    };
}

#endif //INC_2D_BLOCK_TREE_HEPA_4ARY_HPP
