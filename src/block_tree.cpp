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
// Created by Adrián on 02/08/2019.
//


#include <block_tree.hpp>
#include <vector>
#include <adjacency_list_helper.hpp>
#include <block_tree_intersection_lists.hpp>
#include <block_tree_hybrid.hpp>
#include <block_tree_double_hybrid.hpp>
#include <block_tree_double_hybrid_skipping_block.hpp>

int main(int argc, char **argv) {

    typedef uint64_t hash_type;


    typedef hash_table::hash_table_chainning<hash_type, uint64_t> htc_type;
    std::vector<int64_t> row0 = {};
    std::vector<int64_t> row1 = {};
    std::vector<int64_t> row2 = {6};
    std::vector<int64_t> row3 = {};
    std::vector<int64_t> row4 = {7};
    std::vector<int64_t> row5 = {2};
    std::vector<int64_t> row6 = {};
    std::vector<int64_t> row7 = {3};
    std::vector<int64_t> row8 = {};
    std::vector<int64_t> row9 = {};
    std::vector<int64_t> row10 = {6};
    std::vector<int64_t> row11 = {5, 10};
    std::vector<int64_t> row12 = {4};
    std::vector<int64_t> row13 = {3, 13};
    std::vector<int64_t> row14 = {2};
    std::vector<int64_t> row15 = {};
    std::vector<int64_t> row16 = {16, 17};
    std::vector<int64_t> row17 = {16, 17};
    std::vector<int64_t> row18 = {14};
    std::vector<int64_t> row19 = {5, 14};
    std::vector<int64_t> row20 = {13};
    std::vector<int64_t> row21 = {8};
    std::vector<int64_t> row22 = {};
    std::vector<int64_t> row23 = {};
    std::vector<int64_t> row24 = {9};
    std::vector<int64_t> row25 = {};
    std::vector<int64_t> row26 = {};
    std::vector<int64_t> row27 = {2, 10, 13};
    std::vector<int64_t> row28 = {3, 5, 11};
    std::vector<int64_t> row29 = {4, 10, 13};
    std::vector<int64_t> row30 = {};
    std::vector<int64_t> row31 = {};
    /*std::vector<int64_t> row0 = {1, 2, 4, 5, 6, 7};
    std::vector<int64_t> row1 = {1, 3, 4, 6, 7};
    std::vector<int64_t> row2 = {0, 2, 4, 6};
    std::vector<int64_t> row3 = {1, 2, 4, 6};
    std::vector<int64_t> row4 = {1, 3, 5, 7};
    std::vector<int64_t> row5 = {1, 3, 4, 6, 7};
    std::vector<int64_t> row6 = {0, 1, 2, 3, 5, 7};
    std::vector<int64_t> row7 = {0, 1, 3, 4, 6, 7};*/





    uint64_t k =2;
    uint64_t dimensions = 32;
    //for(size_t i = 0; i < 100; ++i){
        std::vector< std::vector<int64_t>> matrix8_8;
        //util::adjacency_list::read(matrix8_8, "adjacency_lists.txt");std::vector< std::vector<int64_t>> matrix8_8;
        matrix8_8.push_back(row0);
        matrix8_8.push_back(row1);
        matrix8_8.push_back(row2);
        matrix8_8.push_back(row3);
        matrix8_8.push_back(row4);
        matrix8_8.push_back(row5);
        matrix8_8.push_back(row6);
        matrix8_8.push_back(row7);
        matrix8_8.push_back(row8);
        matrix8_8.push_back(row9);
        matrix8_8.push_back(row10);
        matrix8_8.push_back(row11);
        matrix8_8.push_back(row12);
        matrix8_8.push_back(row13);
        matrix8_8.push_back(row14);
        matrix8_8.push_back(row15);
        matrix8_8.push_back(row16);
        matrix8_8.push_back(row17);
        matrix8_8.push_back(row18);
        matrix8_8.push_back(row19);
        matrix8_8.push_back(row20);
        matrix8_8.push_back(row21);
        matrix8_8.push_back(row22);
        matrix8_8.push_back(row23);
        matrix8_8.push_back(row24);
        matrix8_8.push_back(row25);
        matrix8_8.push_back(row26);
        matrix8_8.push_back(row27);
        matrix8_8.push_back(row28);
        matrix8_8.push_back(row29);
        matrix8_8.push_back(row30);
        matrix8_8.push_back(row31);
        std::cout << matrix8_8.size() << std::endl;

        block_tree_2d::algorithm::print_ajdacent_list(matrix8_8);

        std::cout << "Building Block-tree dimensions=" << dimensions;
        block_tree_2d::block_tree_double_hybrid_skipping_block<> m_block_tree(matrix8_8, k, 8);
        std::cout << "Done." << std::endl;
        m_block_tree.print();
        std::cout << "Retrieving adjacency lists...";
        std::vector<std::vector<int64_t >> result, result2;
        m_block_tree.access_region(0, 0, dimensions - 1, dimensions - 1, result);
        std::cout << "Done." << std::endl;

        block_tree_2d::algorithm::print_ajdacent_list(result);
        sdsl::store_to_file(m_block_tree, "test.2dbt");

        block_tree_2d::block_tree_double_hybrid_skipping_block<> m_block_tree2;
        sdsl::load_from_file(m_block_tree2, "test.2dbt");

        m_block_tree2.access_region(0, 0, dimensions - 1, dimensions - 1, result2);
        std::cout << "Done." << std::endl;

        std::cout << "Block tree starts from level " << m_block_tree2.minimum_level+1
        << " up to level " << m_block_tree2.maximum_level-1 << std::endl;
        block_tree_2d::algorithm::print_ajdacent_list(result2);
    //}

}
