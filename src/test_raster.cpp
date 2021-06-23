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
void run_build(const std::string &dataset, const uint64_t k,
               const uint64_t last_block_size_k2_tree, const uint64_t n_rows, const uint64_t n_cols){

    std::cout << "Building Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    uint64_t first_block_size = last_block_size_k2_tree / k;
    std::string name_file = dataset;
    name_file = name_file +"_" + std::to_string(first_block_size) + ".2dbt";
    uint64_t duration = 0;
    if(util::file::file_exists(name_file)){
        sdsl::load_from_file(m_block_tree, name_file);
    }else{
        auto t0 = std::chrono::high_resolution_clock::now();
        build(m_block_tree, dataset, k, last_block_size_k2_tree, n_rows, n_cols);
        auto t1 = std::chrono::high_resolution_clock::now();
        duration = std::chrono::duration_cast<std::chrono::seconds>(t1-t0).count();
    }



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

    //exit(0);
    /*
    n = 0;
    for(int r = 0; r <n_rows; ++r){
        for (int c = 0; c < n_cols; ++c){
            auto v = m_block_tree.get_cell(c, r, n_cols);
            if(n % 10000 == 0) std::cout << n << std::endl;
            if(values[n] != v){
                std::cout << "c=" << c << std::endl;
                std::cout << "r=" << r << std::endl;
                std::cout << "Error en n=" << n << std::endl;
                std::cout << "Obtained=" << v << std::endl;
                std::cout << "Expected=" << values[n] << std::endl;
                std::cout << m_block_tree.get_cell(c, r, n_cols) << std::endl;
                exit(10);
            }
            ++n;
        }
    }*/

    auto lb = 1;
    auto ub = 20;
    auto size = 20;

    auto vals = m_block_tree.region_range(3330, 756,3330 + size-1, 756+size-1, lb, ub, n_cols);
    n = 0;
    for(int r = 0; r + size-1 < n_rows; ++r){
        for(int c = 0; c + size-1 < n_cols; ++c){
            auto vals = m_block_tree.region_range(c, r,c + size-1, r+size-1, lb, ub, n_cols);
            if(n % 10000 == 0) std::cout << n << std::endl;
            for(int oy = 0; oy < size; ++oy){
                for(int ox = 0; ox < size; ++ox){
                    auto n_i = (r+oy) * n_cols +  (c+ox);
                    if(((lb <= values[n_i] && values[n_i] <= ub) && !vals[ox + oy * size]) ||
                        ((lb > values[n_i] || values[n_i] > ub) && vals[ox + oy * size])){
                        std::cout << "Error: r=" << r << " c=" << c << std::endl;
                        std::cout << "ox=" << ox << " oy=" << oy << std::endl;
                        std::cout << "Value: " << values[n_i] << std::endl;
                        std::cout << "Set: " <<  (uint64_t) vals[ox + oy * size] << std::endl;
                        exit(0);
                    }
                }
            }
            ++n;
        }
    }



    std::cout << "Everything OK!" << std::endl;

   //auto pair = m_block_tree.shannon_entropy_bits_delete();
   //std::cout << "Bits entropy: " << pair.first << std::endl;
   //std::cout << "Bits to delete: " << pair.second << std::endl;




}

int main(int argc, char **argv) {

    if(argc != 6){
        std::cout << argv[0] << " <dataset> <k> <last_block_size_k2_tree> <n_rows> <n_cols>" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    auto k = static_cast<uint64_t >(atoi(argv[2]));
    auto last_block_size_k2_tree = static_cast<uint64_t >(atoi(argv[3]));
    auto n_rows = static_cast<uint64_t >(atoi(argv[4]));
    auto n_cols = static_cast<uint64_t >(atoi(argv[5]));

    run_build<block_tree_2d::block_tree_raster<dataset_reader::raster>>(dataset, k, last_block_size_k2_tree, n_rows, n_cols);

}