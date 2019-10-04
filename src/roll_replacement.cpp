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



#include <iostream>
#include <vector>
#include <stdint.h>
#include <dataset_reader.hpp>
#include <block_tree_algorithm_helper_v2.hpp>
#include <logger.hpp>
#include <block_tree.hpp>

int main(int argc, char **argv) {

    typedef std::vector<std::vector<int64_t>> input_type;
    typedef typename block_tree_2d::block_tree<input_type>::value_type value_type;
    typedef typename block_tree_2d::block_tree<input_type>::size_type size_type;
    typedef typename block_tree_2d::block_tree<input_type>::node_type node_type;
    typedef typename block_tree_2d::block_tree<input_type>::htc_type htc_type;
    typedef typename block_tree_2d::block_tree<input_type>::htc_multiple_type htc_multiple_type;
    typedef typename block_tree_2d::algorithm::hash_type hash_type;
    typedef typename block_tree_2d::algorithm::diff_cord_type diff_cord_type;
    typedef typename block_tree_2d::algorithm::point_type point_type;
    typedef typename block_tree_2d::algorithm::replacements_list_type replacements_list_type;
    typedef typename block_tree_2d::algorithm::replacements_map_type replacements_map_type;
    typedef typename block_tree_2d::algorithm::sources_map_type sources_map_type;
    typedef typename block_tree_2d::algorithm::blocks_replace_map_type blocks_replace_map_type;
    //typedef typename std::unordered_map<point_type, bool, pair_hash> replacements_map_type;
    typedef typename replacements_map_type::iterator replacements_map_iterator;
    class compare_abs_value {
    public:
        bool operator()(const int64_t &x, const int64_t &y){
            return std::abs(x) < std::abs(y);
        }
    };



    if (argc != 2) {
        std::cout << argv[0] << " <dataset> <block_size>" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];

    size_type large_block_size = 2;
    size_type block_size = 4096;
    size_type level = 10;
    size_type k = 2;

    std::vector<std::vector<int64_t>> adjacency_lists, b1, b2;
    dataset_reader::web_graph::read(dataset, adjacency_lists, -1);

    size_type h = (size_type) std::ceil(std::log(adjacency_lists.size())/std::log(k));
    size_type total_size = (size_type) std::pow(k, h);
    if(adjacency_lists.size() < total_size){
        adjacency_lists.resize(total_size);
    }

    block_tree_2d::algorithm::hash_type hash;
    sdsl::bit_vector topology;
    std::vector<node_type> nodes(k*k);
    //Init map between z_order and position in vector nodes
    for(uint64_t i = 0; i < k*k; ++i){
        hash.insert({i,i});
    }
    block_tree_2d::algorithm::build_k2_tree(adjacency_lists, k, h, 65536, topology, hash);

    htc_multiple_type m_htc_multiple(std::min(static_cast<size_type>(10240), static_cast<size_type>(2 * nodes.size())));
    block_tree_2d::algorithm::replacements_map_type replacements_map;
    block_tree_2d::algorithm::sources_map_type sources_map;
    block_tree_2d::algorithm::blocks_replace_map_type blocks_replace_map;

    util::logger::log("Checking blocks at level=" + std::to_string(level) + " with block_size=" +
                      std::to_string(large_block_size));
    block_tree_2d::algorithm::list_blocks(adjacency_lists, k, m_htc_multiple, adjacency_lists.size(),
                                          large_block_size, replacements_map);
    util::logger::log("Checking rolls at level=" + std::to_string(level) + " with block_size=" +
                      std::to_string(large_block_size));
    block_tree_2d::algorithm::list_rolls(adjacency_lists, k, m_htc_multiple, adjacency_lists.size(),
                                         large_block_size, replacements_map);

    util::logger::log("Replacements with block_size=" +std::to_string(block_size));
    block_tree_2d::algorithm::update_replacements(adjacency_lists, replacements_map, block_size, large_block_size,
                                                  adjacency_lists.size(), k);

    //Building sources hashtable
    for(const auto &r : replacements_map){
        auto x_target = r.first.first;
        auto y_target = r.first.second;
        auto z_order = codes::zeta_order::encode(x_target / block_size, y_target/block_size);
        //blocks_replace_map.insert({z_order, 1});
        auto pos_source = hash.find(z_order)->second;
        nodes[pos_source].type = NODE_INTERNAL;
        nodes[pos_source].z_order = z_order;
        for(const auto &s : r.second){
            size_type x_src = x_target + s.first;
            size_type y_src = y_target + s.second;
            auto it = sources_map.find({x_src, y_src});
            if(it != sources_map.end()){
                it->second.push_back(z_order);
            }else{
                std::vector<size_type> z_vec = {z_order};
                sources_map.insert({{x_src, y_src}, z_vec});
            }
        }
    }
    replacements_map.clear();

    std::unordered_map<size_type, char> untouchable_block;
    block_tree_2d::algorithm::get_block_replacements(adjacency_lists, k, sources_map,
                                                     adjacency_lists.size(), block_size, hash, nodes, untouchable_block);
    block_tree_2d::algorithm::get_roll_replacements_v2(adjacency_lists, k, sources_map,
                                                       adjacency_lists.size(), block_size, hash, nodes, untouchable_block);
    block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);


}