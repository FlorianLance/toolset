
/*******************************************************************************
** Toolset-ts-base                                                            **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
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
#include <array>

namespace tool{

struct Byte{

    union{
        std::uint8_t b1 : 1, b2 : 1, b3 : 1, b4 : 1, b5 : 1, b6 : 1, b7 : 1, b8 : 1;
        std::uint8_t value = 0;
    };

    Byte() = default;
    Byte(const Byte&) = default;
    constexpr explicit Byte(std::uint8_t initValue) noexcept : value(initValue){}

    constexpr auto at(size_t idBit) const noexcept -> bool{
        return value & m_byteA[idBit];
    }

    auto set(size_t idBit, bool bit) -> void{

        if(!bit){
            value &= m_invertByteA[idBit];
        }else{
            value |= m_byteA[idBit];
        }
    }

    constexpr auto count(bool state) const noexcept -> std::uint8_t{
        std::uint8_t count = 0;
        for(size_t ii = 0; ii < 8; ++ii){
            if(at(ii) == state){
                ++count;
            }
        }
        return count;
    }

private:
    static constexpr std::array<std::uint8_t, 8> m_byteA{
        0b1, 0b10, 0b100, 0b1000, 0b10000, 0b100000, 0b1000000, 0b10000000
    };
    static constexpr std::array<std::uint8_t, 8> m_invertByteA{
        0b11111110, 0b11111101, 0b11111011, 0b11110111, 0b11101111, 0b11011111, 0b10111111, 0b01111111
    };
};

template<size_t sizeBits>
struct BitMask{

    static_assert(sizeBits % 8 == 0);

    BitMask() = default;
    BitMask(const BitMask &) = default;

    explicit BitMask(bool initValue){
        fill(initValue);
    }

    inline auto set(size_t idBit, bool bit) noexcept -> void{
        bytes[idBit/8].set(idBit%8, bit);
    }

    inline auto at(size_t idBit) const noexcept -> bool{
        return bytes[idBit/8].at(idBit%8);
    }

    constexpr auto size_bytes() const -> size_t{
        return size_bits()/8;
    }

    constexpr auto size_bits() const -> size_t{
        return sizeBits;
    }

    inline auto fill(bool initValue) -> void{
        std::fill(bytes.begin(), bytes.end(), initValue ? Byte(255) : Byte(0));
    }

    auto count(bool state) const -> size_t{
        size_t count = 0;
        for(const auto &byte : bytes){
            count += byte.count(state);
        }
        return count;
    }

    std::array<Byte, sizeBits/8> bytes;
};
}
