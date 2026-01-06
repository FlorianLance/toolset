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
#include <vector>
#include <unordered_set>


namespace tool {

// vector alias
// template< class value > using std_v1 = std::vector<value>; /**< templated std vector alias */
// template< class value > using std_v2 = std::vector<std_v1<value>>; /**< templated std vector of std vector alias */
// template< class value > using std_v3 = std::vector<std_v2<value>>; /**< templated std vector of std vector of std vector alias */
// template< class value > using c_std_v1  = const std::vector<value>; /**< templated const std vector alias */
// template< class value > using c_std_v2  = const std_v2<value>; /**< templated const std vector of std vector alias*/
// template< class value > using c_std_v3  = const std_v3<value>; /**< templated const std vector of std vector of std vector alias */

template <class T>
static inline auto erase_row(std::vector<T> &vector, size_t id) noexcept -> void{
    using difference_type = typename std::vector<T>::difference_type;
    vector.erase(vector.begin() + static_cast<difference_type>(id));
}

template <class T>
static inline auto erase_range(std::vector<T> &vector, size_t start, size_t end) noexcept -> void{
    vector.erase(vector.begin() + start, vector.begin() + end);
}

template<typename T>
auto remove_duplicates(std::vector<T> &vec) -> size_t{

    std::unordered_set<T> seen;
    auto newEnd = std::remove_if(vec.begin(), vec.end(), [&seen](const T &value){
        if (seen.find(value) != std::end(seen)){
            return true;
        }
        seen.insert(value);
        return false;
    });
    vec.erase(newEnd, vec.end());
    return vec.size();
}

template<typename T>
auto merge_to_v1(std::vector<T> &v1, std::vector<T> &v2) -> void{
    auto v1size = v1.size();
    v1.resize(v1size+v2.size());
    std::move(v2.begin(), v2.end(), v1.begin() + v1size);
}

template<typename T1, typename T2>
auto convert(const std::vector<T2*> &v) -> std::vector<T1*>{
    std::vector<T1*> r(v.size());
    for(size_t ii = 0; ii < v.size(); ++ii){
        r[ii] = v[ii];
    }
    return r;
}

}
