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

int main(int argc, char **argv) {

    typedef uint64_t hash_type;
    typedef hash_table::hash_table_chainning<hash_type, uint64_t> htc_type;
    std::vector<int64_t> row0 = {2, 3, 4, 7};
    std::vector<int64_t> row1 = {0, 4};
    std::vector<int64_t> row2 = {2, 3, 5, 7};
    std::vector<int64_t> row3 = {3, 5};
    std::vector<int64_t> row4 = {2, 6};
    std::vector<int64_t> row5 = {1, 3, 6};
    std::vector<int64_t> row6 = {2, 3, 5};
    std::vector<int64_t> row7 = {1, 5};




    uint64_t k =4;
    uint64_t dimensions = 8;
    //for(size_t i = 0; i < 100; ++i){
        std::vector< std::vector<int64_t>> matrix8_8;
        matrix8_8.push_back(row0);
        matrix8_8.push_back(row1);
        matrix8_8.push_back(row2);
        matrix8_8.push_back(row3);
        matrix8_8.push_back(row4);
        matrix8_8.push_back(row5);
        matrix8_8.push_back(row6);
        matrix8_8.push_back(row7);
        std::cout << "Building Block-tree dimensions=" << dimensions;
        block_tree_2d::block_tree<> m_block_tree(matrix8_8, dimensions, k);
        std::cout << "Done." << std::endl;
        m_block_tree.print();
        std::cout << "Retrieving adjacency lists...";
        std::vector<std::vector<int64_t >> result;
        m_block_tree.access_region(0, 0, dimensions - 1, dimensions - 1, result);
        std::cout << "Done." << std::endl;

        block_tree_2d::algorithm::print_ajdacent_list(result);
    //}

}
