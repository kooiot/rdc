-- A solution
solution "rdc"
	configurations { "Debug", "Release"}
	project "serial"
		kind "StaticLib"
		platforms {"Native"}
		language "C++"
		flags {"C++11"}
		location "build"
		targetdir "bin/%{cfg.buildcfg}"
		includedirs {"./libs/serial/include"}

		files {"./libs/serial/include/**.h", "./libs/serial/src/**.cc" }
		links { "rt", "pthread"}

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

	project "mapper"
		kind "ConsoleApp"
		platforms {"Native"}
		language "C++"
		flags {"C++11"}
		defines {"RDC_LINUX_SYS"}
		location "build"
		targetdir "bin/%{cfg.buildcfg}"

		files {"./Mapper/**.h", "./Mapper/**.cpp" }
		links { "pthread", "rt", "zmq", "enet", "uv", "serial"}
		includedirs { ".", "libs/api", "libs/serial/include", "libs/enet/include", "libs/zeromq/include", "libs/libuv-v1.9.1/include" }
		libdirs {"libs/.libs"}

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

