//@	{
//@	 "targets":[{"name":"logger.hpp","type":"include"}]
//@	,"dependencies_extra":[{"ref":"logger.o", "rel":"implementation"}]
//@	}

#ifndef TEXPAINTER_LOG_LOGGER_HPP
#define TEXPAINTER_LOG_LOGGER_HPP

#include <string>
#include <cstdio>

namespace Texpainter::Logger
{
	enum class MessageType : int
	{
		Info,
		Warning,
		Error
	};

	constexpr char const* toCstring(MessageType type)
	{
		switch(type)
		{
			case MessageType::Info: return "(i)";
			case MessageType::Warning: return "(!)";
			case MessageType::Error: return "(x)";

			default: __builtin_unreachable();
		}
	}

	namespace detail
	{
		using LogFunction = void (*)(void*, MessageType, std::string&&);

		void log(MessageType type, std::string&& message);
		void logger(LogFunction, void*);
	}

	template<class Item>
	void log(MessageType type, Item&& msg)
	{
		detail::log(type, toString(msg));
	}

	inline void log(MessageType type, std::string&& msg) { detail::log(type, std::move(msg)); }

	inline void log(MessageType type, std::string const& msg)
	{
		detail::log(type, std::string{msg});
	}

	inline void log(MessageType type, char const* msg) { detail::log(type, std::string{msg}); }

	template<class Logger>
	void logger(Logger& logger)
	{
		detail::logger(
		    [](void* ptr, MessageType type, std::string&& msg) {
			    write(*static_cast<Logger*>(ptr), type, std::move(msg));
		    },
		    &logger);
	}

	void logger(FILE* stdstream);
}

#endif