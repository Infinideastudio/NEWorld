#ifndef SETTINGSMANAGER_H__
#define SETTINGSMANAGER_H__
#include <map>
#include <string>
#include <boost/any.hpp>

class Settings
{
public:
    //使用文件名初始化Settings，自动从该文件读取配置
    Settings(std::string filename);

    //将配置文件保存到读取的文件
    void save();

    //从配置文件中获取配置
    template<class T>
    T get(std::string key);

    //设置某一项配置
    template<class T>
    void set(std::string key, const T& value);

private:
    using SettingsMap = std::map<std::string, boost::any>;
    SettingsMap m_settings;
    std::string m_filename;

    //从文件读取配置
    static Settings readFromFile(std::ifstream& file);

    //把配置写入到文件
    static void writeToFile(std::ofstream& file, Settings settings);
};


#endif // SETTINGSMANAGER_H__
