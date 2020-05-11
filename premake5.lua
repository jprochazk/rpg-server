include("conanbuildinfo.premake.lua")

workspace "rpg-server"
	configurations { "Debug", "Release" }
	
	conan_basic_setup()

	project "server"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		pchheader "pch.h"
		pchsource "src/pch.cpp"

		files { 
			"src/**.h", 
			"src/**.cpp" 
		}

		includedirs {
			"src"
		}

		linkoptions { conan_exelinkflags }

		-- boost uses templates heavily
		-- which causes msvc to complain about too many sections in object files
		filter "system:windows" 
			buildoptions { "/bigobj", "/W4" }

		filter "system:linux"
			buildoptions { "-Wall", "-Wextra", "-Wpedantic" }
			
		filter "configurations:Debug"
			defines { "DEBUG" }
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"