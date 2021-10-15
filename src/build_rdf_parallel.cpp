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
//#include <block_tree_intersection_lists.hpp>
//#include <block_tree_hybrid.hpp>
//#include <block_tree_double_hybrid.hpp>
#include <block_tree_double_hybrid_skipping_block.hpp>
#include <omp.h>

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

void build(block_tree_2d::block_tree_double_hybrid_skipping_block<dataset_reader::rdf> &b,
           const std::string &file_name,
           const uint64_t k, const uint64_t last_block_size_k2_tree){
    b = block_tree_2d::block_tree_double_hybrid_skipping_block<dataset_reader::rdf>(file_name, k, last_block_size_k2_tree, -1);
    std::cout << "Block tree height=" << b.height << std::endl;
    std::cout << "There are pointers from level " << b.minimum_level+1 << " up to level " << b.maximum_level-1 << std::endl;
}

template<class t_block_tree>
void run_build(const std::string &dataset, const std::string &out_file, const uint64_t k,
               const uint64_t last_block_size_k2_tree){

    std::cout << "Building Block-tree..." << std::endl;
    t_block_tree m_block_tree;
    auto t0 = std::chrono::high_resolution_clock::now();
    build(m_block_tree, dataset, k, last_block_size_k2_tree);
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(t1-t0).count();
    std::string name_file = dataset;
    uint64_t first_block_size = last_block_size_k2_tree / k;

    name_file = name_file +"_" + std::to_string(first_block_size) + ".2dbt";
    std::cout << "Storing the block tree... " << std::flush;
    sdsl::store_to_file(m_block_tree, name_file);
    std::cout << "Done. " << std::endl;
    //m_block_tree.print();
    std::vector<std::vector<int64_t>> copy_lists;
    auto rows_cols = dataset_reader::rdf::read(dataset, copy_lists);
    std::cout << "Retrieving adjacency lists... " << std::flush;
    std::vector<std::vector<int64_t >> result;
    m_block_tree.access_region(0, 0, rows_cols.second-1, rows_cols.first - 1, result);
    std::cout << "Done." << std::endl;
    auto size_bt = sdsl::size_in_bytes(m_block_tree);
    /*std::cout << "--------------------Result--------------------" << std::endl;
    block_tree_2d::algorithm::print_ajdacent_list(result);
    std::cout << "----------------------------------------------" << std::endl;*/
    uint64_t elements = 0;
    std::cout << "Checking results." << std::endl;
    if (result.size() != copy_lists.size()) {
        std::cout << "Error: the number of lists is incorrect." << std::endl;
        std::cout << "Expected: " << copy_lists.size() << std::endl;
        std::cout << "Obtained: " << result.size() << std::endl;
        exit(10);
    }
    bool error = false;
    for (auto i = 0; i < result.size(); ++i) {
        if (!error && result[i].size() != copy_lists[i].size()) {
            std::cout << "Error: the size of list " << i << " is incorrect." << std::endl;
            std::cout << "Expected: " << copy_lists[i].size() << std::endl;
            std::cout << "Obtained: " << result[i].size() << std::endl;
            for(uint64_t o = 0; o < copy_lists[i].size(); ++o){
                std::cout << copy_lists[i][o] << ",";
            }
            std::cout << std::endl;
            for(uint64_t o = 0; o < result[i].size(); ++o){
                std::cout << result[i][o] << ",";
            }
            std::cout << std::endl;
            //m_block_tree.access_region(10, 2, 11, 3, result);
            error = true;
        }
    }
    if (error){
        util::adjacency_list::write(copy_lists, "adjacency_lists.txt");
        exit(10);
    }else{
        for (auto i = 0; i < result.size(); ++i) {
            for (auto j = 0; j < result[i].size(); ++j) {
                if (result[i][j] != copy_lists[i][j]) {
                  //  std::cout << "Error: the " << j << "-th value of list " << i << " is incorrect." << std::endl;
                  //  std::cout << "Expected: " << copy_lists[i][j] << std::endl;
                  //  std::cout << "Obtained: " << result[i][j] << std::endl;
                    error = true;
                }
                ++elements;
            }
        }
        if (!error) {
            std::cout << "Everything is OK!" << std::endl;
        }else{
            util::adjacency_list::write(copy_lists, "adjacency_lists.txt");
            exit(10);
        }
    }
    std::cout << std::endl;
    std::cout << "The Block-tree was built in " << duration << " seconds and uses " << size_bt << " bytes." << std::endl;
    std::cout << duration << " " << size_bt << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(m_block_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(m_block_tree, name_file + ".html");
    std::cout << "Elements: " << elements << std::endl;
    std::ofstream out(out_file);
    out << "The Block-tree was built in " << duration << " seconds and uses " << size_bt << " bytes." << std::endl;
    out.close();
}

int main(int argc, char **argv) {

    if(argc != 6){
        std::cout << argv[0] << "<dataset> <k> <block_size> <first> <last>" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    auto k = static_cast<uint64_t >(atoi(argv[2]));
    auto last_block_size_k2_tree = static_cast<uint64_t >(atoi(argv[3]));
    auto first = static_cast<uint64_t >(atoi(argv[4]));
    auto last = static_cast<uint64_t >(atoi(argv[5]));

    std::vector<std::string> files, outs;
    for(auto i = first; i<=last; ++i){
        files.push_back(dataset + ".hdt.adj." + std::to_string(i));
    }
    for(auto i = first; i<=last; ++i){
        files.push_back(dataset + ".bt." + std::to_string(i) + ".out");
    }

#pragma omp parallel for
    for(auto i=0; i < files.size(); ++i){
        run_build<block_tree_2d::block_tree_double_hybrid_skipping_block<dataset_reader::rdf>>(files[i], outs[i], k,
                last_block_size_k2_tree);
    }
}