#ifndef EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_STORAGE_CONCEPTS_H_
#define EMU_MANAGER_INCLUDE_EMU_PERSISTENCE_STORAGE_CONCEPTS_H_

#include <concepts>
#include <filesystem>
#include <ranges>
#include <utility>
#include <vector>

namespace emu {

template <typename Container, typename T>
concept IsSequenceContainer =
    std::ranges::forward_range<Container> &&
    std::same_as<std::ranges::range_value_t<Container>, T> &&
    std::ranges::sized_range<Container> && requires(Container cont, T value) {
      cont.push_back(std::move(value));
      cont.clear();
    };

}

#endif
