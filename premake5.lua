function file_exists(name)
   local f=io.open(name,"r")
   if f~=nil then io.close(f) return true else return false end
end

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
	
	build_type = "Debug"
	if _OPTIONS["build"] == "release" then 
		build_type = "Release"
	end
	os.execute("conan install . -s build_type=" .. build_type)

	if not file_exists("conanbuildinfo.premake.lua") then
		error("conan install failed");
	end

	include("conanbuildinfo.premake.lua")

	configurations { build_type }
	
	conan_basic_setup()
	
	project "core"
		kind "StaticLib"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"
		runtime (build_type)

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

		if build_type == "Debug" then
			defines { "DEBUG" }
			symbols "On"
		else
			defines { "NDEBUG" }
			optimize "On"
		end

	project "server"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"
		runtime (build_type)

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

		if build_type == "Debug" then
			defines { "DEBUG" }
			symbols "On"
		else
			defines { "NDEBUG" }
			optimize "On"
		end

	project "test"
		kind "ConsoleApp"
		language "C++"
		cppdialect "C++17"
		architecture "x86_64"
		runtime (build_type)

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

		if build_type == "Debug" then
			defines { "DEBUG" }
			symbols "On"
		else
			defines { "NDEBUG" }
			optimize "On"
		end
