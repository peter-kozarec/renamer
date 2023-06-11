#ifndef RENAMER_LOGGER_HPP
#define RENAMER_LOGGER_HPP

#include <boost/log/utility/setup/console.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>


inline void initialize_logger()
{
    boost::log::add_console_log(std::cout, boost::log::keywords::format = "%Message%");

#ifdef DEBUG
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
#else
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif // NDEBUG
}

inline void change_logger_severity(boost::log::trivial::severity_level severity)
{
    boost::log::core::get()->set_filter(boost::log::trivial::severity >= severity);
}

#define BOOST_LOG_TRACE BOOST_LOG_TRIVIAL(trace)
#define BOOST_LOG_DEBUG BOOST_LOG_TRIVIAL(debug)
#define BOOST_LOG_INFO BOOST_LOG_TRIVIAL(info)
#define BOOST_LOG_WARNING BOOST_LOG_TRIVIAL(warning)
#define BOOST_LOG_FATAL BOOST_LOG_TRIVIAL(fatal)

#endif // RENAMER_LOGGER_HPP