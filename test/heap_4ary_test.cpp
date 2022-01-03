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

#include <heap_4ary.hpp>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {


    struct compare_min {
        bool operator()(uint64_t x, uint64_t y){
            return x < y;
        }
    };

    struct local_min {
        uint64_t operator()(const uint64_t* cont, uint64_t x, uint64_t y){
            return (cont[x] < cont[y]) ? x : y;
        };
    };

    /*util::heap_4ary<uint64_t, local_min, compare_min> m_heap(9);
    m_heap.push(5);
    m_heap.print();
    m_heap.push(4);
    m_heap.print();
    m_heap.push(9);
    m_heap.print();
    m_heap.push(8);
    m_heap.print();
    m_heap.push(1);
    m_heap.print();
    m_heap.push(6);
    m_heap.print();
    m_heap.push(7);
    m_heap.print();
    m_heap.push(2);
    m_heap.print();
    m_heap.push(3);
    m_heap.print();*/

    std::vector<uint64_t> va = {5, 4, 9, 8, 1, 6, 7, 2, 3};
    util::heap_4ary<uint64_t, local_min, compare_min> m_heap(9, va);

    while(!m_heap.empty()){
        std::cout << m_heap.top() << std::endl;
        m_heap.pop();
        m_heap.print();
    }

}


