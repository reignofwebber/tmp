#ifndef LOG_H_
#define LOG_H_

// std
#include <string>

#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/sinks.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup.hpp>

#define log_trace   BOOST_LOG_TRIVIAL(trace) << "<< " /* << __FILE__ << "|" */ << __FUNCTION__ << " >>:"
#define log_info    BOOST_LOG_TRIVIAL(info) << "<< " /* << __FILE__ << "|" */ << __FUNCTION__ << " >>:"
#define log_warning BOOST_LOG_TRIVIAL(warning) << "<< " /* << __FILE__ << "|" */ << __FUNCTION__ << " >>:"
#define log_error   BOOST_LOG_TRIVIAL(error) << "<< " /* << __FILE__ << "|" */ << __FUNCTION__ << " >>:"
#define log_fatal   BOOST_LOG_TRIVIAL(fatal) << "<< " /* << __FILE__ << "|" */ << __FUNCTION__ << " >>:"



void initLog();


#endif  // LOG_H_
