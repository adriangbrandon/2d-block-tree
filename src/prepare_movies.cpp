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
// Created by Adrián on 6/11/21.
//

#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>
#include <file_util.hpp>

std::vector<float> ratings = {0.0, 0.5, 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0};

void split_ratings(const std::string &in_file, const std::string &out_file){

    for(const auto &r : ratings){
        std::ifstream in(in_file);
        std::ofstream out(out_file + "-" + std::to_string(r));

        std::string line;
        std::getline(in, line); //skip header
        std::string u, m, ts, rat;
        int userId, movieId, timestamp, prevUserId=0;
        float rating;
        int elements;

        std::vector<int> users, aux_movies;
        std::vector<std::vector<int>> movies;
        int i = 0, ones = 0;
        while(std::getline(in, line)){
            std::stringstream linestream(line);
            std::string data;
            std::getline(linestream, u, ',');
            std::getline(linestream, m, ',');
            std::getline(linestream, rat, ',');
            std::getline(linestream, ts, ',');

            userId = std::stoi(u);
            movieId = std::stoi(m);
            rating = std::stof(rat);
            if(rating == r){
                if(ones > 0 && prevUserId != userId){
                    users.push_back(prevUserId);
                    movies.emplace_back(aux_movies);
                    aux_movies.clear();
                    ++i;
                }
                aux_movies.push_back(movieId);
                prevUserId = userId;
                ++ones;
            }
        }
        uint32_t number_nodes = prevUserId;
        uint64_t number_ones = ones;

        util::file::write_value(number_nodes, out);
        util::file::write_value(number_ones, out);
        int pu = 0;
        for(i = 0; i < users.size(); ++i){
            for(int32_t j = pu+1; pu < users[i]; ++pu){
                int32_t negative = -j;
                util::file::write_value(negative, out);
            }
            int32_t negative = -users[i];
            util::file::write_value(negative, out);
            util::file::write_vector(movies[i], out);
            pu = users[i];
        }
    }


}

int main(int argc, char **argv) {

    split_ratings(argv[1], argv[2]);
}