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
// Created by Adrián on 29/03/2020.
//

#ifndef INC_2D_BLOCK_TREE_k2_tree_compression_leaves_HPP
#define INC_2D_BLOCK_TREE_k2_tree_compression_leaves_HPP

#define BITMAP_RESIZE 512
#define ARRAY_RESIZE 64
#define BT_VERBOSE 0

#include <sdsl/bit_vectors.hpp>
#include <algorithm>
#include <result_util.hpp>
#include <zeta_order.hpp>
#include <search_util.hpp>
#include <dac_vector.hpp>
#include <unordered_map>


namespace block_tree_2d {

    template <class input_t = std::vector<std::vector<int64_t>>, uint8_t leaf_size = 8, uint8_t voc_word = 64>
    class k2_tree_compression_leaves {

    public:

        typedef int64_t value_type;
        typedef uint64_t size_type;
        typedef input_t input_type;

    private:

        size_type m_k;
        size_type m_k2;
        size_type m_dimensions;
        size_type m_height;
        size_type m_ones_prev_leaves;
        sdsl::bit_vector m_t;
        sdsl::rank_support_v5<1,1> m_t_rank;
        sdsl::int_vector<voc_word> m_voc;
        sdsl::dac_vector_dp_opt<> m_l;

        void copy(const k2_tree_compression_leaves &p){
            m_k = p.m_k;
            m_k2 = m_k*m_k;
            m_height = p.m_height;
            m_dimensions = p.m_dimensions;
            m_t = p.m_t;
            m_t_rank = p.m_t_rank;
            m_t_rank.set_vector(&m_t);
            m_voc = p.m_voc;
            m_l = p.m_l;
            m_ones_prev_leaves = p.m_ones_prev_leaves;
        }


        template <class Container>
        inline void check_resize(Container&& cont, const size_type pos) {
            if(pos >= cont.size()){
                cont.resize(cont.size()*2);
            }
        }


        template <class List>
        uint64_t code_block(const List &block, const size_type s, const size_type e){
            uint64_t v = 0;
            for(size_type i = s; i <= e; ++i){
                auto p = codes::zeta_order::decode(block[i]);
                auto x = p.first % leaf_size;
                auto y = p.second % leaf_size;
                v = v | (1ULL << (y * leaf_size + x));
            }
            return v;
        }

        void init_construction(size_type &total_size, input_type &adjacency_lists, const size_type kparam){
            m_k = kparam;
            m_dimensions = adjacency_lists.size();
            m_k2 = m_k*m_k;
            m_height = (size_type) std::ceil(std::log(m_dimensions)/std::log(m_k));
            total_size = (size_type) std::pow(m_k, m_height);
            if(adjacency_lists.size() < total_size){
                adjacency_lists.resize(total_size);
            }
        }

        void init_structure(){
            m_t = sdsl::bit_vector(BITMAP_RESIZE, 0);
            m_l = sdsl::bit_vector(BITMAP_RESIZE, 0);
        }

        template <class input_type>
        void construction(input_type &adjacent_lists, const size_type k){


            typedef std::tuple<size_type , size_type, size_type,size_type> t_part_tuple;
            auto k_2 = k * k;

            //1. Edges z-order
            std::vector<size_type> edges_z_order;
            for(size_type y = 0; y < adjacent_lists.size(); ++y){
                for(size_type x : adjacent_lists[y]){
                    edges_z_order.push_back(codes::zeta_order::encode(x, y));
                }
            }
            size_type bsize = adjacent_lists.size();
            adjacent_lists.clear();

            //2. Sort edges z-order
            std::sort(edges_z_order.begin(), edges_z_order.end());

            //4. Init bitmap
            m_t[0] = 1;
            m_ones_prev_leaves = 1;

            std::queue<t_part_tuple> q;
            q.push(t_part_tuple(0, edges_z_order.size()-1, bsize/k , 0));

            size_type i, j, z_0;
            size_type t = k_2;
            size_type n_elem = 0, zeroes = k_2 -1;
            std::unordered_map<size_type , size_type > hash;
            std::vector<size_type> code_leaves;

            //5. Split the front of q into its children
            while (!q.empty()) {
                std::tie(i, j, bsize, z_0) = q.front();
                q.pop();
                auto elements = bsize * bsize;
                for(size_type z_child = 0; z_child < k_2; ++z_child){
                    auto le = util::search::lower_or_equal_search(i, j, edges_z_order, z_0+elements-1);
                    if(bsize > leaf_size){
                        check_resize(m_t, t);
                        if(le != -1 && edges_z_order[le] >= z_0){
                            m_t[t] = 1;
                            q.push(t_part_tuple(i, le, bsize/k, z_0));
                            i = le + 1;
                            ++m_ones_prev_leaves;
                        }else{
                            m_t[t] = 0;
                        }
                        ++t;
                    }else{
                        check_resize(m_t, t);
                        if(le != -1 && edges_z_order[le] >= z_0){
                            m_t[t] = 1;
                            uint64_t code = code_block(edges_z_order, i, le);
                            code_leaves.push_back(code);
                            auto it = hash.find(code);
                            if(it != hash.end()){
                                ++it->second;
                            }else{
                                hash.insert({code, 1});
                            }
                            i = le + 1;
                        }else{
                            m_t[t] = 0;
                        }
                        ++t;
                    }
                    z_0 += elements;
                }
            }
            m_t.resize(t);
            sdsl::util::init_support(m_t_rank, &m_t);

            //Building the vocab
            m_voc.resize(hash.size());
            std::vector<std::pair<size_type, size_type>> vocab_vector (hash.begin(),hash.end());
            hash.clear();
            auto sortby = [](std::pair<size_type, size_type> a, std::pair<size_type, size_type> b) {
                                return a.second > b.second;
                            };
            std::sort(vocab_vector.begin(), vocab_vector.end(), sortby);
            size_type cw = 0;
            for(const auto &c: vocab_vector){
                hash.insert({c.first, cw});
                m_voc[cw]=c.first;
                ++cw;
            }

            //Encoding the leaves
            for(j = 0; j < code_leaves.size(); ++j){
                auto it = hash.find(code_leaves[j]);
                code_leaves[j] = it->second;
            }
            hash.clear();
            m_l = sdsl::dac_vector_dp_opt<>(code_leaves);

        }

