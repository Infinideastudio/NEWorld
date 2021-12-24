#pragma once
#include <string>
#include <sstream>
#include <unordered_map>

class GameSettings {
public:
	GameSettings() {
        loadOptions();
	}

	static GameSettings& getInstance() {
		static GameSettings settings;
		return settings;
	}

    void loadOptions();

	void saveOptions();

private:
    template<typename T>
    void loadOption(std::unordered_map<std::string, std::string>& m, const char* name, T& value) {
        if (m.find(name) == m.end()) return;
        std::stringstream ss;
        ss << m[name];
        ss >> value;
    }

    template<typename T>
    void saveOption(std::ostream& out, const char* name, T& value) {
        out << std::string(name) << " " << value << std::endl;
    }
};
