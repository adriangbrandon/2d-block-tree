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

#include <kr_block_adjacent_list_v2.hpp>
#include <kr_roll_adjacent_list_v2.hpp>
#include <block_tree_algorithm_helper.hpp>
#include <hash_table_chainning.hpp>
#include <iostream>


int main(int argc, char **argv) {

    std::vector<uint64_t > row0 = {1, 3, 5, 7};
    std::vector<uint64_t > row1 = {};
    std::vector<uint64_t > row2 = {2, 6};
    std::vector<uint64_t > row3 = {};
    //std::vector<uint64_t > row4 = {4,5};
    std::vector<uint64_t > row4 = {4,5};
    std::vector<uint64_t > row5 = {};
    std::vector<uint64_t > row6 = {};
    std::vector<uint64_t > row7 = {1,5};
    //std::vector<uint64_t > row7 = {1,5};
    std::vector<std::vector<uint64_t>> matrix8_8;
    matrix8_8.push_back(row0);
    matrix8_8.push_back(row1);
    matrix8_8.push_back(row2);
    matrix8_8.push_back(row3);
    matrix8_8.push_back(row4);
    matrix8_8.push_back(row5);
    matrix8_8.push_back(row6);
    matrix8_8.push_back(row7);


    karp_rabin::kr_block_adjacent_list_v2<> m_kr(4, 3355443229, matrix8_8);
    auto i = 0;
    while(m_kr.next()){
        std::cout << "Hash " << i << ": " << m_kr.hash << std::endl;
        std::cout << "<x, y>: <" << m_kr.col << ", " << m_kr.row << ">" << std::endl;
        ++i;
    }

    karp_rabin::kr_roll_adjacent_list_v2<> m_kr_roll(4, 3355443229, matrix8_8);
    i = 0;
    while(m_kr_roll.next()){
        std::cout << "Hash Roll " << i << ": " << m_kr_roll.hash << std::endl;
        std::cout << "<x, y>: <" << m_kr_roll.col << ", " << m_kr_roll.row << ">" << std::endl;
        ++i;
    }
    /*auto old_hash = m_kr_roll.hash;
    std::cout << "Hash 0: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 0.1: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 0.2: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 0.3: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 1: " << m_kr_roll.hash << std::endl;
    m_kr_roll.next_row(old_hash);
    std::cout << "Hash 0.d1: " << m_kr_roll.hash << std::endl;
    old_hash = m_kr_roll.hash;
    m_kr_roll.next_row(old_hash);
    std::cout << "Hash 0.d2: " << m_kr_roll.hash << std::endl;
    old_hash = m_kr_roll.hash;
    m_kr_roll.next_row(old_hash);
    std::cout << "Hash 0.d3: " << m_kr_roll.hash << std::endl;
    old_hash = m_kr_roll.hash;
    m_kr_roll.next_row(old_hash);
    std::cout << "Hash 2: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 2.1: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 2.2: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 2.3: " << m_kr_roll.hash << std::endl;
    m_kr_roll.shift_right();
    std::cout << "Hash 3: " << m_kr_roll.hash << std::endl;*/

}