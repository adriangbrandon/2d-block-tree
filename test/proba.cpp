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

int main(int argc, char **argv) {



    uint64_t size = 100;
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
    std::cout << "Semantrix: " << time2 << std::endl;

}