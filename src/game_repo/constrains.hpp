#pragma once

#include <concepts>
#include <filesystem>
#include <vector>
#include <ranges>

template<typename Container, typename T>
concept IsSequenceContainer = std::ranges::forward_range<Container> &&
    std::same_as<std::ranges::range_value_t<Container>, T> &&
    std::ranges::sized_range<Container> &&
    requires(Container cont, T value) {
        cont.push_back(std::move(value));
        cont.clear();
    };

template<typename Storage, typename T, typename Container>
concept IsStorage = std::constructible_from<Storage, const std::filesystem::path&> &&
    IsSequenceContainer<Container, T> &&
    requires(const Storage& storage, const Container& data) {
        { storage.Save(data) } -> std::same_as<void>;
        { storage.Load() } -> std::same_as<Container>;
    };

