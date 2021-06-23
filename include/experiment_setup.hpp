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
// Created by Adrián on 15/4/21.
//

#ifndef INC_2D_BLOCK_TREE_EXPERIMENT_READER_HPP
#define INC_2D_BLOCK_TREE_EXPERIMENT_READER_HPP

#include <string>
#include <vector>
#include <fstream>

namespace experiments {

    typedef struct {
        int id;
        int id_direct;
    } pair_list_type;

    typedef struct {
        int x;
        int y;
    } cell_type;

    typedef struct {
        int min_x;
        int max_x;
        int min_y;
        int max_y;
        int lb;
        int ub;
    } range_type;

    class reader {

    public:
        static std::vector<int> neighbors(const std::string &file_neighbors){
            std::ifstream input(file_neighbors);
            std::vector<int> neighs;
            int n;
            while(1){
                input >> n;
                if(!input.good()) break;
                neighs.push_back(n);
            }
            input.close();
            return neighs;
        }

        static std::vector<pair_list_type> access(const std::string &file_neighbors){
            std::ifstream input(file_neighbors);
            std::vector<pair_list_type> neighs;
            int id, id_list;
            while(1){
                input >> id >> id_list;
                if(!input.good()) break;
                pair_list_type p{id, id_list};
                neighs.push_back(p);
            }
            input.close();
            return neighs;
        }

        static std::vector<cell_type> cells(const std::string &file_queries){
            std::ifstream input(file_queries);
            std::vector<cell_type> cells;
            int x, y;
            input >> x;
            while(1){
                input >> y >> x;
                if(!input.good()) break;
                cell_type p{x, y};
                cells.push_back(p);
            }
            input.close();
            return cells;
        }

        static std::vector<range_type> range(const std::string &file_queries){
            std::ifstream input(file_queries);
            std::vector<range_type> ranges;
            int min_x, min_y, max_x, max_y, lb, ub;
            input >> min_x;
            while(1){
                input >> min_x >> max_x >> min_y >> max_y >> lb >> ub;
                if(!input.good()) break;
                range_type p{min_x, max_x, min_y, max_y, lb, ub};
                ranges.push_back(p);
            }
            input.close();
            return ranges;
        }

    };

    class writer {

    public:
        static void neighbors(const std::string &file_neighbors, const int size, const int max){
            std::ofstream output(file_neighbors);
            srand (time(NULL));
            for(int i = 0; i < size; ++i){
               int n = rand() % max;
               output << n << std::endl;
            }
            output.close();
        }

        static void access(const std::string &file_neighbors, const int size, const int max){
            std::ofstream output(file_neighbors);
            srand (time(NULL));
            for(int i = 0; i < size; ++i){
                int n = rand() % max;
                int a = rand() % max;
                output << n << " " << a << std::endl;
            }
            output.close();
        }

    };

}
#endif //INC_2D_BLOCK_TREE_EXPERIMENT_READER_HPP
