#include <iostream>
#include <fstream>
#include <filesystem>
#include <stdexcept>

#include "graphics.hpp"
#include "settings_provider.hpp"

SettingsProvider::SettingsProvider(std::string settings_path)
{
    f_settings_path = settings_path;
    read_file();
}

void SettingsProvider::read_file(std::string path)
{
    if (path.empty()) path = f_settings_path;
    generate_default_settings();

    std::ifstream settings_file(path);

    if(settings_file.is_open()) {
        std::string line;
        while(std::getline(settings_file, line)) {
            // parseline
            if (line.empty() || line[0] == '#') continue;
            auto pos = line.find('=');
            if (pos == std::string::npos) continue;

            std::string key = line.substr(0, pos);
            std::string val = line.substr(pos + 1);

            if (f_settings.contains(key)) {
                string_to_setting(f_settings[key], val);
            }
        }
        settings_file.close();
    }
    else {
        save_to_file(path);
    }
}


void SettingsProvider::string_to_setting(Setting& default_setting, std::string new_setting) {
    std::visit([&](auto &el) {
            using T = decltype(el);
            if constexpr (typeid(T) == typeid(int)) {
                el = std::stoi(new_setting);
            }
            else if constexpr (typeid(T) == typeid(bool)) {
                el = (new_setting == "true") ? true : false;
            }
            else if constexpr (typeid(T) == typeid(std::string)) {
                el = new_setting;
            }
            else if constexpr (typeid(T) == typeid(e_color)) {
                el = (new_setting == "White") ? white : black;
            }
            else {
                throw std::invalid_argument("Unknown setting type");
            }
        }, default_setting);
}

std::string SettingsProvider::setting_to_string(Setting setting)
{
    if (int* val = std::get_if<int>(&setting)) {
        return std::to_string(*val);
    }
    if (bool* val = std::get_if<bool>(&setting)) {
        return (*val) ? "true" : "false";
    }
    if (e_color* val = std::get_if<e_color>(&setting)) {
        return graphics::Texture_Operator::f_color_to_string[*val];
    }
    if (std::string* val = std::get_if<std::string>(&setting)) {
        return *val;
    }
    throw std::invalid_argument("Unknown setting type");
}

void SettingsProvider::save_to_file(std::string path)
{
    if (path.empty()) path = f_settings_path;
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream settings_file(path);

    if (settings_file.is_open()) {
        for (auto &pair: f_settings) {
            settings_file << pair.first + '=' + setting_to_string(pair.second) + '\n';
        }
        settings_file.close();
    }
    else {
        std::cout << "Cannot open file" << std::endl;
        return;
    }
}

void SettingsProvider::generate_default_settings()
{
    f_settings.clear();

    // window
    set_setting("window_width", 1024);
    set_setting("window_height", 768);

    // game
    set_setting("game_host_color", white);

    // engine
    set_setting("engine", true);
    set_setting("engine_skill_level", 0);
    set_setting("engine_moves_depth", 1);
    set_setting("engine_elo", 1320);
    set_setting("engine_path", "./res/engine/stockfish/stockfish-windows-x86-64-avx2.exe");

    // textures
    set_setting("textures_path", "./res/Textures/");
    set_setting("board_texture", "Board2.png");
}
