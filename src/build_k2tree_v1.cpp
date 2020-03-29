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
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>
#include <k2_tree.hpp>
#include <k2_tree_compression_leaves.hpp>
#include <block_tree_algorithm_helper_v2.hpp>



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
    auto h = (uint64_t) std::ceil(std::log(adjacency_lists.size())/std::log(k));
    auto total_size = (uint64_t) std::pow(k, h);
    if(adjacency_lists.size() < total_size){
        adjacency_lists.resize(total_size);
    }

    std::cout << "Building K2-tree..." << std::endl;

    block_tree_2d::k2_tree_compression_leaves<> m_k2_tree(adjacency_lists,2);
    std::cout << "The K2-tree was built." << std::endl;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".k2t";
    sdsl::store_to_file(m_k2_tree, name_file);


    auto k2_tree_bytes = sdsl::size_in_bytes(m_k2_tree);
    std::cout << "Size in bytes: " << k2_tree_bytes << std::endl;
    std::cout << "Retrieving adjacency lists... " << std::flush;
    std::vector<std::vector<int64_t >> result;
    m_k2_tree.access_region(0, 0, adjacency_lists.size() - 1, adjacency_lists.size() - 1, result);
    std::cout << "Done." << std::endl;
    /*std::cout << "--------------------Result--------------------" << std::endl;
    block_tree_2d::algorithm::print_ajdacent_list(result);
    std::cout << "----------------------------------------------" << std::endl;*/

    std::cout << "Checking results." << std::endl;
    if (result.size() != adjacency_lists.size()) {
        std::cout << "Error: the number of lists is incorrect." << std::endl;
        std::cout << "Expected: " << adjacency_lists.size() << std::endl;
        std::cout << "Obtained: " << result.size() << std::endl;
        exit(10);
    }
    bool error = false;
    for (auto i = 0; i < result.size(); ++i) {
        if (!error && result[i].size() != adjacency_lists[i].size()) {
            std::cout << "Error: the size of list " << i << " is incorrect." << std::endl;
            std::cout << "Expected: " << adjacency_lists[i].size() << std::endl;
            std::cout << "Obtained: " << result[i].size() << std::endl;
            for(uint64_t o = 0; o < adjacency_lists[i].size(); ++o){
                std::cout << adjacency_lists[i][o] << ",";
            }
            std::cout << std::endl;
            for(uint64_t o = 0; o < result[i].size(); ++o){
                std::cout << result[i][o] << ",";
            }
            std::cout << std::endl;
            //m_block_tree.access_region(10, 2, 11, 3, result);
            error = true;
        }
    }
    if (error){
        util::adjacency_list::write(adjacency_lists, "adjacency_lists.txt");
        exit(10);
    }else{
        for (auto i = 0; i < result.size(); ++i) {
            for (auto j = 0; j < result[i].size(); ++j) {
                if (result[i][j] != adjacency_lists[i][j]) {
                    //  std::cout << "Error: the " << j << "-th value of list " << i << " is incorrect." << std::endl;
                    //  std::cout << "Expected: " << copy_lists[i][j] << std::endl;
                    //  std::cout << "Obtained: " << result[i][j] << std::endl;
                    error = true;
                }
            }
        }
        if (!error) {
            std::cout << "Everything is OK!" << std::endl;
        }else{
            util::adjacency_list::write(adjacency_lists, "adjacency_lists.txt");
            exit(10);
        }
    }
    std::cout << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(m_k2_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(m_k2_tree, name_file + ".html");



}