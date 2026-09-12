#ifndef ETELEMETRY_LOGGER_HPP
#define ETELEMETRY_LOGGER_HPP

#ifndef ET_ENABLED

#define ET_DECLARE_LOGGER()
#define a_ET_INIT_LOGGER(name)

#define ET_LOG_INFO(context_identifier, parent_identifier, what)

#else

#include "opentelemetry/logs/provider.h"

#include "etelemetry/common.hpp"

#define ET_DECLARE_LOGGER()\
::opentelemetry::nostd::shared_ptr<::opentelemetry::logs::Logger> _ET_PREPEND(_logger)

#define a_ET_INIT_LOGGER(name)\
, _ET_PREPEND(_logger){\
	::opentelemetry::logs::Provider::GetLoggerProvider()\
		->GetLogger(name)\
}

#define ET_LOG_INFO(context_identifier, parent_identifier, what_data)\
context_identifier._ET_PREPEND(_logger)->Info(\
	::opentelemetry::nostd::string_view{ what_data },\
	_ET_PREPEND(parent_identifier)->GetContext())

#endif

#endif
