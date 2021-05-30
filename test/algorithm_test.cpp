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

#include <block_tree_algorithm_helper.hpp>
#include <hash_table_chainning.hpp>
#include <iostream>
#include <vector>

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


            uint64_t k = 2;
            uint64_t k_pow_2 = 4;
            uint64_t levels = 2;
            uint64_t level = 0;
            uint64_t blocks = k_pow_2;
            uint64_t block_size = dimensions / k;

            std::vector<block_tree_2d::algorithm::node_type> nodes(blocks);
            block_tree_2d::algorithm::hash_type hash = {{0, 0},
                                                        {1, 1},
                                                        {2, 2},
                                                        {3, 3}};
            std::cout << "Total number of ones=" << number_of_ones(adjacency_lists) << std::endl;
            while (block_size > 1) {
                htc_type m_htc(2*nodes.size());
                block_tree_2d::algorithm::get_fingerprint_blocks(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
                block_tree_2d::algorithm::get_type_of_nodes(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
                /*auto n_i = 0;
                std::cout << "--------- Nodes at level: " << level << " ---------------" << std::endl;
                for (const auto &n : nodes) {
                    std::cout << "Node: " << n_i << std::endl;
                    std::cout << "z_order: " << n.z_order << std::endl;
                    std::cout << "type   : " << n.type << std::endl;
                    std::cout << "offset : <" << n.offset_x << ", " << n.offset_y << ">" << std::endl;
                    std::cout << "ptr    : " << n.ptr << std::endl;
                    std::cout << "hash   : " << n.hash << std::endl;
                    std::cout << std::endl;
                    ++n_i;
                }
                std::cout << "----------------------------------------------------------" << std::endl;*/

                ++level;
                block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, k_pow_2, nodes);
                block_size = block_size / k;
                std::cout << "Number of ones=" << number_of_ones(adjacency_lists) << " after level=" << level-1 << std::endl;

            }
            std::cout << std::endl;
        }
    }

}