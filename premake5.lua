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
IncludeDir["GLFW"]  = "Ngin/vendor/GLFW/include"
IncludeDir["GLAD"]  = "Ngin/vendor/GLFW/deps"
IncludeDir["ImGui"]         = "Ngin/vendor/imgui"
IncludeDir["ImGuiBackends"] = "Ngin/vendor/imgui/backends"

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
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/imgui/imgui.h",
		"%{prj.name}/vendor/imgui/imgui_internal.h",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.h",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.h",
		"%{prj.name}/vendor/imgui/imgui.cpp",
		"%{prj.name}/vendor/imgui/imgui_draw.cpp",
		"%{prj.name}/vendor/imgui/imgui_tables.cpp",
		"%{prj.name}/vendor/imgui/imgui_widgets.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_glfw.cpp",
		"%{prj.name}/vendor/imgui/backends/imgui_impl_opengl3.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/src",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.GLAD}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiBackends}"
	}

	filter "files:Ngin/src/Platforms/OpenGL/Glad.cpp"
		flags { "NoPCH" }
	filter {}

	filter "files:Ngin/vendor/imgui/backends/**.cpp or Ngin/vendor/imgui/**.cpp"
		flags { "NoPCH" }
	filter {}

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
			"NG_BUILD_DLL",
			"IMGUI_API=__declspec(dllexport)"
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
		"Ngin/src",
		"%{IncludeDir.ImGui}"
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
			"NG_PLATFORM_WINDOWS",
			"IMGUI_API=__declspec(dllimport)"
		}

	filter "configurations:Debug"
		defines "NG_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "NG_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "NG_DIST"
