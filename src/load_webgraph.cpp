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
#include <block_tree_intersection_lists.hpp>
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>
#include <block_tree_double_hybrid_skipping_block.hpp>


template<class t_block_tree>
void run_load(const std::string &dataset, const uint64_t k, const uint64_t limit){

    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".2dbt";
    sdsl::load_from_file(m_block_tree, name_file);

    std::cout  << sdsl::size_in_bytes(m_block_tree) << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(m_block_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(m_block_tree, name_file + ".html");
    /*auto block_size = (uint64_t) std::pow(m_block_tree.k, m_block_tree.height);
    std::vector<std::vector<int64_t >> values;
    auto start_y = 167192;
    m_block_tree.access_region(0,  167192, 7947, 167192, values);
    std::cout << "size: " << values.size() << std::endl;
    for(const auto &r : values){
        if(start_y == 167192){
            std::cout << start_y << " (" << r.size() << ") {";
            for(const auto &v: r){
                std::cout << v << ", ";
            }
            std::cout << "}" << std::endl;
        }
        ++start_y;
    }
    //auto result = m_block_tree.neigh(167192);
    //std::cout << "Result: " << result.size()<< std::endl;

    std::cout << "First level with pointer: " << m_block_tree.first_level_with_pointer() << std::endl;
    m_block_tree.display();*/

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
    }else if (type == "skip_levels_lists"){
        run_load<block_tree_2d::block_tree_intersection_lists<>>(dataset, k, limit);
    }else if (type == "god_level"){
        run_load<block_tree_2d::block_tree_double_hybrid_skipping_block<>>(dataset, k, limit);
    }else{
        std::cout << "Type: " << type << " is not supported." << std::endl;
    }

}