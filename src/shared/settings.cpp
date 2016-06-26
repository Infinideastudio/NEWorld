#include "settingsmanager.h"
#include "logger.h"
#include "common.h"
void loadSharedSettings(Settings& settings)
{
#ifdef NEWORLD_DEBUG
    Logger::clogLevel = settings.get<int>("shared.logger.clogLevel", Logger::trace);
    Logger::cerrLevel = settings.get<int>("shared.logger.cerrLevel", Logger::fatal);
    Logger::fileLevel = settings.get<int>("shared.logger.fileLevel", Logger::trace);
    Logger::lineLevel = settings.get<int>("shared.logger.lineLevel", Logger::warning);
#else
    Logger::clogLevel = settings.get<int>("shared.logger.clogLevel", Logger::info);
    Logger::cerrLevel = settings.get<int>("shared.logger.cerrLevel", Logger::fatal);
    Logger::fileLevel = settings.get<int>("shared.logger.fileLevel", Logger::info);
    Logger::lineLevel = settings.get<int>("shared.logger.lineLevel", Logger::warning);
#endif
    settings.setMinimal(settings.get<bool>("shared.settings.minimal", false));
}