/*
* NEWorld: A free game with similar rules to Minecraft.
* Copyright (C) 2016 NEWorld Team
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "logger.h"
#include "common.h"
void loggerInit()
{
    logging::formatter formatter =
        (
            expr::stream
            << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S:%f")
            << ": <" << logging::trivial::severity
            << "> " << expr::smessage
        );

    logging::add_common_attributes();

    auto console_sink = logging::add_console_log();
    auto file_sink = logging::add_file_log
                     (
                         keywords::file_name = "logs/NEWorld_%Y-%m-%d_%N.log",
                         keywords::rotation_size = 10 * 1024 * 1024,
                         keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0)
                     );

    file_sink->locked_backend()->set_file_collector(sinks::file::make_collector
            (
                keywords::target = "logs",
                keywords::max_size = 5 * 1024 * 1024,
                keywords::min_free_space = 10 * 1024 * 1024
            ));

    file_sink->locked_backend()->scan_for_files();

    console_sink->set_formatter(formatter);
    file_sink->set_formatter(formatter);
    file_sink->locked_backend()->auto_flush(true);

    logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
    logging::core::get()->add_sink(console_sink);
    logging::core::get()->add_sink(file_sink);
#ifndef NEWORLD_DEBUG
    logging::core::get()->set_filter
    (
        logging::trivial::severity >= logging::trivial::info
    );
#endif
}
