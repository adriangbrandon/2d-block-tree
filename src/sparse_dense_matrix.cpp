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
// Created by Adrián on 12/03/2020.
//

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
#include <block_tree_skip_levels.hpp>
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>
#include <block_tree_intersection_lists.hpp>
#include <block_tree_hybrid.hpp>
#include <block_tree_double_hybrid.hpp>

template<class t_block_tree>
void build(t_block_tree &b, std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = t_block_tree(adjacency_lists, k);
}

void build(block_tree_2d::block_tree_hybrid<> &b, std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = block_tree_2d::block_tree_hybrid<>(adjacency_lists, k, last_block_size_k2_tree);
    std::cout << "Block tree height=" << b.height << std::endl;
}

void build(block_tree_2d::block_tree_double_hybrid<> &b, std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = block_tree_2d::block_tree_double_hybrid<>(adjacency_lists, k, last_block_size_k2_tree);
    std::cout << "Block tree height=" << b.height << std::endl;
    std::cout << "There are pointers from level " << b.minimum_level+1 << " up to level " << b.maximum_level-1 << std::endl;
}

uint64_t delete_blocks_one_edge(const uint64_t block_size, std::vector<std::vector<int64_t>> &adjacency_lists){

    std::vector<std::pair<uint64_t , uint64_t >> point_to_remove;
    uint64_t y = 0;
    while(y < adjacency_lists.size()){
        uint64_t x = 0;
        //uint64_t i = 0, j = 0;
        std::vector<uint64_t> pos(block_size, 0);
        while(x < adjacency_lists.size()){
            uint64_t rem_x, rem_y, number_ones = 0;
            uint64_t j = y;
            while(j < y + block_size){
                while(!adjacency_lists[j].empty() && pos[j-y] < adjacency_lists[j].size() &&
                       adjacency_lists[j][pos[j-y]] < x + block_size){

                    rem_x = adjacency_lists[j][pos[j-y]];
                    rem_y = j;
                    ++number_ones;
                    ++pos[j-y];
                }
                ++j;
            }
            if(number_ones == 1) {
                point_to_remove.emplace_back(rem_x, rem_y);
            };
            x = x + block_size;
        }
        y += block_size;
    }
    auto result = point_to_remove.size();
    std::cout << "Blocks with only one edge: " << result << std::endl;
    for(const auto &p:point_to_remove){
        auto it = std::find( adjacency_lists[p.second].begin(),  adjacency_lists[p.second].end(), p.first);
        adjacency_lists[p.second].erase(it);
    }
    return result;
}

template<class t_block_tree>
void run_build(const std::string &type, const std::string &dataset, const uint64_t k, const uint64_t limit, const uint64_t last_block_size_k2_tree){
    std::vector<std::vector<int64_t>> adjacency_lists;
    dataset_reader::web_graph::read(dataset, adjacency_lists, limit);

    auto deleted = delete_blocks_one_edge(32, adjacency_lists);
    uint64_t size = deleted*(10);
    std::cout << "Building Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    auto t0 = std::chrono::high_resolution_clock::now();
    build(m_block_tree, adjacency_lists, k, last_block_size_k2_tree);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t1-t0).count();
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".2dbt";
    sdsl::store_to_file(m_block_tree, name_file);

    auto size_bt = sdsl::size_in_bytes(m_block_tree);
    std::cout << "The Block-tree was built in " << duration << " seconds and uses " << size_bt << " bytes." << std::endl;
    std::cout << deleted << " blocks of size 32x32 were deleted and require " << size / 8 << " bytes." << std::endl;
    std::cout << duration << " " << size_bt << std::endl;
}

int main(int argc, char **argv) {

    if(argc != 6  && argc != 5 && argc !=4){
        std::cout << argv[0] << "<dataset> <type> <k> [last_block_size_k2_tree] [limit]" << std::endl;
        std::cout << "type: naive, skip_levels, hybrid, double_hybrid" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    std::string type = argv[2];
    auto k = static_cast<uint64_t >(atoi(argv[3]));
    auto limit = static_cast<uint64_t>(-1);
    auto last_block_size_k2_tree = static_cast<uint64_t>(-1);
    if( (type == "hybrid" || type == "double_hybrid")){
        if(argc == 5){
            last_block_size_k2_tree = static_cast<uint64_t >(atoi(argv[4]));
        }else if(argc == 6){
            limit = static_cast<uint64_t >(atoi(argv[5]));
        }else{
            std::cout << argv[0] << "<dataset> " << type << " <k> <last_block_size_k2_tree> [limit]" << std::endl;
            //std::cout << "type: naive, skip_levels, hybrid, double_hybrid" << std::endl;
            return 0;
        }

    }else{
        if(argc == 5){
            limit = static_cast<uint64_t >(atoi(argv[4]));
        }
    }




    if(type == "naive"){
        run_build<block_tree_2d::block_tree<>>(type, dataset, k, limit, last_block_size_k2_tree);
    }else if (type == "skip_levels"){
        run_build<block_tree_2d::block_tree_skip_levels<>>(type, dataset, k, limit, last_block_size_k2_tree);
    }else if (type == "skip_levels_lists"){
        run_build<block_tree_2d::block_tree_intersection_lists<>>(type, dataset, k, limit, last_block_size_k2_tree);
    }else if (type == "hybrid"){
        run_build<block_tree_2d::block_tree_hybrid<>>(type, dataset, k, limit, last_block_size_k2_tree);
    }else if (type == "double_hybrid"){
        run_build<block_tree_2d::block_tree_double_hybrid<>>(type, dataset, k, limit, last_block_size_k2_tree);
    } else{
        std::cout << "Type: " << type << " is not supported." << std::endl;
    }
}
