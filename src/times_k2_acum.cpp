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

#include <k2_acum.hpp>
#include <time_util.hpp>
#include <experiment_setup.hpp>

template<class t_block_tree>
void run_times(const std::string &name_file, const std::string &queries, const size_t n_cols, const size_t n_rows){

    std::cout << "Loading Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    sdsl::load_from_file(m_block_tree, name_file);

    std::cout << "Size in bytes 2dbt: " << sdsl::size_in_bytes(m_block_tree) << std::endl;

    std::vector<experiments::range_type> qs = experiments::reader::range(queries);


//    m_block_tree.region_range(3504, 2036, 3590, 2503, 116, 267, n_cols);
 //   exit(0);
    std::cout << "Running queries" << std::endl;
    uint64_t sum_x = 0, sum_y = 0, sum_int = 0;
    uint64_t sum = 0;
    auto t0 = util::time::user::now();
    for(int i = 0; i < qs.size(); ++i){
        /*std::cout << qs[i].min_x << ", " << qs[i].min_y << ", " << qs[i].max_x << ", " <<
        qs[i].max_y << ", " << qs[i].lb << ", " << qs[i].ub << std::endl;*/
        auto values = m_block_tree.region_range(qs[i].min_x, qs[i].min_y,
                                                qs[i].max_x, qs[i].max_y,
                                                qs[i].lb, qs[i].ub, n_cols);
        /*sum_x += (qs[i].max_x - qs[i].min_x );
        sum_y += (qs[i].max_y - qs[i].min_y);
        sum_int += (qs[i].ub - qs[i].lb);*/
        sum += values.size();
    }

    /*double avg_x = (sum_x / (double ) qs.size());
    double avg_y = (sum_y / (double ) qs.size());
    double avg_int = (sum_int / (double ) qs.size());

    std::cout << "Media x: " << avg_x << std::endl;
    std::cout << "Media y: " << avg_y << std::endl;
    std::cout << "Media int: " << avg_int << std::endl;*/
    auto t1 = util::time::user::now();

    auto t = util::time::duration_cast<util::time::microseconds>(t1-t0);

    std::cout << "Total: " << sum << std::endl;
    std::cout << "Queries: " << qs.size() << std::endl;
    std::cout << "Total time(micros): " << t << std::endl;
    std::cout << "Time per query: " << t/qs.size() << std::endl;
}

int main(int argc, char **argv) {

    if(argc != 5){
        std::cout << argv[0] << "<namefile> <queries> <n_rows> <n_cols>" << std::endl;
        return 0;
    }
    std::string name_file = argv[1];
    std::string queries = argv[2];
    int n_rows = atoi(argv[3]);
    int n_cols = atoi(argv[4]);
    run_times<block_tree_2d::k2_acum<>>(name_file, queries, n_cols, n_rows);


}