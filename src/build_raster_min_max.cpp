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
#include <block_tree_raster.hpp>
#include <file_util.hpp>

/*template<class t_block_tree>
void build(t_block_tree &b, std::vector<std::vector<int64_t>> adjacency_lists, const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = t_block_tree(adjacency_lists, k);
}*/

/*
void build(block_tree_2d::block_tree_hybrid<> &b, std::vector<std::vector<int64_t>> adjacency_lists, const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = block_tree_2d::block_tree_hybrid<>(adjacency_lists, k, last_block_size_k2_tree);
    std::cout << "Block tree height=" << b.height << std::endl;
}*/

/*void build(block_tree_2d::block_tree_double_hybrid<> &b, std::vector<std::vector<int64_t>> adjacency_lists, const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = block_tree_2d::block_tree_double_hybrid<>(adjacency_lists, k, last_block_size_k2_tree);
    std::cout << "Block tree height=" << b.height << std::endl;
    std::cout << "There are pointers from level " << b.minimum_level+1 << " up to level " << b.maximum_level-1 << std::endl;
}*/

void build(block_tree_2d::block_tree_raster<dataset_reader::raster> &b, const std::string &file_name,
           const uint64_t k, const uint64_t last_block_size_k2_tree, const uint64_t n_rows, const uint64_t n_cols){
    b = block_tree_2d::block_tree_raster<dataset_reader::raster>(file_name, k, last_block_size_k2_tree, n_rows, n_cols);
    std::cout << "Block tree height=" << b.height << std::endl;
    std::cout << "There are pointers from level " << b.minimum_level+1 << " up to level " << b.maximum_level-1 << std::endl;
}

template<class t_block_tree>
void run_build(const std::string &file_name, const std::string &dataset, const uint64_t n_rows, const uint64_t n_cols){

    std::cout << "Building Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    std::ifstream in(file_name);
    m_block_tree.load_v1(in);
    std::ifstream input(dataset);
    uint64_t n = 0;
    std::vector<int> values(n_rows * n_cols);
    int max_value = 0;
    int min_value = INT32_MAX;
    int zeroes = 0;
    for (int r = 0; r < n_rows; ++r) {
        for (int c = 0; c < n_cols; ++c) {
            sdsl::read_member(values[n], input);
            if(values[n]>0){
                if(values[n] > max_value) max_value = values[n];
                if(values[n] < min_value) min_value = values[n];
            }else{
                ++zeroes;
            }
            ++n;
        }
    }
    input.close();
    m_block_tree.set_min_max(min_value, max_value);
    std::cout << "Storing the block tree... " << std::flush;
    sdsl::store_to_file(m_block_tree, file_name);
    std::cout << "Done. " << std::endl;
}

int main(int argc, char **argv) {

    if(argc != 5){
        std::cout << argv[0] << "<index> <dataset> <n_rows> <n_cols>" << std::endl;
        return 0;
    }

    std::string file_name = argv[1];
    std::string dataset = argv[2];
    auto n_rows = static_cast<uint64_t >(atoi(argv[3]));
    auto n_cols = static_cast<uint64_t >(atoi(argv[4]));

    run_build<block_tree_2d::block_tree_raster<dataset_reader::raster>>(file_name, dataset, n_rows, n_cols);

}