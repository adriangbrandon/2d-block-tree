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
// Created by Adrián on 1/1/22.
//
#include <adjacency_list_helper.hpp>
#include <block_tree_double_hybrid_skipping_block.hpp>

int main(int argc, char **argv) {

    uint64_t n = 9;
    std::vector<std::vector<int64_t>> adjacency_lists(n, std::vector<int64_t>());
    /*for(uint64_t i = 0; i < n; ++i){
        adjacency_lists[i].push_back(i);
        auto j = 2*i+1;
        while(j < n){
            adjacency_lists[i].push_back(j);
            j = j + i+1;
        }
    }*/
    adjacency_lists[0].push_back(0);
    adjacency_lists[0].push_back(3);
    adjacency_lists[1].push_back(1);
    adjacency_lists[1].push_back(2);
    adjacency_lists[2].push_back(3);
    adjacency_lists[3].push_back(4);
    adjacency_lists[4].push_back(6);
    adjacency_lists[5].push_back(0);
    adjacency_lists[8].push_back(2);

    //adjacency_lists[2].push_back(0);
    //adjacency_lists[2].push_back(2);
    //adjacency_lists[3].push_back(1);
    //adjacency_lists[3].push_back(2);
    //adjacency_lists[4].push_back(2);
    //adjacency_lists[7].push_back(7);
    /*adjacency_lists[1].push_back(1);
    adjacency_lists[1].push_back(2);
    adjacency_lists[2].push_back(1);
    adjacency_lists[2].push_back(2);
    adjacency_lists[3].push_back(5);
    adjacency_lists[3].push_back(6);
    adjacency_lists[4].push_back(5);
    adjacency_lists[4].push_back(6);*/

    util::adjacency_list::write(adjacency_lists, "data.txt");

    block_tree_2d::block_tree_double_hybrid_skipping_block<dataset_reader::samatrix> bt =
            block_tree_2d::block_tree_double_hybrid_skipping_block<dataset_reader::samatrix>("data.txt", 2, 8, 0);





}