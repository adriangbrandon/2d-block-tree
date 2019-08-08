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
// Created by Adrián on 06/08/2019.
//
#include <dataset_reader.hpp>
#include <block_tree.hpp>
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>

int main(int argc, char **argv) {

    if(argc != 4 && argc != 3){
        std::cout << argv[0] << "<dataset> <k> [limit]" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    uint64_t k = atoi(argv[2]);
    uint64_t limit = -1;
    if(argc == 4){
        limit = atoi(argv[3]);
    }


    std::vector<std::vector<int64_t>> adjacency_lists;
    dataset_reader::web_graph::read(dataset, adjacency_lists, limit);
    const auto copy_lists = adjacency_lists;

    std::cout << "Building Block-tree..." << std::endl;
    block_tree_2d::block_tree<> m_block_tree(adjacency_lists, k);
    std::cout << "The Block-tree was built." << std::endl;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".2dbt";
    sdsl::store_to_file(m_block_tree, name_file);
    //m_block_tree.print();
    std::cout << "Retrieving adjacency lists..." << std::flush;
    std::vector<std::vector<int64_t >> result;
    m_block_tree.access_region(0, 0, copy_lists.size() - 1, copy_lists.size() - 1, result);
    std::cout << "Adjacency lists were obtained." << std::endl;

    /*std::cout << "--------------------Result--------------------" << std::endl;
    block_tree_2d::algorithm::print_ajdacent_list(result);
    std::cout << "----------------------------------------------" << std::endl;*/

    std::cout << "Checking results." << std::endl;
    if (result.size() != copy_lists.size()) {
        std::cout << "Error: the number of lists is incorrect." << std::endl;
        std::cout << "Expected: " << copy_lists.size() << std::endl;
        std::cout << "Obtained: " << result.size() << std::endl;
        exit(10);
    }
    bool error = false;
    for (auto i = 0; i < result.size(); ++i) {
        if (result[i].size() != copy_lists[i].size()) {
            std::cout << "Error: the size of list " << i << " is incorrect." << std::endl;
            std::cout << "Expected: " << copy_lists[i].size() << std::endl;
            std::cout << "Obtained: " << result[i].size() << std::endl;
            //m_block_tree.access_region(10, 2, 11, 3, result);
            error = true;
        }
    }
    if (error){
        util::adjacency_list::write(copy_lists, "adjacency_lists.txt");
        exit(10);
    }else{
        for (auto i = 0; i < result.size(); ++i) {
            for (auto j = 0; j < result[i].size(); ++j) {
                if (result[i][j] != copy_lists[i][j]) {
                    std::cout << "Error: the " << j << "-th value of list " << i << " is incorrect." << std::endl;
                    std::cout << "Expected: " << copy_lists[i][j] << std::endl;
                    std::cout << "Obtained: " << result[i][j] << std::endl;
                    error = true;
                }
            }
        }
        if (!error) {
            std::cout << "Everything is OK!" << std::endl;
        }else{
            util::adjacency_list::write(copy_lists, "adjacency_lists.txt");
            exit(10);
        }
    }
    std::cout << std::endl;




}