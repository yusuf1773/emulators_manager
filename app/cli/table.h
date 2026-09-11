#ifndef EMU_MANAGER_APP_CLI_TABLE_H_
#define EMU_MANAGER_APP_CLI_TABLE_H_

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace emu {

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

inline constexpr TableStyle kUnicode{"│", "─", "┌", "┬", "┐", "├",
                                     "┼", "┤", "└", "┴", "┘"};

inline constexpr TableStyle kMarkdown{"|", "-", "|", "|", "|", "|",
                                      "|", "|", "|", "|", "|"};

inline constexpr TableStyle kSimple{" ", " ", " ", " ", " ", " ",
                                    " ", " ", " ", " ", " "};

template <typename T>
class Table {
 public:
  using ColumnType =
      std::pair<std::string_view, std::function<std::string(const T&)>>;

  explicit Table(std::initializer_list<ColumnType> getters)
      : getters_(getters) {}

  template <std::ranges::input_range R>
    requires std::same_as<std::ranges::range_value_t<R>, T>
  void Print(const R& data, int default_width = 20,
             TableStyle style = kUnicode) const {
    if (getters_.empty()) {
      return;
    }

    std::vector<int> widths(getters_.size() + 1, default_width);
    std::vector<std::vector<std::string>> rows;

    if constexpr (std::ranges::sized_range<R>) {
      rows.reserve(std::ranges::size(data));
    }

    for (std::size_t i = 1; i < getters_.size(); ++i) {
      widths[i] =
          std::max(default_width, static_cast<int>(getters_[i].first.size()));
    }

    for (const auto& [item, ind] : std::ranges::views::zip(
             data, std::ranges::views::iota(1) |
                       std::ranges::views::transform(
                           [](int i) { return std::to_string(i); }))) {
      std::vector<std::string> row;
      row.reserve(getters_.size() + 1);

      row.push_back(ind);

      for (std::size_t i = 0; i < getters_.size(); ++i) {
        std::string value = getters_[i].second(item);

        widths[i + 1] = std::max(widths[i + 1], static_cast<int>(value.size()));

        row.push_back(std::move(value));
      }

      rows.push_back(std::move(row));
    }

    constexpr int kPadding = 2;

    for (int& width : widths) {
      width += kPadding;
    }

    PrintBorder(widths, style.top_left, style.top_cross, style.top_right,
                style);

    PrintHeader(widths, style);

    PrintBorder(widths, style.middle_left, style.middle_cross,
                style.middle_right, style);

    for (const auto& row : rows) {
      PrintRow(row, widths, style);
    }

    PrintBorder(widths, style.bottom_left, style.bottom_cross,
                style.bottom_right, style);
  }

 private:
  void PrintHeader(const std::vector<int>& widths, TableStyle style) const {
    std::cout << style.vertical << ' ' << std::left << std::setw(widths[0] - 1)
              << "Ind";
    for (std::size_t i = 0; i < getters_.size(); ++i) {
      std::cout << style.vertical << ' ' << std::left
                << std::setw(widths[i + 1] - 1) << getters_[i].first;
    }

    std::cout << style.vertical << '\n';
  }

  static void PrintRow(const std::vector<std::string>& row,
                       const std::vector<int>& widths, TableStyle style) {
    for (std::size_t i = 0; i < row.size(); ++i) {
      std::cout << style.vertical << ' ' << std::left
                << std::setw(widths[i] - 1) << row[i];
    }

    std::cout << style.vertical << '\n';
  }

  static void PrintBorder(const std::vector<int>& widths, std::string_view left,
                          std::string_view cross, std::string_view right,
                          TableStyle style) {
    std::cout << left;

    for (std::size_t i = 0; i < widths.size(); ++i) {
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

}

#endif
