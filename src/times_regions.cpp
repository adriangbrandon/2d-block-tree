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

#include <block_tree_basic.hpp>
#include <time_util.hpp>
#include <experiment_setup.hpp>

template<class t_block_tree>
void run_times(const std::string &name_file, const std::string &queries){

    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    sdsl::load_from_file(m_block_tree, name_file);

    auto qs = experiments::reader::regions(queries);

    std::cout << "Size in bytes 2dbt: " << sdsl::size_in_bytes(m_block_tree) << std::endl;

    auto t0 = util::time::user::now();
    int retrieved = 0;
    for(uint64_t i = 0; i < qs.size(); ++i){
        std::vector<std::vector<int64_t>> r;
        m_block_tree.access_region(qs[i].min_x, qs[i].min_y, qs[i].max_x, qs[i].max_y, r);
    }
    auto t1 = util::time::user::now();

    auto t = util::time::duration_cast<util::time::milliseconds>(t1-t0);

    std::cout << "Queries: " << qs.size() << std::endl;
    std::cout << "Total time(ms): " << t << std::endl;
    std::cout << "Time per query: " << t/qs.size() << std::endl;
    std::cout << "Time per link: " << t/retrieved << std::endl;
}

int main(int argc, char **argv) {

    if(argc != 3){
        std::cout << argv[0] << "<namefile> <queries>" << std::endl;
        return 0;
    }
    std::string name_file = argv[1];
    std::string queries = argv[2];
    run_times<block_tree_2d::block_tree_basic<>>(name_file, queries);


}