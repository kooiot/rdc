-- A solution
workspace "rdc"
	configurations { "Debug", "Release"}

project "serial"
	kind "StaticLib"
	language "C++"
	flags {"C++11"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"
	includedirs {"libs/serial/include"}

	files {"libs/serial/include/**.h", "libs/serial/src/**.cc" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "api"
	kind "StaticLib"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"
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

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

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

	files {"Mapper/**.h", "Mapper/**.cpp" }
	links { "dl", "pthread", "rt", "zmq", "enet", "uv", "serial", "api"}
	includedirs { "libs/api", "libs/common/include", "libs/serial/include", "libs/enet/include", "libs/zeromq/include", "libs/libuv-v1.9.1/include" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "example"
	kind "SharedLib"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build/plugins"
	targetdir "bin/%{cfg.buildcfg}/plugins"

	files {"Plugins/Example/**.h", "Plugins/Example/**.cpp" }
	links { "pthread", "rt"}
	includedirs { "libs/api" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "multi-net"
	kind "SharedLib"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build/plugins"
	targetdir "bin/%{cfg.buildcfg}/plugins"

	files {"./Plugins/MultiNet/**.h", "./Plugins/MultiNet/**.cpp" }
	links { "pthread", "rt", "uv"}
	includedirs { "libs/api", "libs/common/include", "libs/libuv-v1.9.1/include" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

