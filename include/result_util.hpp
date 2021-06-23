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
// Created by Adrián on 19/03/2020.
//

#ifndef INC_2D_BLOCK_TREE_RESULT_UTIL_HPP
#define INC_2D_BLOCK_TREE_RESULT_UTIL_HPP

namespace block_tree_2d{

    struct add_in_region {
        template<class result_type, class size_type>
        void operator() (result_type &result, const size_type x, const size_type y)
        {
            result[y].push_back(x);
        }
    };

    struct add_in_row {
        template<class size_type>
        void operator() (std::vector<size_type> &result, const size_type x, const size_type y)
        {
            result.push_back(x);
        }
    };

    struct add_in_column {
        template<class size_type>
        void operator() (std::vector<size_type> &result, const size_type x, const size_type y)
        {
            result.push_back(y);
        }
    };

    struct add_raster {

        uint64_t n_cols;

       template<class result_type, class size_type>
       void operator() (result_type &result, const size_type x, const size_type y)
        {
            result[y*n_cols+x] = 1;
        }
    };

    struct subtract_raster {

        uint64_t n_cols;

        template<class result_type, class size_type>
        void operator() (result_type &result, const size_type x, const size_type y)
        {
            result[y*n_cols+x] = 0;
        }
    };
}



#endif //INC_2D_BLOCK_TREE_RESULT_UTIL_HPP
