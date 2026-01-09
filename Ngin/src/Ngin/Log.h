#pragma once

#include "Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace Ngin {
	class NG_API Log
	{
	public:
		static void Init();
		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
			static std::shared_ptr<spdlog::logger> s_CoreLogger;
			static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}

// Core log setups
#define NG_CORE_TRACE(...)   ::Ngin::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define NG_CORE_INFO(...)    ::Ngin::Log::GetCoreLogger()->info(__VA_ARGS__)
#define NG_CORE_WARN(...)    ::Ngin::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define NG_CORE_ERROR(...)   ::Ngin::Log::GetCoreLogger()->error(__VA_ARGS__)
#define NG_CORE_FATAL(...)   ::Ngin::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log setups
#define NG_TRACE(...)   ::Ngin::Log::GetClientLogger()->trace(__VA_ARGS__)
#define NG_INFO(...)    ::Ngin::Log::GetClientLogger()->info(__VA_ARGS__)
#define NG_WARN(...)    ::Ngin::Log::GetClientLogger()->warn(__VA_ARGS__)
#define NG_ERROR(...)   ::Ngin::Log::GetClientLogger()->error(__VA_ARGS__)
#define NG_FATAL(...)   ::Ngin::Log::GetClientLogger()->fatal(__VA_ARGS__)

