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

namespace dataset_reader{


    class web_graph{

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
         * 325557 3216152 0 - 1 2 343 344 345 346 347 348 349 350 351 352 211285 223143 -2 3 4 5 320 -3 211285 223143
         * -4 -5 318 -6 -7 118 219 297 -8 7 19 219 286 297 -9 7
         */

        public:
        static void read(const std::string file_name,
                         std::vector<std::vector<int64_t>> &adjacency_lists, uint64_t limit){

            struct stat s;
            const char* file_name_c = file_name.c_str();
            if(stat(file_name_c, &s) != 0){
                std::cout << "Error cannot open file: " << file_name << std::endl;
            }
            FILE* file = fopen(file_name_c, "r");
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
            int32_t* data = (int32_t*) std::malloc(sizeof(int32_t) * len_lists);
            fread(data, sizeof(uint32_t), len_lists, file);
            if(limit != -1){
                number_nodes = limit;
            }
            adjacency_lists.resize(number_nodes, std::vector<int64_t>());
            uint64_t id = 0, number_ones = 0;
            for(uint64_t i = 0; i < len_lists && id < number_nodes; i++){
                if(data[i]<0){
                    id++;
                }else{
                    if(data[i]-1 < number_nodes){
                        number_ones++;
                        adjacency_lists[id].push_back(data[i]-1);
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
}


#endif //INC_2D_BLOCK_TREE_DATASET_READER_HPP
