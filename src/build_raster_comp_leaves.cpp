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
// Created by Adrián on 06/08/2019.
//
#include <dataset_reader.hpp>
#include <adjacency_list_helper.hpp>
#include <sdsl/io.hpp>
#include <block_tree_comp_leaves.hpp>
#include <file_util.hpp>


template<class t_block_tree>
void load_from(const std::string &file_name, const std::string &dataset,
               const uint64_t n_rows, const uint64_t n_cols){

    t_block_tree m_block_tree;
    std::ifstream in(file_name);
    m_block_tree.from(in);
    auto size_bt = sdsl::size_in_bytes(m_block_tree);
    std::cout << "Size: " << size_bt << std::endl;

    std::ifstream input(dataset);
    uint64_t n = 0;
    std::vector<int> values(n_rows * n_cols);
    int msb = 0;
    int zeroes = 0;
    for (int r = 0; r < n_rows; ++r) {
        for (int c = 0; c < n_cols; ++c) {
            sdsl::read_member(values[n], input);
            int hi = sdsl::bits::hi(values[n]);
            if(msb < hi) msb = hi;
            if (values[n] == 0) ++zeroes;
            ++n;
        }
    }
    input.close();

    /*std::cout << values[0 * n_cols + 5759] << ", " << values[0 * n_cols + 5760] << ", " << values[0 * n_cols + 0] << ", " << values[0 * n_cols + 1] << std::endl;
    std::cout << values[1 * n_cols + 5759] << ", " << values[1 * n_cols + 5760] << ", " << values[1 * n_cols + 0] << ", " << values[1 * n_cols + 1] << std::endl;
    std::cout << values[2 * n_cols + 5759] << ", " << values[2 * n_cols + 5760] << ", " << values[2 * n_cols + 0] << ", " << values[2 * n_cols + 1] << std::endl;
    std::cout << values[3 * n_cols + 5759] << ", " << values[3 * n_cols + 5760] << ", " << values[3 * n_cols + 0] << ", " << values[3 * n_cols + 1] << std::endl;
    std::cout << std::endl;
    std::cout << (values[0 * n_cols + 5759] & 0x0002) << ", " << (values[0 * n_cols + 5760] & 0x0002) << ", " << (values[0 * n_cols + 0] & 0x0004) << ", " << (values[0 * n_cols + 1] & 0x0004) << std::endl;
    std::cout << (values[1 * n_cols + 5759] & 0x0002) << ", " << (values[1 * n_cols + 5760] & 0x0002) << ", " << (values[1 * n_cols + 0] & 0x0004) << ", " << (values[1 * n_cols + 1] & 0x0004)<< std::endl;
    std::cout << (values[2 * n_cols + 5759] & 0x0002) << ", " << (values[2 * n_cols + 5760] & 0x0002) << ", " << (values[2 * n_cols + 0] & 0x0004) << ", " << (values[2 * n_cols + 1] & 0x0004)<< std::endl;
    std::cout << (values[3 * n_cols + 5759] & 0x0002) << ", " << (values[3 * n_cols + 5760] & 0x0002) << ", " << (values[3 * n_cols + 0] & 0x0004) << ", " << (values[3 * n_cols + 1] & 0x0004) << std::endl;
*/
    std::vector<int> result;
    std::cout << "Retrieving adjacency lists... " << std::flush;
    m_block_tree.values_region(0, 0, n_cols-1, n_rows-1, n_cols, n_rows, result);
    std::cout << "Done." << std::endl;
    /*for(int r = 0; r < n_rows; ++r) {
        for (int i = 0; i < result[r].size(); ++i) {
            auto x_bit = result[r][i];
            auto b = x_bit / n_cols;
            auto c = x_bit % n_cols;
            n = n_cols * r + c;
            values2[n] = values2[n] | (0x0001 << b);
        }
    }*/


    for(n = 0; n < n_rows*n_cols; ++n){
        if(result[n] != values[n]){
            std::cout << "Error en n=" << n << std::endl;
            std::cout << "Obtained=" << result[n] << std::endl;
            std::cout << "Expected=" << values[n] << std::endl;
            exit(10);
        }
    }

   /* n = 0;
    for (int r = 0; r < n_rows; ++r) {
        for (int c = 0; c < n_cols; ++c) {
            m_block_tree.values_region(c, r, c, r, n_cols, n_rows, result);
            if(result[0] != values[n]){
                std::cout << "Error en n=" << n << std::endl;
                std::cout << "Obtained=" << result[n] << std::endl;
                std::cout << "Expected=" << values[n] << std::endl;
                exit(10);
            }
            ++n;
        }
    }*/

    std::cout << "Storing the block tree... " << std::flush;
    sdsl::store_to_file(m_block_tree, file_name + ".cl");
    std::cout << "Done. " << std::endl;

}


int main(int argc, char **argv) {

    std::string file_name = argv[1];
    std::string dataset = argv[2];
    int n_rows = atoi(argv[3]);
    int n_cols = atoi(argv[4]);

    load_from<block_tree_2d::block_tree_comp_leaves<dataset_reader::raster>>(file_name, dataset, n_rows, n_cols);

}