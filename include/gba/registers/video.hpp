/*
===============================================================================

 Copyright (C) 2022 gba-hpp contributors
 For conditions of distribution and use, see copyright notice in LICENSE.md

===============================================================================
*/

#ifndef GBAXX_REGISTERS_VIDEO_HPP
#define GBAXX_REGISTERS_VIDEO_HPP

#include <gba/ioregister.hpp>

#include <gba/registers/video_types.hpp>

namespace gba::reg {

struct dispcnt : io::register_ptr<dispcnt_type> {
    static constexpr auto address = 0x4000000;

    consteval dispcnt() noexcept : io::register_ptr<dispcnt_type>{address} {}

    static void reset() noexcept {
        io::register_emplace<dispcnt_type, address>();
    }

    static void set(dispcnt_type value) noexcept {
        io::register_write<decltype(value), address>(value);
    }

    static auto get() noexcept {
        return io::register_read<dispcnt_type, address>();
    }
};

struct dispstat : io::register_ptr<dispstat_type> {
    static constexpr auto address = 0x4000004;

    consteval dispstat() noexcept : io::register_ptr<dispstat_type>{address} {}

    static void reset() noexcept {
        io::register_emplace<dispstat_type, address>();
    }

    static void set(dispstat_type value) noexcept {
        io::register_write<decltype(value), address>(value);
    }

    static auto get() noexcept {
        return io::register_read<dispstat_type, address>();
    }
};

struct vcount : io::register_ptr<const uint16> {
    static constexpr auto address = 0x4000006;

    consteval vcount() noexcept : io::register_ptr<const uint16>{address} {}

    static auto get() noexcept {
        return io::register_read<uint16, address>();
    }
};

namespace undocumented {

    struct greenswap : io::register_ptr<bool> {
        static constexpr auto address = 0x4000002;

        consteval greenswap() noexcept : io::register_ptr<bool>{address} {}

        static void reset() noexcept {
            io::register_emplace<bool, address>();
        }

        static void set(bool value) noexcept {
            io::register_write<decltype(value), address>(value);
        }

        static auto get() noexcept {
            return io::register_read<bool, address>();
        }
    };

} // namespace undocumented
} // namespace gba::reg

#endif // define GBAXX_REGISTERS_VIDEO_HPP
