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
// Created by Adrián on 06/11/2019.
//


#include <chrono>
#include <vector>
#include <iostream>
#include <sdsl/int_vector.hpp>
#include <sdsl/suffix_arrays.hpp>
#include <intersection_lists.hpp>
#include <trie.hpp>
#include <dataset_reader.hpp>

int main(int argc, char **argv) {


    std::vector<uint32_t > vec;
    vec.resize(10);
    uint32_t* array = new uint32_t[10];
    array[0] = 5;
    array[1] = 8;
    array[2] = 9;
    array[3] = 1;
    array[4] = 2;
    array[5] = 54;
    array[6] = 52;
    array[7] = 51;
    array[8] = 50;
    array[9] = 33;

    std::memmove(vec.data(), array, sizeof(uint32_t)*10);

    for(const auto &v : vec){
        std::cout << v << std::endl;
    }

    //std::vector<std::vector<int64_t>> adjacency_lists;
    //dataset_reader::web_graph::read(argv[1], adjacency_lists);

    /*uint64_t size = 100;
    std::vector<std::vector<uint8_t >> v(size, std::vector<uint8_t>(size));
    for(uint i = 0;  i < v.size(); ++i){
        for(uint j = 0; j < v[i].size(); ++j){
            v[i][j] = rand() % 10+1;
        }
    }


    sdsl::int_vector<8> text(size*size);
    int index = 0;
    for(uint i = 0;  i < v.size(); ++i){
        for(uint j = 0; j < v[i].size(); ++j){
            text[index] = v[i][j];
            ++index;
        }
    }

    sdsl::csa_wt<> fm_index;
    sdsl::construct_im(fm_index, text);

    std::vector<sdsl::int_vector<8>> patterns;
    size_t iterations = 100;
    for(size_t i = 0; i < iterations; ++i){
        sdsl::int_vector<8> pattern = {rand() % 10 +1, rand() % 10 +1};
        patterns.emplace_back(pattern);
    }
    auto t0 = std::chrono::high_resolution_clock::now();
    for(size_t i = 0; i < patterns.size(); ++i){
        sdsl::count(fm_index, patterns[i].begin(), patterns[i].end());
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    auto time = std::chrono::duration_cast<std::chrono::nanoseconds>(t1-t0).count();
    std::cout << "FM-Index: " << time << std::endl;

    auto t2 = std::chrono::high_resolution_clock::now();
    size_t occ = 0;
    for(auto &pattern : patterns){
        for(uint i = 0;  i < v.size(); ++i){
            for(uint j = 0; j < v[i].size(); ++j){
                if(v[i][j] == pattern[0]){
                    if(j+1 < v[i].size()){
                        if(v[i][j+1] == pattern[1]){
                            ++occ;
                        }
                    }else{
                        if(i+1 < v.size()){
                            if(v[i+1][0] == pattern[1]){
                                ++occ;
                            }
                        }
                    }
                }
            }
        }
    }
    auto t3 = std::chrono::high_resolution_clock::now();
    auto time2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t3-t2).count();
    std::cout << "Semantrix: " << time2 << std::endl;*/

    /*int number = 16;
    int size_file = 1048576;
    std::ofstream out("sixteen.txt");
    for(size_t i = 0; i < size_file/4; ++i){
        out.write((char*) &number, sizeof(int));
    }
    out.close();*/

    /*std::ifstream in("out.dat");
    int v;
    while(!in.eof()){
        in.read((char*) &v, sizeof(int));
        std::cout << "v: " << v << std::endl;
    }*/


    /*std::vector<std::pair<uint32_t, uint32_t>> sol;
    std::vector<std::pair<uint32_t, uint32_t>> l0 = {{0,0}, {0, 0}, {4, 0}, {4,0}};
    std::vector<std::pair<uint32_t, uint32_t>> l1 = {{0,0}, {0, 0}, {4, 0}, {6,0}};
    std::vector<std::pair<uint32_t, uint32_t>> l2 = {{0,2}, {2, 2}, {4, 2}, {6,2}};


    //util::intersection_lists_no_reps(l0.begin(), l0.end(), l1.begin(), l1.end(), std::back_inserter(sol));
    util::intersection_lists_no_reps(l0, l1);

    for(const auto &s : l0){
        std::cout << "(" << s.first << ", " << s.second << ")" << std::endl;
    }

    util::trie<> m_trie(4);
    std::vector<uint32_t> v1 = {1,2,3,4};
    std::vector<uint32_t> v2 = {2,1,3,4};

    std::cout << std::endl;
    m_trie.insert(v1, 5);
    m_trie.insert(v1, 7);
    m_trie.insert(v2, 8);

    m_trie.print_subtree(0, 0);

    std::vector<std::unordered_map<uint32_t , uint32_t >> vec_map(10);
    auto map = &vec_map[3];
    map->insert({1,1});
    map = &vec_map[4];
    map->insert({5,5});*/


}