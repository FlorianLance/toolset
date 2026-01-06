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
#include <optional>
#include <span>
#include <iostream>

// local
#include "buffer.hpp"
#include "string_unordered_map.hpp"

namespace tool {

template <class T>
class IdAliasMap{
public:

    auto add_element(std::string_view alias, T &&element) -> void{

        if(aliases.contains(alias)){
            // ...
            return;
        }
        currentId = aliases.size();        
        elements.push_back({std::string(alias), std::forward<T>(element)});
        aliases[alias] = currentId;
    }

    auto get_alias(size_t id) const noexcept -> std::optional<std::string_view>{
        if(has_id(id)){
            return std::get<0>(elements[id]);
        }
        return std::nullopt;
    }

    auto get_id(std::string_view alias) const noexcept -> std::optional<size_t>{
        if(aliases.contains(alias)){
            return aliases.at(alias);
        }
        return std::nullopt;
    }

    auto get_current_alias() const noexcept -> std::string_view{
        if(has_id(currentId)){
            return std::get<0>(elements[currentId]);
        }
        return "";
    }

    auto get_current_id() const noexcept -> size_t{
        return currentId;
    }

    auto has_alias(std::string_view alias) const noexcept -> bool{
        return aliases.contains(alias);
    }

    auto has_id(size_t id) const noexcept -> bool{
        return id < elements.size();
    }

    auto count() const noexcept -> size_t{
        return aliases.size();
    }

    auto set_current_id(size_t id) noexcept -> void{
        if(has_id(id)){
            currentId = id;
        }
        return;
    }

    auto set_current_id(std::string_view alias) noexcept -> void{
        if(has_alias(alias)){
            currentId = aliases[alias];
        }
        return;
    }

    auto increment_current_id() noexcept -> void{
        if(has_id(currentId+1)){
            ++currentId;
        }
    }

    auto decrement_current_id() noexcept -> void{
        if(currentId > 0){
            --currentId;
        }
    }

    auto move_up_current_element() -> void {
        if(currentId > 0){
            auto currentAlias     = std::get<0>(elements[currentId]);
            auto idUp             = currentId-1;
            auto aliasUp          = std::get<0>(elements[idUp]);
            aliases[currentAlias] = idUp;
            aliases[aliasUp]      = currentId;
            std::swap(elements[currentId], elements[idUp]);
        }
    }

    auto get_span() -> std::span<std::tuple<std::string, T>>{
        return std::span<std::tuple<std::string, T>>(elements.begin(),elements.end());
    }

protected:

    size_t currentId = 0;
    Buffer<std::tuple<std::string, T>> elements;
    s_umap<std::string, size_t> aliases;
};

template <class T>
class IdAliasMapSharedPtr : public IdAliasMap<std::shared_ptr<T>>{
    using F = IdAliasMap<std::shared_ptr<T>>;
public:

    auto get_curent_element() const -> std::shared_ptr<T>{
        if(F::has_id(F::currentId)){
            return std::get<1>(F::elements[F::currentId]);
        }
        return nullptr;
    }

    auto get_element(size_t id) const -> std::shared_ptr<T>{
        if(F::has_id(F::id)){
            return std::get<1>(F::elements[id]);
        }
        return nullptr;
    }

    auto get_element(std::string_view alias) const -> std::shared_ptr<T>{
        if(F::aliases.contains(alias)){
            return std::get<1>(F::elements[F::aliases.at(alias)]);
        }
        return nullptr;
    }

    auto get_current_element_ptr() const -> T*{
        if(F::has_id(F::currentId)){
            return std::get<1>(F::elements[F::currentId]).get();
        }
        return nullptr;
    }

    auto get_element_ptr(size_t id) const -> T*{
        if(F::has_id(F::id)){
            return std::get<1>(F::elements[id]).get();
        }
        return nullptr;
    }

    auto get_element_ptr(std::string_view alias) const -> T*{
        if(F::aliases.contains(alias)){
            return std::get<1>(F::elements[F::aliases.at(alias)]).get();
        }
        return nullptr;
    }

    auto debug() -> void{
        size_t id = 0;
        std::cout << "sizes " << F::elements.size() << " " << F::aliases.size()  << "\n";
        for(const auto &element : F::elements){
            std::cout << "element " << id << " " << std::get<0>(element) << "\n";
            ++id;
        }

        id = 0;
        for(const auto &alias : F::aliases){
            std::cout << "alias " << id << " " << std::get<0>(alias) << " " << std::get<1>(alias) << "\n";
            ++id;
        }
    }
};

template <class T>
class IdAliasMapUniquePtr : public IdAliasMap<std::unique_ptr<T>>{
    using F = IdAliasMap<std::unique_ptr<T>>;
public:

    auto get_current_element_ptr() const -> T*{
        if(F::has_id(F::currentId)){
            return std::get<1>(F::elements[F::currentId]).get();
        }
        return nullptr;
    }

    auto get_element_ptr(size_t id) const -> T*{
        if(F::has_id(F::id)){
            return std::get<1>(F::elements[id]).get();
        }
        return nullptr;
    }

    auto get_element_ptr(std::string_view alias) const -> T*{
        if(F::aliases.contains(alias)){
            return std::get<1>(F::elements[F::aliases.at(alias)]).get();
        }
        return nullptr;
    }
};

}
