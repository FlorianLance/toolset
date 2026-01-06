
/*******************************************************************************
** Toolset-ts-base                                                            **
** MIT License                                                                **
** Copyright (c) [2024] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/


#pragma once

// std
#include <cstdint>

struct signed_24_bit {

    uint8_t other_data;
    uint8_t use_by_the_24_bit_int[3];

    static auto decode(signed_24_bit x) -> std::int32_t{

        std::int32_t y =
            x.use_by_the_24_bit_int[0] // 1st byte of int24
            + (x.use_by_the_24_bit_int[1] * 0x100)
            + (x.use_by_the_24_bit_int[2] * 0x10000);

        if (y > 0x7FFFFF){
            y -= 0x1000000;
        }
        return y;
    }

    // assume `x` is in range of `int24_t`
    static auto encode(int x) -> signed_24_bit{

        if ( x < 0) {
            x += 0x1000000;
        }

        signed_24_bit y;
        y.other_data = 0;
        y.use_by_the_24_bit_int[0] = x;
        y.use_by_the_24_bit_int[1] = x/0x100;
        y.use_by_the_24_bit_int[2] = x/0x10000;

        return y;
    }
};

struct Int24{
    unsigned char data[3];

    Int24() = default;
    Int24( const int val ){*this = val;}
    Int24( const Int24& val ){*this = val;}

    auto to_int() const -> int{
        if ( data[2] & 0x80 ){ // Is this a negative?  Then we need to siingn extend.
            return (0xff << 24) | (data[2] << 16) | (data[1] << 8) | (data[0] << 0);
        }else{
            return (data[2] << 16) | (data[1] << 8) | (data[0] << 0);
        }
    }

    auto to_float() const -> float{
        return static_cast<float>(to_int());
    }

    auto operator =( const Int24& input ) -> Int24&{
        data[0] = input.data[0];
        data[1] = input.data[1];
        data[2] = input.data[2];
        return *this;
    }

    auto operator =(const int input) -> Int24&{
        data[0]   = ((unsigned char*)&input)[0];
        data[1]   = ((unsigned char*)&input)[1];
        data[2]   = ((unsigned char*)&input)[2];
        return *this;
    }
};
