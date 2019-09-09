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
// Created by Adrián on 09/09/2019.
//

#include <stdint.h>
#include <vector>
#include <block_tree_algorithm_helper_v2.hpp>

int main(int argc, char **argv) {


    std::vector<std::vector<int64_t>> adjacency_lists(16);
    adjacency_lists[0] = {0, 1, 4, 7};
    adjacency_lists[1] = {0, 1, 7};
    adjacency_lists[2] = {0, 8};
    adjacency_lists[3] = {1, 9};
    adjacency_lists[4] = {0, 11};

    sdsl::bit_vector t;
    block_tree_2d::algorithm::build_k2_tree(adjacency_lists, 2, 4, 1, t);

    std::cout << "T: ";
    for(size_t i = 0; i < t.size(); ++i){
        std::cout << t[i] << ", ";
    }
    std::cout << std::endl;
}