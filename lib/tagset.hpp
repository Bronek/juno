// Copyright (c) 2018 Bronislaw (Bronek) Kozicki
//
// Distributed under the MIT License. See accompanying file LICENSE
// or copy at https://opensource.org/licenses/MIT

#pragma once

#include "tag.hpp"
#include "set.hpp"
#include <unordered_set>

namespace juno {
    template <typename Hash = std::hash<uintptr_t>>
    class tagset {
        using Data = std::unordered_set<uintptr_t, Hash>;
        Data data_;

        constexpr explicit tagset(Data&& d) : data_(std::move(d)) { }

    public:
        constexpr tagset() : data_(typename Data::size_type(0)) { }
        constexpr tagset(tagset&& ) noexcept = default;
        constexpr tagset(const tagset& ) = default;

        tagset& operator=(tagset&& ) & noexcept = default;
        tagset& operator=(const tagset& ) & = default;

        constexpr bool operator==(const tagset& rh) const noexcept {
            return this->data_ == rh.data_;
        }

        using size_type = size_t;
        using key_type = uintptr_t;
        using value_type = uintptr_t;
        using hasher = Hash;

        using const_reference = const uintptr_t&;
        using reference = const_reference;
        using const_iterator = typename Data::const_iterator;
        using iterator = const_iterator;

        // Note, with the exception of copy assignment (above), all member functions
        // are non-mutating and will return new value when necessary instead.
        constexpr bool empty() const noexcept { return data_.empty(); }
        constexpr size_t size() const noexcept { return data_.size(); }
        constexpr const_iterator begin() const noexcept { return data_.begin(); }
        constexpr const_iterator end() const noexcept { return data_.begin(); }

        template <typename T>
        constexpr bool is_same_set() const noexcept {
            return T::size == data_.size() && this->contains_set<T>();
        }

        template <typename ...P>
        constexpr bool is_same() const noexcept {
            using Set = set<P ...>;
            return this->is_same_set<Set>();
        }

        template <typename T>
        constexpr bool contains_set() const noexcept {
            auto fn = [](auto* p, const Data& d) noexcept -> bool {
                return d.count(tag<typename std::remove_pointer<decltype(p)>::type>::value) == 1;
            };
            return T::size <= data_.size() && T::for_each(fn, data_);
        }

        template <typename ...P>
        constexpr bool contains() const noexcept {
            using Set = set<P ...>;
            return this->contains_set<Set>();
        }

        template <typename T>
        constexpr bool intersects_set() const noexcept {
            auto fn = [](auto* p, const Data& d) noexcept -> bool {
                return d.count(tag<typename std::remove_pointer<decltype(p)>::type>::value) == 0;
            };
            return T::size > 0 && not T::for_each(fn, data_);
        }

        template <typename ...P>
        constexpr bool intersects() const noexcept {
            using Set = set<P ...>;
            return this->intersects_set<Set>();
        }

        // Mathematical term is "union"
        template <typename T>
        constexpr tagset insert_set() const noexcept {
            Data result(data_.size() + T::size);
            auto fn = [](auto* p, const Data& me, Data& r) noexcept -> bool {
                const auto value = tag<typename std::remove_pointer<decltype(p)>::type>::value;
                if (me.count(value) == 0)
                    r.insert(value);
                return true;
            };
            T::for_each(fn, data_, result);
            result.reserve(data_.size() + result.size());
            result.insert(data_.begin(), data_.end());
            return tagset(std::move(result));
        }

        template <typename ...P>
        constexpr tagset insert() const noexcept {
            using Set = set<P ...>;
            return this->insert_set<Set>();
        }

        // Mathematical term is "intersection"
        template <typename T>
        constexpr tagset cross_set() const noexcept {
            Data result;
            auto fn = [](auto* p, const Data& me, Data& r) noexcept -> bool {
                const auto value = tag<typename std::remove_pointer<decltype(p)>::type>::value;
                if (me.count(value) == 1) {
                    r.insert(value);
                }
                return r.size() < T::size && r.size() < me.size();
            };
            T::for_each(fn, data_, result);
            return tagset(std::move(result));
        }

        template <typename ...P>
        constexpr tagset cross() const noexcept {
            using Set = set<P ...>;
            return this->cross_set<Set>();
        }

        // Mathematical term is "relative complement"
        template <typename T>
        constexpr tagset remove_set() const noexcept {
            Data result = data_;
            auto fn = [](auto* p, Data& r) noexcept -> bool {
                r.erase(tag<typename std::remove_pointer<decltype(p)>::type>::value);
                return not r.empty();
            };
            T::for_each(fn, result);
            return tagset(std::move(result));
        }

        template <typename ...P>
        constexpr tagset remove() const noexcept {
            using Set = set<P ...>;
            return this->remove_set<Set>();
        }
    };
}
