-- A solution
workspace "rdc"
	configurations { "Debug", "Release"}

project "serial"
	kind "StaticLib"
	language "C++"
	flags {"C++11"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"
	includedirs {"./libs/serial/include"}

	files {"./libs/serial/include/**.h", "./libs/serial/src/**.cc" }
	links { "rt", "pthread"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "mapper"
	kind "ConsoleApp"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	files {"./Mapper/**.h", "./Mapper/**.cpp" }
	links { "pthread", "rt", "zmq", "enet", "uv", "serial"}
	includedirs { ".", "libs/api", "libs/serial/include" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

