/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_UTIL_BIT_CONTAINER_HPP
#define GBAXX_UTIL_BIT_CONTAINER_HPP

#include <array>
#include <cstddef>
#include <cstring>
#include <type_traits>

#include <gba/inttype.hpp>

#if __cpp_lib_bit_cast
#   include <bit>
#elseif !defined(__has_builtin)
#   define __has_builtin(x) 0
#endif

namespace gba::util {
namespace detail {

    template <std::size_t Start, std::size_t N, typename D, typename S>
    [[gnu::always_inline]]
    constexpr void constexpr_array_copy(D dst[N], S src[N]) noexcept {
        if constexpr (Start < N) {
            constexpr auto index = std::integral_constant<decltype(Start), Start>();
            dst[index] = src[index];
            constexpr_array_copy<Start + 1, N, D, S>(dst, src);
        }
    }

    struct empty_long_container {
        constexpr empty_long_container() noexcept = default;

        explicit empty_long_container(const volatile empty_long_container*) noexcept {}

        void copy_longs(volatile empty_long_container*) const noexcept {}
    };

    template <std::size_t Sz>
    struct long_container {
        constexpr long_container() noexcept = default;

        explicit long_container(const volatile long_container* o) noexcept {
            constexpr_array_copy<0, Sz>(data32, o->data32);
        }

        void copy_longs(volatile long_container* to) const noexcept {
            constexpr_array_copy<0, Sz>(to->data32, data32);
        }

        uint32 data32[Sz];
    };

    struct empty_short_container {
        constexpr empty_short_container() noexcept = default;

        explicit empty_short_container(const volatile empty_short_container*) noexcept {}

        void copy_shorts(volatile empty_short_container*) const noexcept {}
    };

    template <std::size_t Sz>
    struct [[gnu::packed]] short_container {
        constexpr short_container() noexcept = default;

        explicit short_container(const volatile short_container* o) noexcept {
            constexpr_array_copy<0, Sz>(data16, o->data16);
        }

        void copy_shorts(volatile short_container* to) const noexcept {
            constexpr_array_copy<0, Sz>(to->data16, data16);
        }

        uint16 data16[Sz];
    };

    struct empty_byte_container {
        constexpr empty_byte_container() noexcept = default;

        explicit empty_byte_container(const volatile empty_byte_container*) noexcept {}

        void copy_bytes(volatile empty_byte_container*) const noexcept {}
    };

    template <std::size_t Sz>
    struct [[gnu::packed]] byte_container {
        constexpr byte_container() noexcept = default;

        explicit byte_container(const volatile byte_container* o) noexcept {
            constexpr_array_copy<0, Sz>(data8, o->data8);
        }

        void copy_bytes(volatile byte_container* to) const noexcept {
            constexpr_array_copy<0, Sz>(to->data8, data8);
        }

        uint8 data8[Sz];
    };

} // namespace detail

template <typename Type, std::size_t Longs = sizeof(Type) / 4, std::size_t Shorts = (sizeof(Type) - (Longs * 4)) / 2, std::size_t Bytes = sizeof(Type) - (Longs * 4) - (Shorts * 2)>
struct [[gnu::packed]] bit_container :
        std::conditional_t<Longs == 0, detail::empty_long_container, detail::long_container<Longs>>,
        std::conditional_t<Shorts == 0, detail::empty_short_container, detail::short_container<Shorts>>,
        std::conditional_t<Bytes == 0, detail::empty_byte_container, detail::byte_container<Bytes>>
{
    using longs_type = std::conditional_t<Longs == 0, detail::empty_long_container, detail::long_container<Longs>>;
    using shorts_type = std::conditional_t<Shorts == 0, detail::empty_short_container, detail::short_container<Shorts>>;
    using bytes_type = std::conditional_t<Bytes == 0, detail::empty_byte_container, detail::byte_container<Bytes>>;

    using value_type = Type;

    constexpr bit_container() noexcept = default;

    constexpr bit_container(const bit_container&& o) noexcept : longs_type(o), shorts_type(o), bytes_type(o) {}

    constexpr explicit bit_container(const value_type& o) noexcept :
#if __cpp_lib_bit_cast
        bit_container(std::move(std::bit_cast<const bit_container>(o)))
#elseif __has_builtin(__builtin_bit_cast)
        bit_container(std::move(__builtin_bit_cast(const bit_container, o)))
#else
        bit_container(std::move(*reinterpret_cast<const bit_container*>(&o)))
#endif
    {}

    explicit bit_container(const volatile bit_container* o) noexcept : longs_type(o), shorts_type(o), bytes_type(o) {}

    void operator=(const bit_container& from) volatile noexcept {
        from.copy_longs(this);
        from.copy_shorts(this);
        from.copy_bytes(this);
    }

    [[nodiscard]]
    explicit operator value_type() const volatile noexcept {
#if __cpp_lib_bit_cast
        return std::bit_cast<const value_type>(*this);
#elseif __has_builtin(__builtin_bit_cast)
        return __builtin_bit_cast(const value_type, *this);
#else
        const bit_container temp{this};
#   pragma GCC diagnostic push
#   pragma GCC diagnostic ignored "-Wstrict-aliasing"
        return *reinterpret_cast<const value_type*>(&temp);
#   pragma GCC diagnostic pop
#endif
    }

    template <typename... Args>
    [[nodiscard]]
    static bit_container construct(Args&&... args) noexcept {
#if __cpp_lib_bit_cast
        return bit_container(std::move(std::bit_cast<const bit_container>(Type{args...})));
#elseif __has_builtin(__builtin_bit_cast)
        return bit_container(std::move(__builtin_bit_cast(const bit_container, Type{args...})));
#else
        const Type temp{args...};
        return bit_container(std::move(*reinterpret_cast<const bit_container*>(&temp)));
#endif
    }
};

template <class...>
constexpr std::false_type always_false{};

} // namespace gba::util

#endif // define GBAXX_UTIL_BIT_CONTAINER_HPP
