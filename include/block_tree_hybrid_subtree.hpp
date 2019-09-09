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
// Created by Adrián on 09/08/2019.
//

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_HYBRID_SUBTREE_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_HYBRID_SUBTREE_HPP

#include <block_tree.hpp>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree_hybrid_subtree : public block_tree<input_t> {

    private:

        void processing_level(const size_type level, input_type &adjacency_lists, size_type block_size,
                            size_type &topology_index, size_type &is_pointer_index,
                            typename block_tree_2d::algorithm::hash_type hash, std::vector<node_type> &nodes){

            htc_type m_htc(2*nodes.size()); //2* nodes, in order to reduce resize operations
            util::logger::log("Computing fingerprint of blocks at level=" + std::to_string(level));
            block_tree_2d::algorithm::get_fingerprint_blocks(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
            util::logger::log("Computing fingerprint of shifts at level=" + std::to_string(level));
            block_tree_2d::algorithm::get_type_of_nodes(adjacency_lists, k, m_htc, dimensions, block_size, hash, nodes);
            util::logger::log("Clearing adjacency lists at level=" + std::to_string(level));
            block_tree_2d::algorithm::clear_adjacency_lists(adjacency_lists);
            util::logger::log("Compacting level=" + std::to_string(level));
            compact_current_level(nodes, level, topology_index, is_pointer_index);
            util::logger::log("Preparing next level");
            block_tree_2d::algorithm::prepare_next_level(adjacency_lists, hash, m_k2, nodes);
            //std::cout << "Level: " << level << std::endl;
        }


    };
}
#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_HYBRID_SUBTREE_HPP
