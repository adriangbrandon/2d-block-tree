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
// Created by Adrián on 12/06/2020.
//

#include <block_tree_double_hybrid_skipping_block.hpp>
#include <dataset_reader.hpp>

template<class t_block_tree>
void run_test(const std::string &dataset, const std::string &index){

    std::vector<std::vector<int64_t>> adjacency_lists;
    dataset_reader::web_graph::read(dataset, adjacency_lists);

    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    sdsl::load_from_file(m_block_tree, index);

    std::cout << "Size in bytes 2dbt: " << sdsl::size_in_bytes(m_block_tree) << std::endl;

    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_block_tree.neigh(id);
        if(r.size() != adjacency_lists[id].size()){
            std::cout << "Error. Direct neighbors: " << id << std::endl;
            std::cout << "Expected size: " << adjacency_lists[id].size() << std::endl;
            std::cout << "Obtained size: " << r.size() << std::endl;
            exit(10);
        }else{
            for(uint64_t i = 0; i < r.size(); ++i){
                if(r[i] != adjacency_lists[id][i]){
                    std::cout << "Error. Direct neighbors: " << id << std::endl;
                    std::cout << "Expected (" <<i << "): " << adjacency_lists[id][i] << std::endl;
                    std::cout << "Obtained (" <<i << "): " << r[i] << std::endl;
                    exit(10);
                }
            }
            //std::cout << "Direct neighbors: " << id << ". OK." << std::endl;
        }
    }

    std::cout << "Reversing adjacency lists" << std::endl;
    std::vector<std::vector<int64_t>> rev_adjacency_lists(adjacency_lists.size());
    for(uint64_t id = 0; id < adjacency_lists.size(); ++id){
        for(uint64_t j = 0; j < adjacency_lists[id].size(); ++j){
            rev_adjacency_lists[adjacency_lists[id][j]].push_back(id);
        }
    }

    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_block_tree.reverse_neigh(id);
        if(r.size() != rev_adjacency_lists[id].size()){
            std::cout << "Error. Reverse neighbors: " << id << std::endl;
            std::cout << "Expected size: " << rev_adjacency_lists[id].size() << std::endl;
            std::cout << "Obtained size: " << r.size() << std::endl;
            exit(10);
        }else{
            for(uint64_t i = 0; i < r.size(); ++i){
                if(r[i] != rev_adjacency_lists[id][i]){
                    std::cout << "Error. Reverse neighbors: " << id << std::endl;
                    std::cout << "Expected (" <<i << "): " << rev_adjacency_lists[id][i] << std::endl;
                    std::cout << "Obtained (" <<i << "): " << r[i] << std::endl;
                    exit(10);
                }
            }
            //std::cout << "Reverse neighbors: " << id << ". OK." << std::endl;
        }
    }
    std::cout << "Everything is OK!" << std::endl;
}

int main(int argc, char **argv) {

    if(argc != 3){
        std::cout << argv[0] << "<dataset> <index>" << std::endl;
        return 0;
    }
    std::string dataset = argv[1];
    std::string index = argv[2];
    run_test<block_tree_2d::block_tree_double_hybrid_skipping_block<>>(dataset, index);


}