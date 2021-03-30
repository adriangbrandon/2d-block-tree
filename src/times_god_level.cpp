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
// Created by Adrián on 03/06/2020.
//

#include <block_tree_double_hybrid_skipping_block.hpp>
#include <time_util.hpp>
template<class t_block_tree>
void run_times(const std::string &name_file){

    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    sdsl::load_from_file(m_block_tree, name_file);

    std::cout << "Size in bytes 2dbt: " << sdsl::size_in_bytes(m_block_tree) << std::endl;

    auto t0 = util::time::user::now();
    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_block_tree.neigh(id);
    }
    auto t1 = util::time::user::now();
    for(uint64_t id = 0; id < m_block_tree.dimensions; ++id){
        auto r = m_block_tree.reverse_neigh(id);
    }
    auto t2 = util::time::user::now();

    auto t_dn_2dbt = util::time::duration_cast<util::time::milliseconds>(t1-t0);
    auto t_rn_2dbt = util::time::duration_cast<util::time::milliseconds>(t2-t1);

    std::cout << "Direct neighbors 2dbt: " << t_dn_2dbt << " (ms)" << std::endl;
    std::cout << "Reverse neighbors 2dbt: " << t_rn_2dbt << " (ms)" << std::endl;
}

int main(int argc, char **argv) {

    if(argc != 2){
        std::cout << argv[0] << "<namefile>" << std::endl;
        return 0;
    }
    std::string name_file = argv[1];
    run_times<block_tree_2d::block_tree_double_hybrid_skipping_block<>>(name_file);


}