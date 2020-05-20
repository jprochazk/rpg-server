include("conanbuildinfo.premake.lua")

workspace "rpg-server"
	configurations { "Release" }
	
	conan_basic_setup()
	
	project "core"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		files { 
			"src/core/**.h",
			"src/core/**.cpp"
		}

		includedirs {
			"src/core"
		}

		linkoptions { conan_exelinkflags }

		filter "system:windows" 
			-- msvc complains about object file size
			buildoptions { "/bigobj" }

		filter "system:linux"
			buildoptions { "-Wall", "-Wextra" }

		filter "configurations:Release"
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"

	project "server"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		files {
			"src/bin/**.h",
			"src/bin/**.cpp"
		}

		includedirs {
			"src/core"
		}

		links {
			"core"
		}

		filter "system:windows" 
			buildoptions { "/bigobj" }

		filter "system:linux"
			buildoptions { "-Wall", "-Wextra" }

		filter "configurations:Release"
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"

	project "test"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"

		targetdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}")
		objdir ("bin/%{cfg.buildcfg}/%{cfg.system}-%{cfg.architecture}/%{prj.name}/obj")

		files {
			"src/test/**.h",
			"src/test/**.cpp"
		}

		includedirs {
			"src/core"
		}

		links {
			"core"
		}

		filter "system:windows" 
			buildoptions { "/bigobj" }

		filter "system:linux"
			buildoptions { "-Wall", "-Wextra" }

		filter "configurations:Release"
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"
