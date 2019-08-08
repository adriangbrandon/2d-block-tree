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
#include <sdsl/k2_tree.hpp>

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
    std::vector<std::tuple<uint64_t, uint64_t>> al;
    for(uint64_t i = 0; i < adjacency_lists.size(); ++i){
        for(uint64_t j = 0; j < adjacency_lists[i].size(); ++j){
            al.push_back({i, adjacency_lists[i][j]});
        }
    }

    std::cout << "Building K2-tree..." << std::endl;

    sdsl::k2_tree<2> k2_tree(al, adjacency_lists.size());
    std::cout << "The K2-tree was built." << std::endl;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".k2t";
    sdsl::store_to_file(k2_tree, name_file);

    std::cout << "Size in bytes: " << sdsl::size_in_bytes(k2_tree) << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(k2_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(k2_tree, name_file + ".html");



}