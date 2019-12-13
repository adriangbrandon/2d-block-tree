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

#include <kr_block_adjacent_list_v4.hpp>
#include <kr_roll_adjacent_list_v4.hpp>
#include <block_tree_algorithm_helper.hpp>
#include <hash_table_chainning.hpp>
#include <iostream>



uint64_t comp_hash(const std::vector<std::vector<int64_t>> &adjacency_list,
             uint64_t prime,
             uint64_t x, uint64_t y,
             uint64_t block_size){

    ino64_t x_min = x;
    ino64_t x_max = x_min + block_size -1;
    ino64_t y_min = y;
    ino64_t y_max = y_min + block_size -1;
    uint64_t hash_i = 0;
    for(uint64_t i = y_min; i <= y_max; ++i){
        auto prev_value = (int64_t) x_min -1;
        auto it = std::lower_bound(adjacency_list[i].begin(), adjacency_list[i].end(), x_min);
        while(it != adjacency_list[i].end() && (*it) <= x_max){
            //2.1 Compute hash_value with 0s
            for(auto v = prev_value+1; v < (*it); ++v){
                hash_i = (hash_i * 2) % prime;
            }
            //2.2 Compute hash_value with 1
            hash_i = (hash_i * 2 + 1) % prime;
            //2.3 Next element of adjacent list
            prev_value = (*it);
            ++it;
        }
        //3. Check the last element and compute hash_value with 0s
        //prev_value is always smaller than block_size
        for(int64_t j = prev_value + 1; j <= x_max; ++j){
            hash_i = (hash_i * 2) % prime;
        }
        std::cout << "hash after: " << i << ": " << hash_i << std::endl;
    }
    //std::cout << "Hash Expected: " << hash_i << std::endl;
    return hash_i;
}


void print_adjacency_lists(const std::vector<std::vector<int64_t>> &adjacency_list){

    auto i = 0;
    for(const auto &list : adjacency_list){
        std::cout << "List " << i << ": { ";
        for(const auto &v : list){
            std::cout << v << ", ";
        }
        std::cout << " }" << std::endl;
        ++i;
    }
}


int main(int argc, char **argv) {


    uint64_t prime = 27162335252586509;
    //uint64_t prime = 3355443229;
    uint64_t block_size = 16;
    int64_t size = atoi(argv[1]);
    //uint64_t freq = atoi(argv[2]);
    uint64_t rep = atoi(argv[2]);

    std::cout << "Prime: " << prime << std::endl;
    std::vector<uint64_t> freqs = {10, 20, 40, 80, 160, 320, 640};

    for(const auto &freq : freqs){
        for(uint64_t i = 0; i < rep; ++i){
            std::vector<std::vector<int64_t>> adjacency_lists(size, std::vector<int64_t>());
            for(auto &list : adjacency_lists){
                int64_t last = rand() % freq;
                while(last < size){
                    list.push_back(last);
                    last = last + (rand() % freq +1);
                }
            }

            karp_rabin::kr_roll_adjacent_list_v4<> m_kr_roll(block_size, prime, adjacency_lists);
            while(m_kr_roll.next()){
                //std::cout << "Hash Roll " << i << ": " << m_kr_roll.hash << std::endl;
                //std::cout << "<x, y>: <" << m_kr_roll.col << ", " << m_kr_roll.row << ">" << std::endl;
                auto expected_hash = comp_hash(adjacency_lists, prime, m_kr_roll.col, m_kr_roll.row, block_size);
                if(expected_hash != m_kr_roll.hash){
                    std::cout << "Error at <x, y>: <" << m_kr_roll.col << ", " << m_kr_roll.row << ">" << std::endl;
                    std::cout << "Obtained: " << m_kr_roll.hash << std::endl;
                    std::cout << "Expected: " << expected_hash << std::endl;
                    karp_rabin::kr_block_adjacent_list_v4<> m_kr_block(block_size, prime, adjacency_lists);
                    m_kr_block.next();
                    std::cout << "Block: " << m_kr_block.hash << std::endl;
                    for(uint64_t r = m_kr_roll.row; r < m_kr_roll.row + block_size; ++r){
                        std::cout << "Row " << r - m_kr_roll.row << ": {";
                        uint64_t i_list = 0;
                        while(adjacency_lists[r][i_list] < m_kr_roll.col){
                            ++i_list;
                        }
                        while(adjacency_lists[r][i_list] < m_kr_roll.col + block_size){
                            std::cout << adjacency_lists[r][i_list] - m_kr_roll.col << ", ";
                            ++i_list;
                        }
                        std::cout << "}" << std::endl;
                    }
                    //print_adjacency_lists(adjacency_lists);
                    exit(1);
                }
            }
            //print_adjacency_lists(adjacency_lists);
            std::cout << "Everything is OK!" << std::endl;
        }
    }



    /*std::vector<uint64_t > row0 = {1, 3, 5, 7};
    std::vector<uint64_t > row1 = {};
    std::vector<uint64_t > row2 = {2, 6};
    std::vector<uint64_t > row3 = {};
    //std::vector<uint64_t > row4 = {4,5};
    std::vector<uint64_t > row4 = {};
    std::vector<uint64_t > row5 = {};
    std::vector<uint64_t > row6 = {};
    std::vector<uint64_t > row7 = {};
    //std::vector<uint64_t > row7 = {1,5};
    std::vector<std::vector<uint64_t>> matrix8_8;
    matrix8_8.push_back(row0);
    matrix8_8.push_back(row1);
    matrix8_8.push_back(row2);
    matrix8_8.push_back(row3);
    matrix8_8.push_back(row4);
    matrix8_8.push_back(row5);
    matrix8_8.push_back(row6);
    matrix8_8.push_back(row7);*/



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