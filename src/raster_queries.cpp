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
// Created by Adrián on 24/6/21.
//
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {

    if(argc != 8){
        std::cout << argv[0] << "<file_name> <queries> <window> <range> <n_rows> <n_cols> <max_value>" << std::endl;
        return 0;
    }

    auto file_name = argv[1];
    auto queries = static_cast<uint64_t >(atoi(argv[2]));
    auto window = static_cast<uint64_t >(atoi(argv[3]));
    auto range = static_cast<uint64_t >(atoi(argv[4]));
    auto n_rows = static_cast<uint64_t >(atoi(argv[5]));
    auto n_cols = static_cast<uint64_t >(atoi(argv[6]));
    auto max_value = static_cast<uint64_t >(atoi(argv[7]));


    std::ofstream out(file_name);
    out << queries << std::endl;
    for(int i = 0; i < queries; ++i){
        auto min_x = (rand() % (n_cols - window));
        auto min_y = (rand() % (n_rows - window));
        auto max_x = min_x + window-1;
        auto max_y = min_y + window-1;
        auto lb = rand() % (max_value - range) + 1;
        auto ub = lb + range-1;
        out << min_y << " " << max_y << " " << min_x << " " << max_x << " " << lb << " " << ub << std::endl;
    }
    out.close();

}