        template <class add_function, class result_type>
        void recursive_access_region(const size_type min_x, const size_type max_x, const size_type min_y, const size_type max_y,
                                     const size_type x, const size_type y, const size_type idx, const size_type level,
                                     const size_type block_size, result_type &result, add_function add){

#if BT_VERBOSE
            std::cout << "at position: " << idx << std::endl;
            std::cout << "x=" << x << " y=" << y << std::endl;
            std::cout << "block_size: " << block_size << std::endl;
            std::cout << "current_level: " << level << std::endl;
            std::cout << "height: " << height << std::endl;
            std::cout << std::endl;
#endif
            if(block_size == leaf_size){
                if(m_t[idx]) {
                    size_type c_i = m_t_rank(idx+1) - m_ones_prev_leaves-1;
                    size_type code = m_voc[m_l[c_i]];
                    while(code){
                        size_type set_bit = sdsl::bits::lo(code);
                        size_type o_x = set_bit % leaf_size;
                        size_type o_y = set_bit / leaf_size;
                        if(min_x <= o_x && o_x <= max_x && min_y <= o_y && o_y <= max_y){
                            add(result, x + o_x, y + o_y);
                        }
                        code = code & sdsl::bits::lo_unset[set_bit+1];
                    }
                }

            }else{
                //if(m_topology[idx]){
                if(m_t[idx]){
                    size_type new_min_x, new_max_x, new_min_y, new_max_y;
                    //size_type start_children =  m_topology_rank(idx + 1) * m_k2;
                    size_type start_children =  m_t_rank(idx + 1) * m_k2;
                    size_type new_block_size = block_size / m_k;
                    size_type disp_x = 0;
                    for (size_type i = min_x / new_block_size; i <= max_x / new_block_size; i++) {
                        new_min_x = 0;
                        if (i == min_x / new_block_size) {
                            new_min_x = min_x % new_block_size;
                        }
                        new_max_x = new_block_size - 1;
                        if (i == max_x / new_block_size) {
                            new_max_x = max_x % new_block_size;
                        }
                        size_type disp_y = 0;
                        for (size_type j = min_y / new_block_size; j <= max_y / new_block_size; j++) {
                            new_min_y = 0;
                            if (j == min_y / new_block_size) {
                                new_min_y = min_y % new_block_size;
                            }
                            new_max_y = new_block_size - 1;
                            if (j == max_y / new_block_size) {
                                new_max_y = max_y % new_block_size;
                            }
                            recursive_access_region(new_min_x, new_max_x, new_min_y, new_max_y,
                                                    x + (new_block_size * i - min_x) * disp_x,
                                                    y + (new_block_size * j - min_y) * disp_y,
                                                    start_children + codes::zeta_order::encode(i, j),
                                                    level + 1,
                                                    new_block_size, result, add);
                            disp_y = 1;
                        }
                        disp_x = 1;
                    }
                }
            }
        }

    public:

        const size_type &k = m_k;
        const size_type &height = m_height;
        const size_type &dimensions = m_dimensions;

        k2_tree_compression_leaves() = default;

