#include "logger.h"
#include "Common.h"
void loggerInit()
{
    logging::formatter formatter =
        (
            expr::stream
            << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
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
