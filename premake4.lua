-- A solution
solution "rdc"
	configurations { "Debug", "Release"}
	buildoptions {"-std=c++11"}
	project "serial"
		kind "StaticLib"
		platforms {"Native"}
		language "C++"
		location "build"
		targetdir "bin"
		includedirs {"libs/serial/include"}
		files {"libs/serial/include/**.h", "libs/serial/src/**.cc" }
		links { "rt", "pthread"}

		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

	project "api"
		kind "StaticLib"
		platforms {"Native"}
		language "C++"
		defines {"RDC_LINUX_SYS"}
		location "build"
		targetdir "bin"
		includedirs { "libs/api", "libs/common/include", "libs/libuv-v1.9.1/include" }
		files {
			"libs/api/**.h",
			"libs/api/DataJson.cpp",
			"libs/api/PluginApi.cpp",
			"libs/api/PluginLoader.cpp",
			"libs/api/PortMgr.cpp",
			"libs/api/TcpServer.cpp",
			"libs/api/TcpClient.cpp",
			"libs/api/Udp.cpp",
		}
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
		defines {"RDC_LINUX_SYS"}
		location "build"
		targetdir "bin"
		files {"./Mapper/**.h", "./Mapper/**.cpp" }
		links { "dl", "pthread", "rt", "zmq", "enet", "uv", "serial", "api"}
		includedirs { "libs/api", "libs/common/include", "libs/serial/include", "libs/enet/include", "libs/zeromq/include", "libs/libuv-v1.9.1/include" }
		libdirs {"libs/.libs"}
		
		configuration "Debug"
			defines { "DEBUG" }
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			flags { "Optimize" }

	project "example"
		kind "SharedLib"
		platforms {"Native"}
		language "C++"
		defines {"RDC_LINUX_SYS"}
		location "build"
		includedirs {"libs/api"}
		files {"Plugins/Example/**.h", "Plugins/Example/**.cpp" }
		links { "rt", "pthread"}

		configuration "Debug"
			defines { "DEBUG" }
			targetdir "bin/plugins"
			flags { "Symbols" }

		configuration "Release"
			defines { "NDEBUG" }
			targetdir "bin/plugins"
			flags { "Optimize" }


