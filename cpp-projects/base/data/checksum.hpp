

/*******************************************************************************
** Toolset-base                                                               **
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

// std
#include <cstdint>
#include <cstddef>
#include <span>

#pragma once

namespace tool::data{

struct Checksum{

    [[nodiscard]][[maybe_unused]] static auto gen_crc16(std::span<const std::byte> data) -> std::uint16_t{
        return gen_crc16(reinterpret_cast<const std::uint8_t*>(data.data()), static_cast<std::uint16_t>(data.size_bytes()));
    }

    [[nodiscard]][[maybe_unused]] static auto gen_crc16(const std::uint8_t *data, std::uint16_t size) -> std::uint16_t{

        static constexpr std::uint16_t cr16 = 0x8005;

        /* Sanity check: */
        if(data == nullptr){
            return 0;
        }

        int bits_read = 0;
        std::uint16_t out = 0;
        while(size > 0)
        {
            int bit_flag = out >> 15;

            /* Get next bit: */
            out <<= 1;
            out |= (*data >> bits_read) & 1; // item a) work from the least significant bits

            /* Increment bit counter: */
            bits_read++;
            if(bits_read > 7){
                bits_read = 0;
                data++;
                size--;
            }

            /* Cycle check: */
            if(bit_flag){
                out ^= cr16;
            }
        }

        // item b) "push out" the last 16 bits
        for (int ii = 0; ii < 16; ++ii) {
            int bit_flag = out >> 15;
            out <<= 1;
            if(bit_flag){
                out ^= cr16;
            }
        }

        // item c) reverse the bits
        std::uint16_t crc = 0;
        int jj = 0x0001;
        for (int ii = 0x8000; ii != 0; ii >>=1, jj <<= 1) {
            if (ii & out){
                crc |= jj;
            }
        }

        return crc;
    }
};
}


