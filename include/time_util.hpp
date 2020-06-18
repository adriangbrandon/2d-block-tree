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
// Created by Adrián on 27/11/2018.
//

#ifndef TIME_UTIL_HPP
#define TIME_UTIL_HPP

#include <cstdint>
#include <sys/resource.h>

namespace util {

    class time {

    public:

        const static uint64_t microseconds    = 1;
        const static uint64_t milliseconds    = 1000;
        const static uint64_t seconds         = 1000000;
        const static uint64_t minutes         = 60000000;

        template<uint64_t ratio = 1>
        static double duration_cast(uint64_t value){
            return value / (double) ratio;
        }

        class user {

        public:

            /***
             * User time in microseconds
             * @return the user time in microseconds
             */
            static uint64_t now(){

                struct rusage r_usage;
                getrusage(RUSAGE_SELF, &r_usage);
                return (r_usage.ru_utime.tv_sec * seconds + r_usage.ru_utime.tv_usec);
            }

        };

        class system {

        public:

            /***
             * Sys time in microseconds
             * @return the system time in microseconds
             */
            static uint64_t now(){

                struct rusage r_usage;
                getrusage(RUSAGE_SELF, &r_usage);
                return (r_usage.ru_stime.tv_sec * seconds + r_usage.ru_stime.tv_usec);
            }

        };


    };
}

#endif
