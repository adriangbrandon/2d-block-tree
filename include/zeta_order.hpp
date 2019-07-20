//
// Created by adrian on 25/06/17.
//

#ifndef INC_CODES_ZETA_ORDER_HPP
#define INC_CODES_ZETA_ORDER_HPP

#include <stdint.h>
#include <utility>

namespace codes {

    class zeta_order {

    private:
        /*static constexpr uint64_t masks[6] = {0x5555555555555555 , 0x3333333333333333,
                                          0x0F0F0F0F0F0F0F0F, 0x00FF00FF00FF00FF, 0x0000FFFF0000FFFF, 0x00000000FFFFFFFF};
        static constexpr uint64_t offset[5] = {1, 2, 4, 8, 16};*/
        static uint64_t interleaving_number_encode(uint64_t x){
            x &= 0x00000000FFFFFFFF;
            x = (x | (x << 16)) & 0x0000FFFF0000FFFF;
            x = (x | (x << 8)) & 0x00FF00FF00FF00FF;
            x = (x | (x << 4)) & 0x0F0F0F0F0F0F0F0F;
            x = (x | (x << 2)) & 0x3333333333333333;
            x = (x | (x << 1)) & 0x5555555555555555;
            return x;
        }


        static uint64_t interleaving_number_decode(uint64_t x){
            x  &= 0x5555555555555555;
            x = (x ^(x >> 1)) & 0x3333333333333333;
            x = (x ^(x >> 2)) & 0x0F0F0F0F0F0F0F0F;
            x = (x ^(x >> 4)) & 0x00FF00FF00FF00FF;
            x = (x ^(x >> 8)) & 0x0000FFFF0000FFFF;
            x = (x ^(x >> 16)) & 0x00000000FFFFFFFF;
            return x;
        }

    public:
        static uint64_t encode(uint64_t x, uint64_t y){
            return (interleaving_number_encode(y) << 1) | interleaving_number_encode(x);
        }

        static std::pair<uint64_t, uint64_t> decode(uint64_t code){
            uint64_t x = interleaving_number_decode(code);
            uint64_t y = interleaving_number_decode(code >> 1);
            return {x, y};
            //return std::make_pair(x, y);
        }
    };
}
#endif //INC_CODES_ZETA_ORDER_HPP
