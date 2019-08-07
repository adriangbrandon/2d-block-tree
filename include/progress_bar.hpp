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
// Created by Adrián on 07/08/2019.
//

#ifndef INC_UTIL_PROGRESS_BAR_HPP
#define INC_UTIL_PROGRESS_BAR_HPP

#include <iostream>
#include <stdint.h>

namespace util {

    class progress_bar {


    public:
        typedef uint64_t size_type;
    private:

        size_type m_total;
        size_type m_width;
        const char bar_char = '=';
        const char empty_char = ' ';
        size_type m_freq_refresh;
        std::chrono::steady_clock::time_point m_prev_time = std::chrono::steady_clock::now();

        uint64_t m_elements;

        void display(){

            float progress =  m_elements / (float) m_total;
            auto pos = (size_type) (progress * m_width);
            std::cout << "[";
            for(size_type i = 0; i < m_width; ++i){
                if(i < pos){
                    std::cout << bar_char;
                }else if (i == pos){
                    std::cout << '>';
                }else{
                    std::cout << empty_char;
                }
            }
            std::cout << "] " << (size_type) (progress * 100) << "% \t (" << m_elements << "/" << m_total << ") \r";
            std::cout.flush();

        }

    public:

        progress_bar(const size_type t, const size_type w = 50, const size_type freq = 10000){
            m_total = t;
            m_width = w;
            m_elements = 0;
            m_freq_refresh = freq;
        }

        void update_by(const size_type add){
            m_elements += add;
            display();
        }

        void update(const size_type v){
            m_elements = v;
            std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
            auto time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now-m_prev_time).count();
            if(time_elapsed > m_freq_refresh) {
                display();
                m_prev_time = now;
            }
        }

        void done(){
            m_elements = m_total;
            display();
            std::cout << std::endl;
        }



    };
}

#endif //INC_UTIL_PROGRESS_BAR_HPP
