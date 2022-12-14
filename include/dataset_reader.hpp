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

#ifndef INC_2D_BLOCK_TREE_DATASET_READER_HPP
#define INC_2D_BLOCK_TREE_DATASET_READER_HPP

#include <iostream>
#include <stdint.h>
#include <vector>
#include <sys/stat.h>
#include <fstream>
#include <sdsl/coder.hpp>
#include <adjacency_list_helper.hpp>

namespace dataset_reader {


    class web_graph {

        /***
         * File containing the graph in binary format.
         * This format consists in one integer with
         * the number of nodes, one double with the
         * number of edges, and then the adjacency
         * list of each node, where we mark with a
         * negative number the start of the next list.
         * That is, we write -1, then the neighbors of
         * the first node, then -2, the list of neighbors
         * of the second node, and so on.
         *
         * For instance, the first integers of the cnr-2000 graph in binary format will be:
         * 325557 3216152 0 -1 2 343 344 345 346 347 348 349 350 351 352 211285 223143 -2 3 4 5 320 -3 211285 223143
         * -4 -5 318 -6 -7 118 219 297 -8 7 19 219 286 297 -9 7
         */

    public:
        static void read(const std::string file_name,
                         std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t n_rows = 0,
                         const uint64_t n_cols = 0) {

            struct stat s;
            const char *file_name_c = file_name.c_str();
            if (stat(file_name_c, &s) != 0) {
                std::cout << "Error cannot open file: " << file_name << std::endl;
            }
            FILE *file = fopen(file_name_c, "r");
            uint32_t number_nodes = 0;
            uint64_t number_edges = 0;
            fread(&number_nodes, sizeof(uint32_t), 1, file);
            fread(&number_edges, sizeof(uint64_t), 1, file);
            std::cout << "number_nodes: " << number_nodes << std::endl;
            std::cout << "number_edges: " << number_edges << std::endl;
            std::cout << s.st_size << std::endl;
            uint32_t zero = 0;
            //fread(&zero, sizeof(uint32_t), 1, file);
            uint64_t len_lists = (s.st_size - sizeof(uint32_t) - sizeof(uint64_t)) / sizeof(int32_t);
            int32_t *data = (int32_t *) std::malloc(sizeof(int32_t) * len_lists);
            fread(data, sizeof(uint32_t), len_lists, file);
            adjacency_lists.resize(number_nodes, std::vector<int64_t>());

            int64_t id = -1, number_ones = 0;
            for (uint64_t i = 0; i < len_lists && id < number_nodes; i++) {
                if (data[i] < 0) {
                    id++;
                } else {
                    if (data[i] - 1 < number_nodes) {
                        number_ones++;
                        adjacency_lists[id].push_back(data[i] - 1);
                    }
                }
            }

            std::free(data);
            fclose(file);
            std::cout << "number_ones: " << number_ones << std::endl;
            std::cout << "number_nodes: " << number_nodes << std::endl;
            std::cout << "number_edges: " << number_edges << std::endl;

        }
    };


    class rdf {


    public:
        static std::pair<int, int> read(const std::string file_name,
                                        std::vector<std::vector<int64_t>> &adjacency_lists,
                                        const uint64_t n_rows = 0, const uint64_t n_cols = 0) {

            struct stat s;
            const char *file_name_c = file_name.c_str();
            if (stat(file_name_c, &s) != 0) {
                std::cout << "Error cannot open file: " << file_name << std::endl;
            }
            FILE *file = fopen(file_name_c, "r");
            uint32_t number_nodes = 0;
            uint64_t number_edges = 0;
            fread(&number_nodes, sizeof(uint32_t), 1, file);
            fread(&number_edges, sizeof(uint64_t), 1, file);
            std::cout << "number_nodes: " << number_nodes << std::endl;
            std::cout << "number_edges: " << number_edges << std::endl;
            std::cout << s.st_size << std::endl;
            uint32_t zero = 0;
            //fread(&zero, sizeof(uint32_t), 1, file);
            uint64_t len_lists = (s.st_size - sizeof(uint32_t) - sizeof(uint64_t)) / sizeof(int32_t);
            int32_t *data = (int32_t *) std::malloc(sizeof(int32_t) * len_lists);
            fread(data, sizeof(uint32_t), len_lists, file);
            adjacency_lists.resize(number_nodes, std::vector<int64_t>());
            int max = 0;
            int64_t id = -1, number_ones = 0;
            for (uint64_t i = 0; i < len_lists && id < number_nodes; i++) {
                if (data[i] < 0) {
                    id++;
                } else {
                    number_ones++;
                    adjacency_lists[id].push_back(data[i]);
                    if (data[i] > max) max = data[i];
                }
            }

            std::free(data);
            fclose(file);
            std::cout << "number_ones: " << number_ones << std::endl;
            std::cout << "number_nodes: " << number_nodes << std::endl;
            std::cout << "number_edges: " << number_edges << std::endl;
            return {number_nodes, max + 1};
        }
    };

