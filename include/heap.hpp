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

namespace util {

    template <class _Tp, class _Container = std::vector<_Tp>,
            class _Compare = std::less<typename _Container::value_type> >
    class heap {

    public:
        typedef _Container                               container_type;
        typedef _Compare                                 value_compare;
    private:
        container_type m_values;
        value_compare  m_comp;

        void copy(const heap &p){
            m_values = p.m_values;
            m_comp = p.m_comp;
        }


    public:


        heap() = default;

        explicit heap(const container_type &values){
            m_values = values;
            std::make_heap(m_values.begin(), m_values.end(), m_comp);
        }

        _Tp top(){
            return m_values.front();
        }

        void pop(){
            std::pop_heap(m_values.begin(), m_values.end(), m_comp);
            m_values.pop_back();
        }

        void push(const _Tp &v){
            m_values.push_back(v);
            std::push_heap(m_values.begin(), m_values.end(), m_comp);
        }

        void update_top(const _Tp &v){
            m_values[0]=v;
            std::__sift_down(m_values.begin(), m_values.end(), m_comp, m_values.size(), m_values.begin());
        }

        bool empty(){
            return m_values.empty();
        }

        //! Copy constructor
        heap(const heap& o)
        {
            copy(o);
        }

        //! Move constructor
        heap(heap&& o)
        {
            *this = std::move(o);
        }


        heap &operator=(const heap &o) {
            if (this != &o) {
                copy(o);
            }
            return *this;
        }

        heap &operator=(heap &&o) {
            if (this != &o) {
                m_values = std::move(o.m_values);
                m_comp = std::move(o.m_comp);
            }
            return *this;
        }

        void swap(heap &o) {
            // m_bp.swap(bp_support.m_bp); use set_vector to set the supported bit_vector
            std::swap(m_values, o.m_values);
            std::swap(m_comp, o.m_comp);
        }

    };

}

#endif
