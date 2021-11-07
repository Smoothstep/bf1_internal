#pragma once

#include <sstream>
#include <io.h>
#include <fstream>
#include <iostream>
#include <array>

enum ELogLevel
{
	LogInfo,
	LogWarning,
	LogError,
	LogFatal,
	LogNum
};

#ifdef _DEBUG
constexpr bool IsDebugMode = true;
#else
constexpr bool IsDebugMode = false;
#endif

class LoggerBase
{
public:
	struct EchoStream
	{
		std::wostream& Strm;
		ELogLevel Level;

		template<class T>
		EchoStream& operator<<(T&& Value)
		{
			Strm << std::forward<T>(Value);
			return *this;
		}
	};

	class ScopedLogLine
	{
		LoggerBase::EchoStream& Strm;
		std::wstringstream Input;

	public:
		ScopedLogLine(ScopedLogLine&) = delete;
		ScopedLogLine(ScopedLogLine&&) = delete;
		ScopedLogLine(LoggerBase& Log)
			: Strm{ Log.InputStream }
		{
			constexpr std::wstring_view LogLevelStrings[] =
			{
				L"Info",
				L"Warn",
				L"Error",
				L"Fatal"
			};

			Input << "[" << LogLevelStrings[Strm.Level] << "] ";
		}

		~ScopedLogLine()
		{
			Strm << Input.str();
		}

		ScopedLogLine& operator <<(const char* Log)
		{
			Input << Log;
			return *this;
		}

		ScopedLogLine& operator <<(ScopedLogLine& (*Function)(ScopedLogLine& Log))
		{
			return Function(*this);
		}

		ScopedLogLine& operator <<(const std::wstring_view Log)
		{
			Input << Log;
			return *this;
		}

		ScopedLogLine& operator <<(const std::string_view Log)
		{
			Input << std::wstring(Log.begin(), Log.end());
			return *this;
		}

		ScopedLogLine& DoEndLine()
		{
			Input << std::endl;
			return *this;
		}
	};

public:
	LoggerBase(EchoStream&& Strm)
		: InputStream{ std::move(Strm) }
	{}

protected:
	EchoStream InputStream;
};

class Logger : public LoggerBase
{
private:
	static inline LoggerBase LogInstance[ELogLevel::LogNum] =
	{
		{ EchoStream{ std::wcout, ELogLevel::LogInfo } },
		{ EchoStream{ std::wcout, ELogLevel::LogWarning } },
		{ EchoStream{ std::wcerr, ELogLevel::LogError } },
		{ EchoStream{ std::wcerr, ELogLevel::LogFatal } }
	};

public:
	template<ELogLevel Level = ELogLevel::LogInfo>
	static ScopedLogLine Log()
	{
		return { LogInstance[Level] };
	}

	static ScopedLogLine Info()
	{
		return { LogInstance[ELogLevel::LogInfo] };
	}

	static ScopedLogLine Fatal()
	{
		return { LogInstance[ELogLevel::LogFatal] };
	}

	static ScopedLogLine Warn()
	{
		return { LogInstance[ELogLevel::LogWarning] };
	}

	static ScopedLogLine Error()
	{
		return { LogInstance[ELogLevel::LogError] };
	}

	static ScopedLogLine& EndLine(ScopedLogLine& Log)
	{
		return Log << "\n";
	}

	template<ELogLevel Level = ELogLevel::LogInfo, class... Arguments>
	static ScopedLogLine& Log(Arguments&&... Args)
	{
		return (LogInstance[Level] << ... << Args);
	}

	Logger() = delete;
	Logger(const Logger& Other) = default;
};