    class raster {
    public:
        static std::pair<int, int> read(const std::string file_name,
                                        std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t n_rows,
                                        const uint64_t n_cols) {

            std::ifstream input(file_name);
            uint64_t n = 0;
            std::vector<int> values(n_rows * n_cols);
            int max_value = 0;
            int min_value = INT32_MAX;
            int zeroes = 0;
            for (int r = 0; r < n_rows; ++r) {
                for (int c = 0; c < n_cols; ++c) {
                    sdsl::read_member(values[n], input);
                    if (values[n] > 0) {
                        if (values[n] > max_value) max_value = values[n];
                        if (values[n] < min_value) min_value = values[n];
                    } else {
                        ++zeroes;
                    }
                    ++n;
                }
            }
            input.close();

            std::cout << "Min value: " << min_value << std::endl;
            std::cout << "Max value: " << max_value << std::endl;
            std::cout << "Zeroes: " << zeroes << " (" << (zeroes / (double) n) * 100 << "% ) " << std::endl;
            //exit(0);

            auto bit_position = [](uint64_t col, uint64_t value, uint64_t n_cols, uint64_t min_value) {
                return col + (n_cols * (value - min_value));
            };

            //Prepare input for adjacency_lists
            adjacency_lists.resize(n_rows, std::vector<int64_t>());
            int sigma = max_value - min_value + 1;
            n = 0;
            for (int r = 0; r < n_rows; ++r) {
                for (int c = 0; c < n_cols; ++c) {
                    if (values[n] > 0) {
                        for (auto v = values[n]; v <= max_value; ++v) {
                            adjacency_lists[r].push_back(bit_position(c, v, n_cols, min_value));
                        }
                    }
                    ++n;
                }
            }
            for (int r = 0; r < n_rows; ++r) {
                std::sort(adjacency_lists[r].begin(), adjacency_lists[r].end());
            }


            return {min_value, max_value};

        }

    };

    class raster_log {
    public:
        static std::pair<int, int> read(const std::string file_name,
                                        std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t n_rows,
                                        const uint64_t n_cols) {

            std::ifstream input(file_name);
            uint64_t n = 0;
            std::vector<int> values(n_rows * n_cols);
            int msb = 0;
            int zeroes = 0;
            for (int r = 0; r < n_rows; ++r) {
                for (int c = 0; c < n_cols; ++c) {
                    sdsl::read_member(values[n], input);
                    int hi = sdsl::bits::hi(values[n]);
                    if (msb < hi) msb = hi;
                    if (values[n] == 0) ++zeroes;
                    ++n;
                }
            }
            input.close();

            std::cout << "MSB: " << msb << std::endl;
            std::cout << "Zeroes: " << zeroes << " (" << (zeroes / (double) n) * 100 << "% ) " << std::endl;
            //exit(0);

            auto bit_position = [](uint64_t col, uint64_t n_cols, uint64_t ith_bit) {
                return col + (n_cols * ith_bit);
            };

            //Prepare input for adjacency_lists
            adjacency_lists.resize(n_rows, std::vector<int64_t>());
            for (int b = 0; b <= msb; ++b) {
                n = 0;
                for (int r = 0; r < n_rows; ++r) {
                    for (int c = 0; c < n_cols; ++c) {
                        bool set_bit = values[n] & (0x0001 << b);
                        if (set_bit) {
                            adjacency_lists[r].push_back(bit_position(c, n_cols, b));
                        }
                        ++n;
                    }
                }
            }

            for (int r = 0; r < n_rows; ++r) {
                std::sort(adjacency_lists[r].begin(), adjacency_lists[r].end());
            }


            return {n_rows, (msb + 1) * n_cols};

        }

    };

    class samatrix {

    public:
        static void read(const std::string file_name,
                         std::vector<std::vector<int64_t>> &adjacency_lists, const uint64_t n_rows = 0,
                         const uint64_t n_cols = 0) {
            util::adjacency_list::read(adjacency_lists, file_name);
        }
    };


}


#endif //INC_2D_BLOCK_TREE_DATASET_READER_HPP
