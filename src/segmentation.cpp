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
// Created by Adrián on 26/03/2020.
//

#include <adjacency_list_helper.hpp>
#include <vector>


uint64_t n_ones_bloque(uint64_t col, const  std::vector<std::vector<int64_t>> &adjacency_lists){
    uint64_t n_ones = 0;
    for(uint64_t i = 255; i < 255+64; ++i){
        for(const auto &v : adjacency_lists[i]){
            if(v < 0) continue;
            if(v >= col + 64) break;
            if(v >= col) n_ones++;
        }
    }
    return n_ones;

}


int main(int argc, char **argv) {

    std::string file_name = argv[1];
    std::vector<std::vector<int64_t>> adjacency_lists;
    util::adjacency_list::read(adjacency_lists, file_name);

    for(uint64_t i = 255; i < 255+64; ++i){
        std::cout<< "List " << i << ": ";
        for(const auto &v : adjacency_lists[i]){
            if(v < 84) std::cout << v << ", ";
        }
        std::cout << std::endl;
    }

    for(uint64_t col = 0; col < 85; ++col){
        std::cout << "Bloque col: " << col << " n_ones: " << n_ones_bloque(col, adjacency_lists) << std::endl;
    }





}