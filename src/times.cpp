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
// Created by Adrián on 22/09/2019.
//

#include <block_tree.hpp>
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>
#include <sdsl/k2_tree.hpp>
#include <chrono>
#include <block_tree_double_hybrid_skipping_block.hpp>

template<class t_block_tree>
void run_times(const std::string &dataset, const uint64_t k, const uint64_t limit){

    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".2dbt";
    sdsl::load_from_file(m_block_tree, name_file);

    std::cout << "Loading k2-tree..." << std::endl;
    sdsl::k2_tree<2> m_k2_tree;
    sdsl::load_from_file(m_k2_tree, dataset+".k2t");

    std::cout << "Size in bytes 2dbt: " << sdsl::size_in_bytes(m_block_tree) << std::endl;
    std::cout << "Size in bytes k2t: " << sdsl::size_in_bytes(m_k2_tree) << std::endl;

    auto t0 = std::chrono::high_resolution_clock::now();
    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_block_tree.neigh(id);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_k2_tree.neigh(id);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_block_tree.reverse_neigh(id);
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_k2_tree.reverse_neigh(id);
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    auto t_dn_2dbt = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
    auto t_dn_k2t = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    auto t_rn_2dbt = std::chrono::duration_cast<std::chrono::milliseconds>(t3-t2).count();
    auto t_rn_k2t = std::chrono::duration_cast<std::chrono::milliseconds>(t4-t3).count();

    std::cout << "Direct neighbors 2dbt: " << t_dn_2dbt << " (ms)" << std::endl;
    std::cout << "Direct neighbors k2t: " << t_dn_k2t << " (ms)" << std::endl;
    std::cout << "Reverse neighbors 2dbt: " << t_rn_2dbt << " (ms)" << std::endl;
    std::cout << "Reverse neighbors k2t: " << t_rn_k2t << " (ms)" << std::endl;
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

    if (type == "god_level"){
        run_times<block_tree_2d::block_tree_double_hybrid_skipping_block<>>(dataset, k, limit);
    }else{
        std::cout << "Type: " << type << " is not supported." << std::endl;
    }

}