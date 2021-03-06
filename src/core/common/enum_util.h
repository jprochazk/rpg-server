#pragma once

#include <type_traits>

namespace common {

template <typename Enum>
constexpr auto to_underlying(Enum value) noexcept
{
    return static_cast<std::underlying_type_t<Enum>>(value);
}

} // namespace common