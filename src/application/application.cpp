#include "application.hpp"

Application::Application(std::string name,
    const std::filesystem::path& config_path) :
    cli_{std::move(name)}, context_{Config{config_path}.GetData()},
    comand_manager_{context_} {

    comand_manager_.Register(cli_);
}

void Application::Run(int argc, char** argv) {
    try {
        cli_.parse(argc, argv);
        comand_manager_.RunSelected();
    } catch (const CLI::ParseError& e) {
        cli_.exit(e);
    }
}