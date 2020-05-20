newoption {
	trigger = "build",
	value = "Build type",
	description = "Choose which configuration to use",
	allowed = {
		{ "debug", "Debug" },
		{ "release", "Release" }
	}
}

if not _OPTIONS["build"] then 
	_OPTIONS["build"] = "release"
end

workspace "rpg-server"

	if _OPTIONS["build"] == debug then 
		os.execute("conan install . -s build_type=Debug")
	end

	include("conanbuildinfo.premake.lua")

	if _options["build"] == debug then
		configurations { "Debug " }
	else
		configurations { "Release" }
	end
	
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

		if _options["build"] == debug then
			defines { "DEBUG" }
			runtime "Debug"
			symbols "On"
		else
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"
		end

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

		if _options["build"] == debug then
			defines { "DEBUG" }
			runtime "Debug"
			symbols "On"
		else
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"
		end

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

		if _options["build"] == debug then
			defines { "DEBUG" }
			runtime "Debug"
			symbols "On"
		else
			defines { "NDEBUG" }
			runtime "Release"
			optimize "On"
		end
