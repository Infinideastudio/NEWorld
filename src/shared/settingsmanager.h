#ifndef SETTINGSMANAGER_H__
#define SETTINGSMANAGER_H__
#include <map>
#include <string>
#include <boost/any.hpp>

class Settings
{
public:
    //ʹ���ļ�����ʼ��Settings���Զ��Ӹ��ļ���ȡ����
    Settings(std::string filename);

    //�������ļ����浽��ȡ���ļ�
    void save();

    //�������ļ��л�ȡ����
    template<class T>
    T get(std::string key);

    //����ĳһ������
    template<class T>
    void set(std::string key, const T& value);

private:
    using SettingsMap = std::map<std::string, boost::any>;
    SettingsMap m_settings;
    std::string m_filename;

    //���ļ���ȡ����
    static Settings readFromFile(std::ifstream& file);

    //������д�뵽�ļ�
    static void writeToFile(std::ofstream& file, Settings settings);
};


#endif // SETTINGSMANAGER_H__
