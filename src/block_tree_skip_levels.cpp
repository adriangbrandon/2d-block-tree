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


#include <vector>
#include <adjacency_list_helper.hpp>
#include <block_tree_skip_levels.hpp>

int main(int argc, char **argv) {

    typedef uint64_t hash_type;


    typedef hash_table::hash_table_chainning<hash_type, uint64_t> htc_type;
    std::vector<int64_t> row0 = {2};
    std::vector<int64_t> row1 = {2};
    std::vector<int64_t> row2 = {};
    std::vector<int64_t> row3 = {};
    std::vector<int64_t> row4 = {};
    std::vector<int64_t> row5 = {6};
    std::vector<int64_t> row6 = {};
    std::vector<int64_t> row7 = {};
    std::vector<int64_t> row8 = {8};
    std::vector<int64_t> row9 = {8};
    std::vector<int64_t> row10 = {};
    std::vector<int64_t> row11 = {};
    std::vector<int64_t> row12 = {};
    std::vector<int64_t> row13 = {13,14,15};
    std::vector<int64_t> row14 = {12,13,14,15};
    std::vector<int64_t> row15 = {12,13,14,15};





    uint64_t k =2;
    uint64_t dimensions = 16;
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
        //util::adjacency_list::read(matrix8_8, "adjacency_lists.txt");

        std::cout << matrix8_8.size() << std::endl;

        block_tree_2d::algorithm::print_ajdacent_list(matrix8_8);

        std::cout << "Building Block-tree dimensions=" << dimensions << std::endl;
        block_tree_2d::block_tree_skip_levels<> m_block_tree(matrix8_8, k);
        std::cout << "Done." << std::endl;
        m_block_tree.print();
        std::cout << "Retrieving adjacency lists...";
        std::vector<std::vector<int64_t >> result, result2;
        m_block_tree.access_region(0, 0, dimensions - 1, dimensions - 1, result);
        std::cout << "Done." << std::endl;

        block_tree_2d::algorithm::print_ajdacent_list(result);
        sdsl::store_to_file(m_block_tree, "test.2dbt");
        std::ofstream out("test.2dbt");
        m_block_tree.serialize(out);
        out.close();

        block_tree_2d::block_tree_skip_levels<> m_block_tree2;
        std::ifstream in("test.2dbt");
        m_block_tree2.load(in);
        in.close();

        m_block_tree.access_region(0, 0, dimensions - 1, dimensions - 1, result2);
        std::cout << "Done." << std::endl;

        block_tree_2d::algorithm::print_ajdacent_list(result2);
    //}

}
