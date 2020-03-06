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
// Created by Adrián on 05/03/2020.
//

#ifndef INC_2D_BLOCK_TREE_TRIE_HPP
#define INC_2D_BLOCK_TREE_TRIE_HPP

namespace util {


    template <class t_input = uint32_t, class t_value = uint32_t >
    class trie {

    public:
        typedef t_value value_type;
        typedef t_input input_type;
        typedef uint64_t size_type;
        typedef std::unordered_map<input_type, size_type> node_type;
        typedef std::vector<value_type> leaf_type;
        typedef typename std::vector<node_type>::iterator iterator;

    private:
        std::vector<node_type> m_nodes;
        std::vector<leaf_type> m_leaves;
        size_type m_height;
    public:
        trie(size_type h){
            m_nodes.push_back(node_type());
            m_height = h;
        }

        iterator root(){
           return  m_nodes.begin();
        }

        void insert(std::vector<input_type>& input, value_type value){

            size_type ptr = 0;
            size_type depth = 0;
            for(const auto &i : input) {
                auto it = m_nodes[ptr].find(i);
                if (it !=  m_nodes[ptr].end()) {
                    if(depth == m_height-1){
                        m_leaves[it->second].push_back(value);
                    }else{
                        ptr = it->second;
                    }
                } else {
                    if(depth == m_height -1){
                        m_leaves.push_back(leaf_type());
                        m_nodes[ptr].insert({i, m_leaves.size() - 1});
                        m_leaves[m_leaves.size()-1].push_back(value);
                    }else{
                        m_nodes.push_back(node_type());
                        m_nodes[ptr].insert({i, m_nodes.size() - 1});
                        ptr = m_nodes.size()-1;
                    }
                }
                ++depth;
            }
        }


        void print_subtree(size_type ptr, size_type depth){
            if(depth == m_height){
                std::cout << "[";
                for(const auto &l : m_leaves[ptr]){
                    std::cout << l << ", ";
                }
                std::cout << "]" << std::endl;
            }else{
                for(const auto &p : m_nodes[ptr]){
                    std::cout << p.first << "--> ";
                    print_subtree(p.second, depth+1);
                }
            }
        }



    };
}

#endif //INC_2D_BLOCK_TREE_TRIE_HPP
