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
// Created by Adrián on 18/07/2019.
//
#include <hash_table_chainning.hpp>
#include <iostream>
#include <vector>

typedef uint64_t hash_type;
typedef hash_table::hash_table_chainning<hash_type, uint64_t> htc_type;
typedef typename htc_type::iterator_table_type iterator_table_type;
typedef typename htc_type::iterator_hash_type iterator_hash_type;
typedef typename htc_type::iterator_value_type iterator_value_type;

void insert(hash_type v, hash_type h, htc_type &htc){
    iterator_table_type it_table;
    iterator_hash_type it_hash;
    auto c1 = htc.hash_collision(h, it_table, it_hash);
    if(c1){
        std::cout << "Collision inserting: " << v << std::endl;
        htc.insert_hash_collision(it_hash, v);
    }else{
        std::cout << "No collision inserting: " << v << std::endl;
        htc.insert_no_hash_collision(it_table, h, v);
    }
}

int main(int argc, char **argv) {

    std::vector<hash_type> hashes = {2, 4, 3, 1, 5};

    htc_type m_htc(4);

    for(uint64_t i = 0; i < hashes.size(); ++i){
        insert(i, hashes[i], m_htc);
        m_htc.print();
    }


}
