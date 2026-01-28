workspace "Ngin"
	architecture "x64"
	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	filter "action:vs*"
		buildoptions { "/utf-8" }
	filter {}
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "Ngin/vendor/GLFW/include"

include "Ngin/vendor/GLFW"
	
project "Ngin"
	location "Ngin"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "ngpch.h"
	pchsource "Ngin/src/ngpch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
    	"%{prj.name}/src",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"NG_PLATFORM_WINDOWS",
			"NG_BUILD_DLL"
		}

		postbuildcommands
		{
			("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox/")
		}

	filter "configurations:Debug"
		defines "NG_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "NG_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "NG_DIST"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Ngin/vendor/spdlog/include",
		"Ngin/src"
	}

	links
	{
		"Ngin"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "Off"
		systemversion "latest"

		defines
		{
			"NG_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "NG_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "NG_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "NG_DIST"
