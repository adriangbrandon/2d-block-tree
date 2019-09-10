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
// Created by Adrián on 09/09/2019.
//

#ifndef INC_2D_BLOCK_TREE_SEARCH_UTIL_HPP
#define INC_2D_BLOCK_TREE_SEARCH_UTIL_HPP

#include <stdint.h>

namespace util {

    namespace search {

        /**
         * If v exists, returns its last position
         * If v not exists, returns the position of the max value which is lower than v
         */
        template <class Container, class Value>
        uint64_t lower_or_equal_search(const uint64_t i, const uint64_t j, const Container& cont, const Value &v){
            if(i > j) return -1;
            auto l = i - 1;
            auto r = j + 1;
            while(l + 1 < r){
                auto mid = (r - l) / 2 + l;
                if(cont[mid] < v){
                    l = mid;
                }else if (cont[mid] > v){
                    r = mid;
                }else{
                    l = mid;
                }
            }
            return l;
        }

    }
}

#endif //INC_2D_BLOCK_TREE_SEARCH_UTIL_HPP
