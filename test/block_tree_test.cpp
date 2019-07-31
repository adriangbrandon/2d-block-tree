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
// Created by Adrián on 26/07/2019.
//

#include <iostream>
#include <vector>
#include <block_tree.hpp>

#define VERBOSE 1

uint64_t number_of_ones(const std::vector<std::vector<int64_t>> &adjacency_lists) {
    uint64_t n_ones = 0;
    for (auto &list : adjacency_lists) {
        n_ones += list.size();
    }
    return n_ones;
}

int main(int argc, char **argv) {

    typedef uint64_t hash_type;
    typedef hash_table::hash_table_chainning <hash_type, uint64_t> htc_type;

    uint64_t dimensions = 1024;
    //uint64_t freq = atoi(argv[2]);
    uint64_t k = 2;
    uint64_t rep = atoi(argv[1]);

    uint64_t freq = 100;
    std::vector<uint64_t> freqs = {freq};
    while(freq < dimensions){
        freq *= 2;
        freqs.push_back(freq);
    }

    for(const auto &freq : freqs) {
        for (uint64_t i = 0; i < rep; ++i) {
            std::vector<std::vector<int64_t>> adjacency_lists(dimensions, std::vector<int64_t>());
            for (auto &list : adjacency_lists) {
                int64_t last = rand() % freq;
                while (last < dimensions) {
                    list.push_back(last);
                    last = last + (rand() % freq + 1);
                }
            }

            
            block_tree_2d::block_tree<> m_block_tree(adjacency_lists, dimensions, k);
            
        }
    }

}