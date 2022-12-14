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
#include <block_tree_raster_comp_leaves.hpp>
#include <file_util.hpp>


template<class t_block_tree>
void load_from(const std::string &file_name, const std::string &dataset,
               const uint64_t n_rows, const uint64_t n_cols){

    t_block_tree m_block_tree;
    std::ifstream in(file_name);
    m_block_tree.from(in);
    auto size_bt = sdsl::size_in_bytes(m_block_tree);
    std::cout << "Size: " << size_bt << std::endl;

    std::cout << "Storing the block tree... " << std::flush;
    std::string name_file = file_name + ".cl";
    sdsl::store_to_file(m_block_tree, name_file);
    std::cout << "Done. " << std::endl;
    //m_block_tree.print();
    /*std::cout << "--------------------Result--------------------" << std::endl;
    block_tree_2d::algorithm::print_ajdacent_list(result);
    std::cout << "----------------------------------------------" << std::endl;*/

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
    m_block_tree.get_cell(3186, 765, n_cols);
    std::cout << "Retrieving cells... " << std::flush;
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
    }
    std::cout << std::endl;
    std::cout << "The Block-tree uses " << size_bt << " bytes." << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(m_block_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(m_block_tree, name_file + ".html");

}


int main(int argc, char **argv) {

    std::string file_name = argv[1];
    std::string dataset = argv[2];
    int n_rows = atoi(argv[3]);
    int n_cols = atoi(argv[4]);

    load_from<block_tree_2d::block_tree_raster_comp_leaves<dataset_reader::raster>>(file_name, dataset, n_rows, n_cols);

}