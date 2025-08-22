#pragma once

#include <string>
#include <map>
#include <variant>

#include "piece.hpp"

class SettingsProvider {

    using Setting = std::variant<int, bool, std::string, e_color>;
    using SettingsMap = std::map<std::string, Setting>;

public:
    SettingsProvider() {};
    SettingsProvider(std::string settings_path);

    void read_file(std::string path = "");
    void save_to_file(std::string path = "");
    
    template<typename T>
    T get_setting(std::string key);
    template<typename T>
    void set_setting(std::string key, T value);


private:
    void generate_default_settings();
    std::string setting_to_string(Setting setting);
    void string_to_setting(Setting& default_setting, std::string new_setting);

    SettingsMap f_settings;
    std::string f_settings_path;
};


template<typename T>
T SettingsProvider::get_setting(std::string key) 
{
    if (f_settings.contains(key)) {
        if (T* pval = std::get_if<T>(&f_settings[key]))
            return *pval;
        throw std::invalid_argument("Setting has different type");
    }
    throw std::invalid_argument("Setting does not exist");
}

template<typename T>
void SettingsProvider::set_setting(std::string key, T value) 
{
    f_settings[key] = value;
}