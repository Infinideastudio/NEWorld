#include <uilib.h>

namespace UI
{
    namespace Data
    {
        SimpleMapFile::SimpleMapFile(const std::string& filename)
            :m_settings(), m_filename(filename)
        {
            std::ifstream file(m_filename);
            try
            {
                if (file.good())
                {
                    while (!file.eof())
                    {
                        std::string line;
                        std::getline(file, line);
                        auto vk = split(line, "=");
                        if (vk.size() != 2) continue;
                        std::string key = vk[0], value = vk[1];
                        trim(key);
                        trim(value);
                        m_settings[key] = Convert::StrToAny(value);
                    }
                }
                else
                    throw SimpleMapFileExceptions::IOFailureOnRead;
            }
            catch (SimpleMapFileExceptions)
            {
                logerror("Failed To Load File \"" + filename + "\",May Be Caused By Illegal Path");
            }
            file.close();
        }

        //�������ļ����浽��ȡ���ļ�
        void SimpleMapFile::save()
        {
            std::ofstream file(m_filename);
            try
            {
                if (file.good())
                {
                    for (const auto& p : m_settings)
                    {
                        file << p.first << " = " << Convert::ToString(p.second) << std::endl;
                    }
                }
                else
                    throw SimpleMapFileExceptions::IOFailureOnWrite;
            }
            catch (SimpleMapFileExceptions)
            {
                logerror("Failed To Load File \"" + m_filename + "\".Is The Target A Read-Only File?");
            }
            file.close();
        }

        boost::spirit::hold_any SimpleMapFile::get(const std::string& key) const
        {
            auto result = m_settings.find(key);
            if (result != m_settings.cend()) return result->second;
            return boost::spirit::hold_any();
        }

        //����ĳһ������
        void SimpleMapFile::set(const std::string& key, const boost::spirit::hold_any value)
        {
            m_settings[key] = value;
        }

        bool SimpleMapFile::has(const std::string& key) const
        {
            return m_settings.find(key) != m_settings.cend();
        }

        const AnyTMap & SimpleMapFile::getMap()
        {
            return m_settings;
        }

    }
}