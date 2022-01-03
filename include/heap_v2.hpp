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
// Created by Adrián on 25/07/2019.
//

#ifndef UTIL_HEAP_HPP
#define UTIL_HEAP_HPP

#include <vector>
#include <iostream>
#include <algorithm>

namespace util {

    template <class _Tp, class _Container = std::vector<_Tp>>
    class heap_v2 {

    public:
        typedef _Container                               container_type;
        typedef typename _Container::iterator            iterator_type;
        typedef typename _Container::difference_type     difference_type;
        typedef typename _Container::value_type          value_type;
    private:
        container_type m_values;

        void copy(const heap_v2 &p){
            m_values = p.m_values;
        }

        inline bool compare(value_type x, value_type y){
            if(*(x.first) == *(y.first)){
                return x.second > y.second;
            }
            return *(x.first) > *(y.first);
        }

        void sift_down(iterator_type __first, iterator_type /*__last*/,
                    difference_type __len,
                    iterator_type __start)
        {
            // left-child of __start is at 2 * __start + 1
            // right-child of __start is at 2 * __start + 2
            difference_type __child = __start - __first;

            if (__len < 2 || (__len - 2) / 2 < __child)
                return;

            __child = 2 * __child + 1;
            iterator_type __child_i = __first + __child;

            //if ((__child + 1) < __len && compare(*__child_i, *(__child_i + 1))) {
            if ((__child + 1) < __len &&
            (*(__child_i->first) > *((__child_i+1)->first)
            || ( *(__child_i->first) == *((__child_i+1)->first) && (__child_i->second) > ((__child_i+1)->second)))) {
                // right-child exists and is greater than left-child
                ++__child_i;
                ++__child;
            }

            // check if we are in heap-order
            if ((*(__child_i->first) > *(__start->first)
                 || ( *(__child_i->first) == *(__start->first) && (__child_i->second) > (__start->second))))
                // we are, __start is larger than it's largest child
                return;

            value_type __top(std::move(*__start));
            do
            {
                // we are not in heap-order, swap the parent with its largest child
                *__start = std::move(*__child_i);
                __start = __child_i;

                if ((__len - 2) / 2 < __child)
                    break;

                // recompute the child based off of the updated parent
                __child = 2 * __child + 1;
                __child_i = __first + __child;

                if ((__child + 1) < __len && compare(*__child_i, *(__child_i + 1))) {
                    // right-child exists and is greater than left-child
                    ++__child_i;
                    ++__child;
                }

                // check if we are in heap-order
            } while (!(*(__child_i->first) > *(__top.first)
                       || ( *(__child_i->first) == *(__top.first) && (__child_i->second) > (__top.second))));
            *__start = std::move(__top);
        }


        void make_heap(iterator_type __first, iterator_type __last)
        {
            difference_type __n = __last - __first;
            if (__n > 1)
            {
                // start from the first parent, there is no need to consider children
                for (difference_type __start = (__n - 2) / 2; __start >= 0; --__start)
                {
                    sift_down(__first, __last, __n, __first + __start);
                }
            }
        }


        inline void pop_heap(iterator_type __first, iterator_type __last)
        {
            difference_type __len = __last - __first;
            if (__len > 1)
            {
                std::swap(*__first, *--__last);
                sift_down(__first, __last, __len - 1, __first);
            }
        }


        void sift_up(iterator_type __first, iterator_type __last,
                  difference_type __len)
        {
            if (__len > 1)
            {
                __len = (__len - 2) / 2;
                iterator_type __ptr = __first + __len;
                if (__comp(*__ptr, *--__last))
                {
                    value_type __t(std::move(*__last));
                    do
                    {
                        *__last = std::move(*__ptr);
                        __last = __ptr;
                        if (__len == 0)
                            break;
                        __len = (__len - 1) / 2;
                        __ptr = __first + __len;
                    } while (compare(*__ptr, __t));
                    *__last = std::move(__t);
                }
            }
        }

        inline void push_heap(iterator_type __first, iterator_type __last){
            sift_up(__first, __last, __last - __first);
        }



    public:


        heap_v2() = default;

        explicit heap_v2(const container_type &values){
            m_values = values;
            make_heap(m_values.begin(), m_values.end());
        }

        inline _Tp top(){
            return m_values.front();
        }

        inline void pop(){
            pop_heap(m_values.begin(), m_values.end());
            m_values.pop_back();
        }

        inline void push(const _Tp &v){
            m_values.push_back(v);
            push_heap(m_values.begin(), m_values.end());
        }

        inline void update_top(const _Tp &v){
            m_values[0]=v;
            sift_down(m_values.begin(), m_values.end(), m_values.size(), m_values.begin());
        }

        inline void clear(){
            m_values.clear();
        }

        inline bool empty(){
            return m_values.empty();
        }

        inline uint64_t size(){
            return m_values.size();
        }

        inline void re_heap(){
            make_heap(m_values.begin(), m_values.end());
        }

        inline bool is_heap(){
            return std::is_heap(m_values.begin(), m_values.end());
        }

        //! Copy constructor
        heap_v2(const heap_v2& o)
        {
            copy(o);
        }

        //! Move constructor
        heap_v2(heap_v2&& o)
        {
            *this = std::move(o);
        }


        heap_v2 &operator=(const heap_v2 &o) {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }

        heap_v2 &operator=(heap_v2 &&o) {
            if (this != &o) {
                m_values = std::move(o.m_values);
            }
            return *this;
        }

        void swap(heap_v2 &o) {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_values, o.m_values);
        }

    };

}

#endif
