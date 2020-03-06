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

#ifndef INC_2D_BLOCK_TREE_INTERSECTION_LISTS_HPP
#define INC_2D_BLOCK_TREE_INTERSECTION_LISTS_HPP

namespace util {

    template <class Iterator, class OutputIterator>
    static void intersection_lists_no_reps(Iterator beg1, Iterator end1, Iterator beg2, Iterator end2,
                                           OutputIterator sol){

        while(beg1 != end1 && beg2 != end2){
            if(*beg1 < *beg2) ++beg1;
            else if(*beg2 < *beg1) ++beg2;
            else {
                auto value = *beg1;
                while(beg1 != end1 && value == *beg1){
                    ++beg1;
                }
                while(beg2 != end2 && value == *beg2){
                    ++beg2;
                }
                *sol = value;
                ++sol;
            }
        }
    }

    template <class Iterator>
    static size_t intersection_lists_no_reps(Iterator beg1, Iterator end1, Iterator beg2, Iterator end2){

        auto sol_first = beg1;
        auto sol_ptr = beg1;
        while(beg1 != end1 && beg2 != end2){
            if(*beg1 < *beg2) ++beg1;
            else if(*beg2 < *beg1) ++beg2;
            else {
                *sol_ptr = *beg1;
                while(beg1 != end1 && *sol_ptr == *beg1){
                    ++beg1;
                }
                while(beg2 != end2 && *sol_ptr == *beg2){
                    ++beg2;
                }
                ++sol_ptr;
            }
        }
        return std::distance(sol_first, sol_ptr);
    }

    template <class Type>
    static void intersection_lists_no_reps(std::vector<Type> &in1, std::vector<Type> &in2, std::vector<Type> &out){
        intersection_lists_no_reps(in1.begin(), in1.end(), in2.begin(), in2.end(), std::back_inserter(out));
    }

    template <class Type>
    static void intersection_lists_no_reps(std::vector<Type> &in1, std::vector<Type> &in2){
        auto size = intersection_lists_no_reps(in1.begin(), in1.end(), in2.begin(), in2.end());
        in1.resize(size);
    }

    template <class Input, class Output>
    static void intersection_lists_no_reps(Input &in1, Input &in2, Output &out){

        auto beg1 = in1.begin();
        auto end1 = in1.end();
        auto beg2 = in2.begin();
        auto end2 = in2.end();
        auto sol = std::back_inserter(out);
        while(beg1 != end1 && beg2 != end2){
            if(*beg1 < *beg2) ++beg1;
            else if(*beg2 < *beg1) ++beg2;
            else {
                *sol = *beg1;
                while(beg1 != end1 && *sol == *beg1){
                    ++beg1;
                }
                while(beg2 != end2 && *sol == *beg2){
                    ++beg2;
                }
                ++sol;
            }
        }
    }

}


#endif //INC_2D_BLOCK_TREE_INTERSECTION_LISTS_HPP
