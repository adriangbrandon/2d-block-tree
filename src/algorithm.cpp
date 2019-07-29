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
// Created by Adrián on 20/07/2019.
//


#include <iostream>
#include <vector>
#include <list>
#include <time_util.hpp>
#include <alternative_code.hpp>

int main(int argc, char **argv) {

    std::list<uint64_t > list = {1, 3, 5, 7};
    std::cout << *(list.end()) << std::endl;
    auto it = list.begin();

    auto rend = list.rend();

    auto it2 = list.end();
    auto beg = list.begin();
    --beg;
    --it2;
    while(it2 != beg){
        std::cout << *it2 << std::endl;
        --it2;
    }
    std::cout << "--------------" << std::endl;
    std::cout << *(++beg) << std::endl;
    //advance(it, 2);
    list.erase(it);

    if(it == list.begin()){
        std::cout << "Begin" << std::endl;
    }
    std::cout << *(list.end()) << std::endl;
    std::cout << *it << std::endl;

    std::cout << "----------" << std::endl;
    it2 = list.begin();
    while(it2 != list.end()){
        std::cout << *it2 << std::endl;
        ++it2;
    }

    std::cout << "----------" << std::endl;
    while(it != list.end()){
        std::cout << *it << std::endl;
        ++it;
    }


    uint64_t size = 10000;
    std::vector<uint64_t > elements(size);
    std::vector<uint64_t > el(size);
    for(uint64_t i = 0; i < elements.size(); ++i){
        elements[i] = i;
        el[i] = i;
    }
    auto j = 0, k = 0;
    auto pos = 4;
    auto t0 = util::time::user::now();
    while(j < 100){
        elements.erase(elements.begin()+pos);
        ++j;
    }
    auto t1 = util::time::user::now();

    auto rest = el.size() - pos-1;
    auto t2 = util::time::user::now();
    while(k < 100){
        std::memmove(&el[pos], &el[pos+1],   (el.size() - pos-1) * sizeof(uint64_t));
        el.resize(el.size()-1);
        ++k;
    }
    auto t3 = util::time::user::now();

    for(uint64_t i = 0; i < elements.size(); ++i){
        if(el[i] != elements[i]){
            std::cout << "Error at i:" << i << "(" << el[i] << ":" << elements[i] << ")" << std::endl;
            exit(11);
        }
    }

    std::cout << "Erase: " << t1-t0 << std::endl;
    std::cout << "Memmove: " << t3-t2 << std::endl;

    uint64_t v1 = codes::alternative_code::encode(-1);
    int64_t v2 = codes::alternative_code::decode(v1);
    std::cout << "v1: " << v1 << std::endl;
    std::cout << "v2: " << v2 << std::endl;


    exit(10);


}