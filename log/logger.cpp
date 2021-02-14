//@	{
//@	 "targets":[{"name":"logger.o","type":"object"}]
//@	}

#include "./logger.hpp"

namespace
{
	void* log_object;
	Texpainter::Logger::detail::LogFunction log_function;
}

void Texpainter::Logger::detail::log(MessageType type, std::string&& message)
{
	log_function(log_object, type, std::move(message));
}

void Texpainter::Logger::detail::logger(LogFunction f, void* log)
{
	log_object   = log;
	log_function = f;
}

void Texpainter::Logger::logger(FILE* f)
{
	detail::logger(
	    [](void* ptr, MessageType type, std::string&& msg) {
		    auto file = static_cast<FILE*>(ptr);
		    fprintf(file, "Texpainter: %s: %s\n", toCstring(type), msg.c_str());
	    },
	    f);
}