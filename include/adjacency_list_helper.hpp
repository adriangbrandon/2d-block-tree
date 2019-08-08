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
// Created by Adrián on 05/08/2019.
//

#ifndef INC_ADJACENCY_LIST_HELPER_HPP
#define INC_ADJACENCY_LIST_HELPER_HPP

#include <fstream>
#include <iostream>
#include <string>

namespace util {

    class adjacency_list {

        template <class Type>
        static void write_value(const Type &t, std::ofstream &out){
            out.write((char*)&t, sizeof(t));
        }

        template <class Type>
        static void write_vector(const std::vector<Type> &t, std::ofstream &out){
            for(auto i = 0; i < t.size(); ++i){
                write_value(t[i], out);
            }
        }

        template <class Type>
        static void read_value(const Type &t, std::ifstream &in){
            in.read((char*)&t, sizeof(t));
        }

        template <class Type>
        static void read_vector(const std::vector<Type> &t, std::ifstream &in){
            for(auto i = 0; i < t.size(); ++i){
                read_value(t[i], in);
            }

        }

    public:

        template <class Container>
        static void write(const Container &cont, const std::string file){
            std::ofstream out(file);
            write_value((uint64_t) cont.size(), out);
            for(const auto &v : cont){
                write_value((uint64_t) v.size(), out);
                write_vector(v, out);
            }
            out.close();
        }

        template <class Container>
        static void read(Container &cont, const std::string file){
            std::ifstream in(file);
            uint64_t size;
            read_value(size, in);
            cont.resize(size);
            for(uint64_t i = 0; i < cont.size(); ++i){
                read_value(size, in);
                cont[i].resize(size);
                read_vector(cont[i], in);
            }
            in.close();
        }
    };
}

#endif //INC_2D_BLOCK_TREE_ADJACENCY_LIST_HELPER_HPP
