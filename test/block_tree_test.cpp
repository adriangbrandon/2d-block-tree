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
// Created by Adrián on 26/07/2019.
//

#include <iostream>
#include <vector>
#include <block_tree.hpp>
#include <adjacency_list_helper.hpp>

uint64_t number_of_ones(const std::vector<std::vector<int64_t>> &adjacency_lists) {
    uint64_t n_ones = 0;
    for (auto &list : adjacency_lists) {
        n_ones += list.size();
    }
    return n_ones;
}

int main(int argc, char **argv) {


    if(argc != 4){
        std::cout << argv[0] << " <k> <dimensions> <rep>" << std::endl;
        return 0;
    }
    typedef uint64_t hash_type;
    typedef hash_table::hash_table_chainning <hash_type, uint64_t> htc_type;

    uint64_t dimensions = atoi(argv[2]);
    //uint64_t freq = atoi(argv[2]);
    uint64_t k = atoi(argv[1]);
    uint64_t rep = atoi(argv[3]);

    uint64_t freq = dimensions/4;
    std::vector<uint64_t> freqs = {freq};
    while(freq < dimensions){
        freq *= 2;
        freqs.push_back(freq);
    }

    for(const auto &f : freqs) {
        for (uint64_t r = 0; r < rep; ++r) {
            std::vector<std::vector<int64_t>> adjacency_lists(dimensions, std::vector<int64_t>());
            for (auto &list : adjacency_lists) {
                int64_t last = rand() % f;
                while (last < dimensions) {
                    list.push_back(last);
                    last = last + (rand() % f + 1);
                }
            }

            const auto copy_lists = adjacency_lists;
            /*std::cout << "---------------------Input--------------------" << std::endl;
            block_tree_2d::algorithm::print_ajdacent_list(adjacency_lists);
            std::cout << "----------------------------------------------" << std::endl;*/

            std::cout << "Building Block-tree rep=" << r << " freq="<< f << " dimensions=" << dimensions << "....";
            block_tree_2d::block_tree<> m_block_tree(adjacency_lists, k);
            std::cout << "Done." << std::endl;
            //m_block_tree.print();
            std::cout << "Retrieving adjacency lists...";
            std::vector<std::vector<int64_t >> result;
            m_block_tree.access_region(0, 0, dimensions - 1, dimensions - 1, result);
            std::cout << "Done." << std::endl;

            /*std::cout << "--------------------Result--------------------" << std::endl;
            block_tree_2d::algorithm::print_ajdacent_list(result);
            std::cout << "----------------------------------------------" << std::endl;*/

            std::cout << "Checking results." << std::endl;
            if (result.size() != copy_lists.size()) {
                std::cout << "Error: the number of lists is incorrect." << std::endl;
                std::cout << "Expected: " << copy_lists.size() << std::endl;
                std::cout << "Obtained: " << result.size() << std::endl;
                exit(10);
            }
            bool error = false;
            for (auto i = 0; i < result.size(); ++i) {
                if (result[i].size() != copy_lists[i].size()) {
                    std::cout << "Error: the size of list " << i << " is incorrect." << std::endl;
                    std::cout << "Expected: " << copy_lists[i].size() << std::endl;
                    std::cout << "Obtained: " << result[i].size() << std::endl;
                    //m_block_tree.access_region(10, 2, 11, 3, result);
                    error = true;
                }
            }
            if (error){
                std::cout << "---------------------Input--------------------" << std::endl;
                block_tree_2d::algorithm::print_ajdacent_list(copy_lists);
                std::cout << "----------------------------------------------" << std::endl;
                std::cout << "--------------------Result--------------------" << std::endl;
                block_tree_2d::algorithm::print_ajdacent_list(result);
                std::cout << "----------------------------------------------" << std::endl;
                util::adjacency_list::write(copy_lists, "adjacency_lists.txt");
                exit(10);
            }else{
                for (auto i = 0; i < result.size(); ++i) {
                    for (auto j = 0; j < result[i].size(); ++j) {
                        if (result[i][j] != copy_lists[i][j]) {
                            std::cout << "Error: the " << j << "-th value of list " << i << " is incorrect." << std::endl;
                            std::cout << "Expected: " << copy_lists[i][j] << std::endl;
                            std::cout << "Obtained: " << result[i][j] << std::endl;
                            error = true;
                        }
                    }
                }
                if (!error) {
                    std::cout << "Everything is OK!" << std::endl;
                }else{
                    std::cout << "---------------------Input--------------------" << std::endl;
                    block_tree_2d::algorithm::print_ajdacent_list(copy_lists);
                    std::cout << "----------------------------------------------" << std::endl;
                    std::cout << "--------------------Result--------------------" << std::endl;
                    block_tree_2d::algorithm::print_ajdacent_list(result);
                    std::cout << "----------------------------------------------" << std::endl;
                    util::adjacency_list::write(copy_lists, "adjacency_lists.txt");
                    exit(10);
                }
            }
            std::cout << std::endl;


            /*m_block_tree.access_region(2, 2, 5, 5, result);
            block_tree_2d::algorithm::print_ajdacent_list(result);*/

        }

    }

}