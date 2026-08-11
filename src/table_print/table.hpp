#pragma once

#include <algorithm>
#include <concepts>
#include <functional>
#include <iomanip>
#include <initializer_list>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

struct TableStyle {
    std::string_view vertical;
    std::string_view horizontal;

    std::string_view top_left;
    std::string_view top_cross;
    std::string_view top_right;

    std::string_view middle_left;
    std::string_view middle_cross;
    std::string_view middle_right;

    std::string_view bottom_left;
    std::string_view bottom_cross;
    std::string_view bottom_right;
};

inline constexpr TableStyle Unicode{
    "│", "─",
    "┌", "┬", "┐",
    "├", "┼", "┤",
    "└", "┴", "┘"
};

inline constexpr TableStyle Markdown{
    "|", "-",
    "|", "|", "|",
    "|", "|", "|",
    "|", "|", "|"
};

inline constexpr TableStyle Simple{
    " ", " ",
    " ", " ", " ",
    " ", " ", " ",
    " ", " ", " "
};

template<typename T>
class Table {
public:
    using ColumnType = std::pair<
        std::string_view,
        std::function<std::string(const T&)>
    >;

    Table(std::initializer_list<ColumnType> getters)
        : getters_(getters) {
    }

    template<std::ranges::input_range R>
    requires std::same_as<std::ranges::range_value_t<R>, T>
    void print(
        const R& data,
        int default_width = 20,
        TableStyle style = Unicode
    ) const {
        if (getters_.empty()) {
            return;
        }

        std::vector<int> widths(getters_.size() + 1, default_width);
        std::vector<std::vector<std::string>> rows;

        if constexpr (std::ranges::sized_range<R>) {
            rows.reserve(std::ranges::size(data));
        }

        for (size_t i = 1; i < getters_.size(); ++i) {
            widths[i] = std::max(
                default_width,
                static_cast<int>(getters_[i].first.size())
            );
        }

        for (const auto& [item, ind] : std::ranges::views::zip(data, 
            std::ranges::views::iota(1) | 
            std::ranges::views::transform([] (int i) { return std::to_string(i); }))) {
            std::vector<std::string> row;
            row.reserve(getters_.size() + 1);
            
            row.push_back(ind);

            for (size_t i = 0; i < getters_.size(); ++i) {
                std::string value = getters_[i].second(item);

                widths[i + 1] = std::max(
                    widths[i + 1],
                    static_cast<int>(value.size())
                );

                row.push_back(std::move(value));
            }

            rows.push_back(std::move(row));
        }

        constexpr int padding = 2;

        for (int& width : widths) {
            width += padding;
        }

        print_border(
            widths,
            style.top_left,
            style.top_cross,
            style.top_right,
            style
        );

        print_header(widths, style);

        print_border(
            widths,
            style.middle_left,
            style.middle_cross,
            style.middle_right,
            style
        );

        for (const auto& row : rows) {
            print_row(row, widths, style);
        }

        print_border(
            widths,
            style.bottom_left,
            style.bottom_cross,
            style.bottom_right,
            style
        );
    }

private:
    void print_header(
        const std::vector<int>& widths,
        TableStyle style
    ) const {
        std::cout << style.vertical << ' '
                      << std::left
                      << std::setw(widths[0] - 1)
                      << "Ind";
        for (size_t i = 0; i < getters_.size(); ++i) {
            std::cout << style.vertical << ' '
                      << std::left
                      << std::setw(widths[i + 1] - 1)
                      << getters_[i].first;
        }

        std::cout << style.vertical << '\n';
    }

    static void print_row(
        const std::vector<std::string>& row,
        const std::vector<int>& widths,
        TableStyle style
    ) {
        for (size_t i = 0; i < row.size(); ++i) {
            std::cout << style.vertical << ' '
                      << std::left
                      << std::setw(widths[i] - 1)
                      << row[i];
        }

        std::cout << style.vertical << '\n';
    }

    static void print_border(
        const std::vector<int>& widths,
        std::string_view left,
        std::string_view cross,
        std::string_view right,
        TableStyle style
    ) {
        std::cout << left;

        for (size_t i = 0; i < widths.size(); ++i) {
            for (int j = 0; j < widths[i]; ++j) {
                std::cout << style.horizontal;
            }

            std::cout << (i + 1 == widths.size() ? right : cross);
        }

        std::cout << '\n';
    }

private:
    std::vector<ColumnType> getters_;
};
