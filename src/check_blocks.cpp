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



#include <iostream>
#include <vector>
#include <stdint.h>
#include <dataset_reader.hpp>

int main(int argc, char **argv) {


    if (argc != 7) {
        std::cout << argv[0] << " <dataset> <x1> <y1> <x2> <y2> <block_size>" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    auto x1 = static_cast<uint64_t >(atoi(argv[2]));
    auto y1 = static_cast<uint64_t >(atoi(argv[3]));
    auto x2 = static_cast<uint64_t >(atoi(argv[4]));
    auto y2 = static_cast<uint64_t >(atoi(argv[5]));
    auto block_size = static_cast<uint64_t >(atoi(argv[6]));

    std::vector<std::vector<int64_t>> adjacency_lists, b1, b2;
    dataset_reader::web_graph::read(dataset, adjacency_lists, -1);
    b1.resize(block_size);
    b2.resize(block_size);
    for(uint64_t row = 0; row < block_size; ++row){
        if(y1 + row < adjacency_lists.size()) {
            for (uint64_t i = 0; i < adjacency_lists[y1 + row].size(); ++i) {
                auto v = adjacency_lists[y1 + row][i];
                if (v >= x1 && v < x1 + block_size) {
                    b1[row].push_back(v - x1);
                }
            }
        }
    }
    for(uint64_t row = 0; row < block_size; ++row){
        if(y2 + row < adjacency_lists.size()){
            for(uint64_t i = 0; i < adjacency_lists[y2 + row].size(); ++i){
                auto v = adjacency_lists[y2+row][i];
                if(v >= x2 && v < x2 + block_size){
                    b2[row].push_back(v-x2);
                }
            }
        }

    }

    uint64_t elements = 0;
    for(uint64_t row = 0; row < block_size; ++row){
        if(b1[row].size() != b2[row].size()){
            std::cout << "Are not identical" << std::endl;
            //exit(10);
        }else{
            for(uint64_t i = 0; i < b1[row].size(); ++i){
                ++elements;
                if(b1[row][i] != b2[row][i]){
                    std::cout << "Are not identical" << std::endl;
                   // exit(10);
                }
            }
        }
    }

    std::cout << "Bloque 1" << std::endl;
    for(uint64_t row = 0; row < block_size; ++row){
        std::cout << "Row " << row << ": { ";
        for(uint64_t i = 0; i < b1[row].size(); ++i){
            std::cout << b1[row][i] << ", ";
        }
        std::cout << "}" << std::endl;
    }

    std::cout << "Bloque 2" << std::endl;
    for(uint64_t row = 0; row < block_size; ++row){
        std::cout << "Row " << row << ": { ";
        for(uint64_t i = 0; i < b2[row].size(); ++i){
            std::cout << b2[row][i] << ", ";
        }
        std::cout << "}" << std::endl;
    }
    std::cout << "Are identical" << std::endl;
    std::cout << "Elements: " << elements << std::endl;

}