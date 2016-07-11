#include <uilib.h>
namespace UI
{
    namespace Globalization
    {

        inline Str::Str(const char* str, bool globalizationNeed)
            : ds(str), gs(globalizationNeed? Service::getInstance().getStr(ds):ds) {}

        inline Str::operator const char*() const
        {
            return gs.c_str();
        }

        inline void Str::flush()
        {
            gs = Service::getInstance().getStr(ds);
        }

        void Service::attachLangFiles(std::initializer_list<const char*> filenames)
        {
            for (auto& s : filenames)
            {
                langs.insert(std::make_pair(std::string(s), Data::SimpleMapFile(basePath + s + suffix)));
            }
        }

        const std::string Service::getCurLang() const
        {
            return curLang;
        }

        void Service::setLang(const std::string & _lang)
        {
            auto result = langs.find(_lang);
            if (result!=langs.end())
            {
                curLang = _lang;
                curLangData = &(result->second);
            }
            else
            {
                logwarning("Language file " + _lang + "does not exist");
            }
        }

        const std::string Service::getStr(const std::string & key) const
        {
            if (!curLangData) return key;
            return curLangData->get<std::string>(key);
        }

        inline void Service::setBasePath(std::string _basePath)
        {
            basePath = _basePath;
        }

        inline std::string Service::getBasePath()
        {
            return basePath;
        }
        inline void Service::setSuffix(std::string _suffix) { suffix = _suffix; }
        inline std::string Service::getSuffix() { return suffix; }
    }
}
