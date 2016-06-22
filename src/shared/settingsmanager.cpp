#include "precomp.h"
#include "settingsmanager.h"

Settings::Settings(std::string filename)
{
}

void Settings::save()
{
}

Settings Settings::readFromFile(std::ifstream & file)
{
    return Settings();
}

void Settings::writeToFile(std::ifstream & file, Settings settings)
{
}

template<class T>
T Settings::get(std::string key)
{
    return T();
}

template<class T>
void Settings::set(std::string key, const T & value)
{

}
