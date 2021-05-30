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
#include <sdsl/k2_tree.hpp>
#include <block_tree_algorithm_helper.hpp>

template <class Range>
uint64_t code_block(Range &block){
    uint64_t v = 0;
    for(const auto &p : block){
        auto y = p.first % 8;
        auto x = p.second % 8;
        v = v | (1ULL << (y*8 + x));
    }
    return v;
}


template <class input_type, class hash_type>
static uint64_t zeta_order_hash(const input_type &adjacent_lists, const uint64_t k,
                                const uint64_t block_size_stop, hash_type &hash){


    typedef std::tuple<uint64_t , uint64_t, uint64_t,uint64_t> t_part_tuple;
    auto k_2 = k * k;
    uint64_t n = 0;

    //1. Edges z-order
    std::vector<uint64_t> edges_z_order;
    for(uint64_t y = 0; y < adjacent_lists.size(); ++y){
        for(uint64_t x : adjacent_lists[y]){
            edges_z_order.push_back(codes::zeta_order::encode(x, y));
        }
    }

    //2. Sort edges z-order
    std::sort(edges_z_order.begin(), edges_z_order.end());

    uint64_t l = adjacent_lists.size();
    std::queue<t_part_tuple> q;
    q.push(t_part_tuple(0, edges_z_order.size()-1, l/k , 0));

    uint64_t i, j, z_0;
    uint64_t t = k_2;
    uint64_t n_elem = 0, zeroes = k_2 -1;

    //5. Split the front of q into its children
    while (!q.empty()) {
        std::tie(i, j, l, z_0) = q.front();
        q.pop();
        auto elements = l * l;
        for(uint64_t z_child = 0; z_child < k_2; ++z_child){
            auto le = util::search::lower_or_equal_search(i, j, edges_z_order, z_0+elements-1);
            if(le != -1 && edges_z_order[le] >= z_0){
                if(l > block_size_stop){
                    q.push(t_part_tuple(i, le, l/k, z_0));
                }else{
                    //Preparing next level
                    std::vector<std::pair<uint64_t , uint64_t >> block;
                    for(uint64_t ii = i; ii <= j; ++ii){
                        block.emplace_back(codes::zeta_order::decode(edges_z_order[ii]));
                    }
                    uint64_t code = code_block(block);
                    auto it = hash.find(code);
                    if(it != hash.end()){
                        it->second++;
                    }else{
                        hash.insert({code, 1});
                    }
                    ++n;
                }
                i = le + 1;
            }else{
                ++zeroes;
            }
            ++t;
            z_0 += elements;
        }
    }
    return n;

}

int main(int argc, char **argv) {


    if(argc != 4 && argc != 3){
        std::cout << argv[0] << "<dataset> <k> [limit]" << std::endl;
        return 0;
    }

    std::string dataset = argv[1];
    uint64_t k = atoi(argv[2]);
    uint64_t limit = -1;
    if(argc == 4){
        limit = atoi(argv[3]);
    }


    std::vector<std::vector<int64_t>> adjacency_lists;
    dataset_reader::web_graph::read(dataset, adjacency_lists, limit);
    auto h = (uint64_t) std::ceil(std::log(adjacency_lists.size())/std::log(k));
    auto total_size = (uint64_t) std::pow(k, h);
    if(adjacency_lists.size() < total_size){
        adjacency_lists.resize(total_size);
    }
    std::vector<std::tuple<uint64_t, uint64_t>> al;
    for(uint64_t i = 0; i < adjacency_lists.size(); ++i){
        for(uint64_t j = 0; j < adjacency_lists[i].size(); ++j){
            al.push_back({i, adjacency_lists[i][j]});
        }
    }

    std::cout << "Building K2-tree..." << std::endl;

    sdsl::k2_tree<2> k2_tree(al, adjacency_lists.size());
    al.clear();
    std::cout << "The K2-tree was built." << std::endl;
    std::string name_file = dataset;
    if(limit != -1){
        name_file = name_file + "_" + std::to_string(limit);
    }
    name_file = name_file + ".k2t";
    sdsl::store_to_file(k2_tree, name_file);

    std::unordered_map<uint64_t, uint64_t> hash_table;
    uint64_t n = zeta_order_hash(adjacency_lists, 2, 8, hash_table);

    auto k2_tree_bytes = sdsl::size_in_bytes(k2_tree);
    std::cout << "Size in bytes: " << k2_tree_bytes << std::endl;
    //Computing entropy
    double entropy = 0;
    for(const auto &v: hash_table){
        entropy += (v.second/ (double) n) * std::log2(n / (double) v.second);
        //std::cout << std::log2(n / (double) v.second) << std::endl;
    }

    auto topology = k2_tree.get_t();
    uint64_t bsize = adjacency_lists.size()/k;
    uint64_t prev_ones = 1, i = 0;
    while(bsize >= 8){
        uint64_t n_ones = 0;
        auto start = i;
        while(i < start + prev_ones*k*k){
            if(topology[i]) ++n_ones;
            ++i;
        }
        //std::cout << "[" << start << ", " << i << "] n_ones: " << n_ones << " bsize: " << bsize << std::endl;
        prev_ones = n_ones;
        bsize = bsize/k;
    }
    auto bits_to_delete = topology.size()-i + k2_tree.get_l().size();

    topology.resize(i);
    sdsl::rank_support_v<1> m_rank;
    sdsl::util::init_support(m_rank, &topology);


    //auto bits_to_delete = block_tree_2d::algorithm::bits_last_k2_tree(adjacency_lists, 2, 8);
    std::cout << topology.size() + k2_tree.get_l().size() << std::endl;
    auto nh0 = static_cast<uint64_t >(n*std::ceil(entropy));
    uint64_t size_k2_tree_leaves = k2_tree_bytes*8 - bits_to_delete + nh0;
    uint64_t other_option = sdsl::size_in_bytes(topology)*8 + sdsl::size_in_bytes(m_rank)*8 + nh0;
    std::cout << "Entropy: " << entropy << " bits." << std::endl;
    std::cout << "nH0: " << static_cast<uint64_t >(n*std::ceil(entropy)) << " bits." << std::endl;
    std::cout << "Bits to delete: " << bits_to_delete  << " bits." << std::endl;
    std::cout << "Size of k2-tree with leaves compression: " << size_k2_tree_leaves << " bits." << std::endl;
    std::cout << "Size of k2-tree with leaves compression: " << static_cast<uint64_t >(std::ceil(size_k2_tree_leaves/ (double) 8)) << " bytes." << std::endl;
    std::cout << "Other option: " << other_option << " bits." << std::endl;
    std::cout << "Other option: " << static_cast<uint64_t >(std::ceil(other_option/ (double) 8)) << " bytes." << std::endl;
    sdsl::write_structure<sdsl::JSON_FORMAT>(k2_tree, name_file + ".json");
    sdsl::write_structure<sdsl::HTML_FORMAT>(k2_tree, name_file + ".html");



}