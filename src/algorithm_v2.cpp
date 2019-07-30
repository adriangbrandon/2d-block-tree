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
// Created by Adrián on 20/07/2019.
//

#include <block_tree_algorithm_helper_v2.hpp>
#include <hash_table_chainning.hpp>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {

    typedef uint64_t hash_type;
    typedef hash_table::hash_table_chainning<hash_type, uint64_t> htc_type;

    std::vector<int64_t> row0 = {1, 3, 5, 7};
    std::vector<int64_t> row1 = {};
    std::vector<int64_t> row2 = {2, 6};
    std::vector<int64_t> row3 = {};
    std::vector<int64_t> row4 = {4, 5};
    std::vector<int64_t> row5 = {2};
    std::vector<int64_t> row6 = {1};
    std::vector<int64_t> row7 = {1, 5};

    /*td::vector<int64_t> row0 = {7};
    std::vector<int64_t> row1 = {3};
    std::vector<int64_t> row2 = {0, 3, 6};
    std::vector<int64_t> row3 = {};
    std::vector<int64_t> row4 = {3};
    std::vector<int64_t> row5 = {0,6};
    std::vector<int64_t> row6 = {2};
    std::vector<int64_t> row7 = {3};*/

    /*std::vector<int64_t> row0 = {};
    std::vector<int64_t> row1 = {1};
    std::vector<int64_t> row2 = {2};
    std::vector<int64_t> row3 = {};
    std::vector<int64_t> row4 = {4};
    std::vector<int64_t> row5 = {5};
    std::vector<int64_t> row6 = {};
    std::vector<int64_t> row7 = {7};*/

    /*std::vector<int64_t> row0 = {1,4,6};
    std::vector<int64_t> row1 = {6};
    std::vector<int64_t> row2 = {3};
    std::vector<int64_t> row3 = {3,4};
    std::vector<int64_t> row4 = {};
    std::vector<int64_t> row5 = {};
    std::vector<int64_t> row6 = {};
    std::vector<int64_t> row7 = {};*/

    std::vector< std::vector<int64_t>> matrix8_8;
    matrix8_8.push_back(row0);
    matrix8_8.push_back(row1);
    matrix8_8.push_back(row2);
    matrix8_8.push_back(row3);
    matrix8_8.push_back(row4);
    matrix8_8.push_back(row5);
    matrix8_8.push_back(row6);
    matrix8_8.push_back(row7);



    uint64_t k = 2;
    uint64_t k_pow_2 = 4;
    uint64_t dimensions = 8;
    uint64_t levels = 2;


    uint64_t level = 0;
    uint64_t blocks = k_pow_2;
    uint64_t block_size = dimensions / k;

    std::vector<block_tree_2d::algorithm::node_type> nodes(blocks);
    sdsl::bit_vector bitmap(blocks, 1);
    sdsl::bit_vector::rank_1_type rank;
    sdsl::util::init_support(rank, &bitmap);
    block_tree_2d::algorithm::hash_type hash = {{0,0}, {1,1}, {2,2}, {3,3}};
    while(block_size > 1){
        htc_type m_htc(nodes.size());
        block_tree_2d::algorithm::get_fingerprint_blocks(matrix8_8, m_htc, dimensions, block_size, hash, nodes);
        block_tree_2d::algorithm::get_type_of_nodes(matrix8_8, m_htc, dimensions, block_size, hash, nodes);
        auto i = 0;
        std::cout << "--------- Nodes at level: " << level << " ---------------" << std::endl;
        for(const auto &n : nodes){
            std::cout << "Node: " << i << std::endl;
            std::cout << "z_order: " << n.z_order << std::endl;
            std::cout << "type   : " << n.type << std::endl;
            std::cout << "offset : <" << n.offset_x << ", " << n.offset_y << ">" << std::endl;
            std::cout << "ptr    : " << n.ptr << std::endl;
            std::cout << "hash   : " << n.hash << std::endl;
            std::cout << std::endl;
            ++i;
        }
        std::cout << "----------------------------------------------------------" << std::endl;
        ++level;
        block_tree_2d::algorithm::prepare_next_level(matrix8_8, hash, k_pow_2, nodes);
        block_size = block_size / k;
    }

    /*std::vector<block_tree_2d::algorithm::node_type> nodes(16);
    sdsl::bit_vector bitmap(16, 1);
    sdsl::bit_vector::rank_1_type rank;
    sdsl::util::init_support(rank, &bitmap);

    block_tree_2d::algorithm::get_fingerprint_blocks(matrix8_8, m_htc, 8, 2, rank, nodes);
    std::cout << "Nodes" << std::endl;
    auto i = 0;
    for(const auto &n : nodes){
        std::cout << "Node: " << i << std::endl;
        std::cout << "z_order: " << n.z_order << std::endl;
        std::cout << "type   : " << n.type << std::endl;
        std::cout << "offset : <" << n.offset_x << ", " << n.offset_y << ">" << std::endl;
        std::cout << "ptr    : " << n.ptr << std::endl;
        std::cout << "hash   : " << n.hash << std::endl;
        std::cout << std::endl;
        ++i;
    }
    block_tree_2d::algorithm::get_type_of_nodes(matrix8_8, m_htc, 8, 2, rank, nodes);

    i=0;
    for(const auto &n : nodes){
        std::cout << "Node: " << i << std::endl;
        std::cout << "z_order: " << n.z_order << std::endl;
        std::cout << "type   : " << n.type << std::endl;
        std::cout << "offset : <" << n.offset_x << ", " << n.offset_y << ">" << std::endl;
        std::cout << "ptr    : " << n.ptr << std::endl;
        std::cout << "hash   : " << n.hash << std::endl;
        std::cout << std::endl;
        ++i;
    }*/

}