        //Pre: adjacency_lists have to contain at least one element
        k2_tree_compression_leaves(input_type &adjacency_lists, const size_type kparam){
            size_type h, total_size;
            init_structure();
            init_construction(total_size, adjacency_lists, kparam);
            size_type blocks = m_k2, block_size = total_size/m_k;
            construction(adjacency_lists, k);
        }

        //! Copy constructor
        k2_tree_compression_leaves(const k2_tree_compression_leaves &p) {
            copy(p);
        }

        //! Move constructor
        k2_tree_compression_leaves(k2_tree_compression_leaves &&p) {
            *this = std::move(p);
        }

        //! Assignment move operation
        k2_tree_compression_leaves &operator=(k2_tree_compression_leaves &&p) {
            if (this != &p) {
                m_k = std::move(p.m_k);
                m_k2 = m_k * m_k;
                m_height = std::move(p.m_height);
                m_dimensions = std::move(p.m_dimensions);
                m_t = std::move(p.m_t);
                m_t_rank = std::move(p.m_t_rank);
                m_t_rank.set_vector(&m_t);
                m_voc = std::move(p.m_voc);
                m_l = std::move(p.m_l);
                m_ones_prev_leaves = std::move(p.m_ones_prev_leaves);

            }
            return *this;
        }

        //! Assignment operator
        k2_tree_compression_leaves &operator=(const k2_tree_compression_leaves &p) {
            if (this != &p) {
                copy(p);
            }
            return *this;
        }

        //! Swap method
        /*! Swaps the content of the two data structure.
        *  You have to use set_vector to adjust the supported bit_vector.
        *  \param bp_support Object which is swapped.
        */
        void swap(k2_tree_compression_leaves &p) {
            std::swap(m_k, p.m_k);
            std::swap(m_k2, p.m_k2);
            std::swap(m_dimensions, p.m_dimensions);
            std::swap(m_height, p.m_height);
            std::swap(m_t, p.m_t);
            sdsl::util::swap_support(m_t_rank, p.m_t_rank, &m_t, &(p.m_t));
            std::swap(m_voc, p.m_voc);
            std::swap(m_l, p.m_l);
            std::swap(m_ones_prev_leaves, p.m_ones_prev_leaves);
        }

        inline void access_region(const size_type min_x, const size_type min_y,
                                  const size_type max_x, const size_type max_y,
                                  input_type &result){
            size_type size_vector = max_y - min_y+1;
            result = input_type(size_vector);
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(min_x, max_x, min_y, max_y, 0, 0, 0, 0, block_size, result, add_in_region());

        }

        inline std::vector<size_type> neigh(size_type id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(0, this->m_dimensions-1, id, id, 0, 0, 0, 0, block_size, r, add_in_row());
            return r;
        }

        inline std::vector<size_type> access(const size_type id, const size_type direct_id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(this->m_k, this->m_height);
            this->recursive_access_region(direct_id, direct_id, id, id, 0, 0, 0, 0, block_size, r, add_in_row());
            return r;
        }

        inline std::vector<size_type> reverse_neigh(size_type id){
            std::vector<size_type> r;
            auto block_size = (size_type) std::pow(m_k, m_height);
            this->recursive_access_region(id, id, 0, this->m_dimensions-1, 0, 0, 0, 0, block_size, r, add_in_column());
            return r;
        }

        //! Serializes the data structure into the given ostream
        virtual size_type serialize(std::ostream& out, sdsl::structure_tree_node* v=nullptr, const std::string name="")const
        {
            sdsl::structure_tree_node* child = sdsl::structure_tree::add_child(
                    v, name, sdsl::util::class_name(*this));
            size_type written_bytes = 0;
            written_bytes += sdsl::write_member(m_k, out, child, "k");
            written_bytes += sdsl::write_member(m_height, out, child, "height");
            written_bytes += sdsl::write_member(m_dimensions, out, child, "dimensions");
            written_bytes += sdsl::write_member(m_ones_prev_leaves, out, child, "ones_prev_leaves");
            written_bytes += m_t.serialize(out, child, "t");
            written_bytes += m_t_rank.serialize(out, child, "t_rank");
            written_bytes += m_voc.serialize(out, child, "voc");
            written_bytes += m_l.serialize(out, child, "l");
            return written_bytes;
        }

        //! Loads the data structure from the given istream.
        virtual void load(std::istream& in)
        {
            sdsl::read_member(m_k, in);
            sdsl::read_member(m_height, in);
            sdsl::read_member(m_dimensions, in);
            sdsl::read_member(m_ones_prev_leaves, in);
            m_t.load(in);
            m_t_rank.load(in);
            m_t_rank.set_vector(&m_t);
            m_voc.load(in);
            m_l.load(in);
            m_k2 = m_k * m_k;
        }


    };
}

#endif //INC_2D_BLOCK_TREE_k2_tree_compression_leaves_HPP
