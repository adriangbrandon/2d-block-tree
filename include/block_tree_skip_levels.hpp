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

#ifndef INC_2D_BLOCK_TREE_BLOCK_TREE_SKIP_LEVELS_HPP
#define INC_2D_BLOCK_TREE_BLOCK_TREE_SKIP_LEVELS_HPP

#include <block_tree.hpp>

namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>>
    class block_tree_skip_levels : public block_tree<input_t> {

    private:

        std::pair<size_type, size_type> minimum_level(input_type &adjacency_lists){

            size_type depth = 1, pointers = 1;
            size_type rows = adjacency_lists.size() / m_k;
            size_type blocks =  rows * rows;
            size_type block_size = m_k;

            while(depth < height){

                htc_type m_htc(2*blocks);
                util::logger::log("Checking blocks at level=" + std::to_string(level));
                if(block_tree_2d::algorithm::contains_identical_blocks(adjacency_lists, k, m_htc, dimensions, block_size)){
                    return {height - depth, block_size};
                }
                util::logger::log("Checking rolls at level=" + std::to_string(level));
                if(block_tree_2d::algorithm::contains_identical_rolls(adjacency_lists, k, m_htc, dimensions, block_size)){
                    return {height - depth, block_size}
                }

                block_size = block_size * m_k;
                blocks = blocks / m_k2;
                ++depth;
            }
            return {height - depth +1, block_size / m_k}
        }

    public:




    };
}
#endif //INC_2D_BLOCK_TREE_BLOCK_TREE_HYBRID_SUBTREE_HPP
