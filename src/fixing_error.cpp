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
// Created by Adrián on 08/08/2019.
//

#include <block_tree.hpp>
#include <block_tree_skip_levels.hpp>
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>
#include <dataset_reader.hpp>

template<class t_block_tree>
void run_load(const std::string &dataset, const uint64_t k, const uint64_t limit){

    /*auto z_order = codes::zeta_order::encode(17074, 16967);
    auto pos = codes::zeta_order::decode(z_order);
    std::cout << "Pos: " << pos.first << ", " << pos.second << std::endl;
    std::cout << "Z_order: " << z_order << std::endl;
    exit(0);*/
    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".2dbt";
    sdsl::load_from_file(m_block_tree, name_file);

    std::cout << "Size in bytes: " << sdsl::size_in_bytes(m_block_tree) << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(m_block_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(m_block_tree, name_file + ".html");

    std::vector<std::vector<int64_t>> copy_lists;
    dataset_reader::web_graph::read(dataset, copy_lists, limit);

    for(uint64_t i = 774454; i < 774455; ++i){
        for(uint64_t j = 0; j < copy_lists[i].size(); ++j){
            //if(copy_lists[i][j] >= 136592 && copy_lists[i][j] < 136600){
                std::cout << copy_lists[i][j] << ", ";
            //}

        }
        std::cout << std::endl;
    }

    std::cout << "Retrieving adjacency lists..." << std::endl;
    std::vector<std::vector<int64_t >> result;
    //m_block_tree.access_region(0, 0, copy_lists.size() - 1, copy_lists.size() - 1, result);
    m_block_tree.access_region(136599, 135741, 136599, 135741, result);
    //m_block_tree.access_region(635470, 623746, 635470, 623746, result);
    std::cout << "Obtained list: " ;
    for(uint64_t i = 0; i < result.size(); ++i){
        for(uint64_t j = 0; j < result[i].size(); ++j){
            std::cout << result[i][j] << ", ";
        }
    }
    std::cout << std::endl;

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
            std::cout << "Expected list: " ;
            for(uint64_t j = 0; j < copy_lists[i].size(); ++j){
                std::cout << copy_lists[i][j] << ", ";
            }
            std::cout << std::endl;
            std::cout << "Obtained list: " ;
            for(uint64_t j = 0; j < result[i].size(); ++j){
                std::cout << result[i][j] << ", ";
            }
            std::cout << std::endl;
            std::cout << std::endl;
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

int main(int argc, char **argv) {

    if(argc != 5 && argc != 4){
        std::cout << argv[0] << "<dataset> <type> <k>  [limit]" << std::endl;
        std::cout << "type: naive, skip_levels" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    std::string type = argv[2];
    auto k = static_cast<uint64_t >(atoi(argv[3]));
    auto limit = static_cast<uint64_t>(-1);
    if(argc == 5){
        limit = static_cast<uint64_t >(atoi(argv[4]));
    }

    if(type == "naive"){
        run_load<block_tree_2d::block_tree<>>(dataset, k, limit);
    }else if (type == "skip_levels"){
        run_load<block_tree_2d::block_tree_skip_levels<>>(dataset, k, limit);
    }else{
        std::cout << "Type: " << type << " is not supported." << std::endl;
    }






}