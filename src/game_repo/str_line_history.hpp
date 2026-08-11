#pragma once

#include <string>
#include <chrono>
#include <nlohmann/json.hpp>

struct StrLineHistory {
    std::string command{};
    std::chrono::system_clock::time_point timestamp;

    StrLineHistory() = default;

    StrLineHistory(std::string line) : 
        command(std::move(line)), timestamp(std::chrono::system_clock::now()) {}

    StrLineHistory(std::string line, std::chrono::system_clock::time_point time) : 
        command(std::move(line)), timestamp(time) {}
    
    StrLineHistory(int argc, char** argv) : timestamp(std::chrono::system_clock::now()) {
        for (int i = 1; i < argc; ++i) {
            command += argv[i];
            if (i < argc - 1) {
                command += " ";
            }
        }
    }
};

inline void to_json(nlohmann::json& j, const StrLineHistory& h) {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(
        h.timestamp.time_since_epoch()
    ).count();
    j = nlohmann::json{{"command", h.command}, {"timestamp", seconds}};
}

inline void from_json(const nlohmann::json& j, StrLineHistory& h) {
    j.at("command").get_to(h.command);
    long long seconds = j.at("timestamp").get<long long>();
    h.timestamp = std::chrono::system_clock::time_point(std::chrono::seconds(seconds));
}
