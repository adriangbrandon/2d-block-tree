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
// Created by Adrián on 8/10/21.
//

#ifndef INC_2D_BLOCK_TREE_RDF_UTIL_HPP
#define INC_2D_BLOCK_TREE_RDF_UTIL_HPP

#include <file_util.hpp>

namespace util{

    namespace rdf {

        struct triplet {
            uint p, s, o;
        };

        bool compareTriplet(const triplet &a, const triplet &b){
            if(a.p != b.p){
                return a.p < b.p;
            }else{
                if(a.s != b.s){
                    return a.s < b.s;
                }else{
                    return a.o < b.o;
                }
            }
        }

        void sort_triplets(const std::string &file){
            std::vector<triplet> triplets;
            util::file::read_from_file(file, triplets);
            std::sort(triplets.begin(), triplets.end(), compareTriplet);
            util::file::write_to_file(file + ".sort", triplets);
        }

        void split(const std::string &header_file, const std::string &sort_file,
                   const std::string &output_file){

            std::vector<uint> header;
            std::vector<triplet> triplets;
            util::file::read_from_file(header_file, header); //4-> np, 5-> ns, 6-> no, 7-> nso, 8-> n
            util::file::read_from_file(sort_file, triplets);
            uint nodes = header[4];
            u_long edges = triplets.size();
            uint prev_p = triplets[0].p;
            uint prev_s = triplets[0].s;
            std::string file_predicate = output_file + "." + std::to_string(triplets[0].p);
            std::ofstream out(file_predicate);
            util::file::write_value(nodes, out);
            util::file::write_value(edges, out);

            int flag_s = -triplets[0].s;
            int flag_o = triplets[0].o;
            util::file::write_value(flag_s, out);
            util::file::write_value(flag_o, out);
            for(u_long i = 1; i < triplets.size(); ++i){
                const auto &t = triplets[i];
                if(prev_p != t.p){
                    out.close();
                    file_predicate = output_file + "." + std::to_string(t.p);
                    out.open(file_predicate);
                    util::file::write_value(nodes, out);
                    util::file::write_value(edges, out);
                    flag_s = -t.s;
                    util::file::write_value(flag_s, out);
                    util::file::write_value(t.o, out);
                }else{
                    if(prev_s != t.s){
                        flag_s = -t.s;
                        util::file::write_value(flag_s, out);
                    }
                    util::file::write_value(t.o, out);
                }
                prev_p = t.p;
                prev_s = t.s;

            }
            out.close();

        }
    }

}

#endif //INC_2D_BLOCK_TREE_RDF_UTIL_HPP
