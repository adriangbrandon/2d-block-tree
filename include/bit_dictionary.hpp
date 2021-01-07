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
// Created by Adrián on 7/1/21.
//

#ifndef UTIL_BIT_DICTIONARY_HPP
#define UTIL_BIT_DICTIONARY_HPP

namespace util {

    class bit_dictionary {

    public:

        typedef typename uint64_t size_type;


    private:

        sdsl::bit_vector m_bit_vector;
        sdsl::rank_support_v<> m_rank_bit;

    public:

        template<class Type>
        bit_dictionary(const std::vector<Type> &c){
            m_bit_vector = sdsl::bit_vector(c.back()+1, 0)
            for(const auto &v : c){
                m_bit_vector[v] = 1;
            }
            sdsl::util::init_support(m_rank_bit, &m_bit_vector);
        }

        bool exists(const size_type i){
            return (i < m_bit_vector.size() && m_bit_vector[i])
        }

        size_type get(const size_type i){
            return m_rank_bit(i);
        }







    };

}

#endif //UTIL_BIT_DICTIONARY_HPP
