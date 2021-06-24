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
#include <experiment_setup.hpp>

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

template <class Container>
void print_container(const Container &c){
    for(const auto &v : c){
        std::cout << v << ", ";
    }
    std::cout << std::endl;
}

void build(block_tree_2d::block_tree_raster<dataset_reader::raster> &b, const std::string &file_name,
           const uint64_t k, const uint64_t last_block_size_k2_tree, const uint64_t n_rows, const uint64_t n_cols){
    b = block_tree_2d::block_tree_raster<dataset_reader::raster>(file_name, k, last_block_size_k2_tree, n_rows, n_cols);
    std::cout << "Block tree height=" << b.height << std::endl;
    std::cout << "There are pointers from level " << b.minimum_level+1 << " up to level " << b.maximum_level-1 << std::endl;
}

template<class t_block_tree>
void run_build(const std::string &dataset, const std::string &queries, const std::string &name_file,
               const uint64_t n_rows, const uint64_t n_cols){

    std::cout << "Building Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    uint64_t duration = 0;
    sdsl::load_from_file(m_block_tree, name_file);


    std::cout << "Storing the block tree... " << std::flush;
    sdsl::store_to_file(m_block_tree, name_file);
    std::cout << "Done. " << std::endl;
    //m_block_tree.print();
    std::vector<std::vector<int64_t>> copy_lists;
    auto size_bt = sdsl::size_in_bytes(m_block_tree);
    std::cout << "Size: " << size_bt << std::endl;


    std::ifstream input(dataset);
    uint64_t n = 0;
    std::vector<int> values(n_rows * n_cols);
    for (int r = 0; r < n_rows; ++r) {
        for (int c = 0; c < n_cols; ++c) {
            sdsl::read_member(values[n], input);
            ++n;
        }
    }
    input.close();

    auto qs = experiments::reader::range(queries);
    auto a = 0;
    for(auto q : qs){
        std::vector<uint64_t > expected;
        auto vals = m_block_tree.region_range_v3(q.min_x, q.min_y, q.max_x, q.max_y,q.lb, q.ub, n_cols);
        std::sort(vals.begin(), vals.end());
        std::cout << q.min_x << " " << q.max_x << " " << q.min_y << " " << q.max_y << " "
        << q.lb << " " << q.ub << std::endl;
        for(int r = q.min_y;  r <= q.max_y; ++r) {
            for (int c = q.min_x; c <= q.max_x; ++c) {
                auto n_i = r * n_cols + c;
                if(q.lb <= values[n_i] && values[n_i] <= q.ub){
                    auto r_i = (r - q.min_y) * (q.max_x-q.min_x+1) + (c - q.min_x);
                    expected.push_back(r_i);
                }
            }
        }
        std::sort(expected.begin(), expected.end());
        if(expected.size() != vals.size()){
            std::cout << "Error different size" << std::endl;
            std::cout << "a: " << a << std::endl;
            std::cout << "Expected" << std::endl;
            print_container(expected);
            std::cout << "Obtained" << std::endl;
            print_container(vals);
            exit(0);
        }else{
            for(int i = 0; i < expected.size(); ++i){
                if(vals[i] != expected[i]){
                    std::cout << "Error at position i: " << i << std::endl;
                    std::cout << "Expected" << std::endl;
                    print_container(expected);
                    std::cout << "Obtained" << std::endl;
                    print_container(vals);
                    exit(0);
                }
            }
        }
        ++a;
    }

    std::cout << "Everything OK!" << std::endl;

   //auto pair = m_block_tree.shannon_entropy_bits_delete();
   //std::cout << "Bits entropy: " << pair.first << std::endl;
   //std::cout << "Bits to delete: " << pair.second << std::endl;




}

int main(int argc, char **argv) {

    if(argc != 6){
        std::cout << argv[0] << " <dataset> <queries> <file_name> <n_rows> <n_cols>" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    std::string queries = argv[2];
    std::string file_name = argv[3];
    auto n_rows = static_cast<uint64_t >(atoi(argv[4]));
    auto n_cols = static_cast<uint64_t >(atoi(argv[5]));

    run_build<block_tree_2d::block_tree_raster<dataset_reader::raster>>(dataset, queries, file_name, n_rows, n_cols);

}