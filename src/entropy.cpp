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
// Created by Adrián on 9/6/21.
//

#include <block_tree_comp_ones_access.hpp>
#include <file_util.hpp>

int main(int argc, char **argv) {

    std::string name_file = argv[1];
    if(util::file::file_exists(name_file)) {
        block_tree_2d::block_tree_comp_ones_access<dataset_reader::raster_log> m_block_tree;
        sdsl::load_from_file(m_block_tree, name_file);
        auto pair = m_block_tree.shannon_entropy_bits_delete();
        std::cout << "Bits entropy: " << pair.first << " (" << (uint64_t) (std::ceil((double) pair.first / 8)) << " bytes.)"<< std::endl;
        std::cout << "Bits to delete: " << pair.second << " (" << (uint64_t) (std::ceil((double) pair.second / 8)) << " bytes.)"<< std::endl;
        std::cout << "Diff (bytes): " << (uint64_t) (std::ceil((double) (pair.second - pair.first)/ 8)) << std::endl;
    }else{
        std::cout << "Index " << name_file << " not found." << std::endl;
    }

}