-- A solution
workspace "rdc_servers"
	configurations { "Debug", "Release"}

project "common"
	kind "StaticLib"
	language "C++"
	flags {"C++11"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"
	includedirs {"libs/common/include"}

	files {"libs/common/include/**.h", "libs/common/src/**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

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
	includedirs {"libs/api", "libs/common/include", "libs/enet/include", "libs/zeromq/include" }

	files {"libs/api/**.h", "libs/api/**.cpp" }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "acc"
	kind "ConsoleApp"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	files {"AccServer/**.h", "AccServer/**.cpp", "libs/sqlite3/sqlite3.c" }
	links { "pthread", "rt", "zmq", "common", "dl", "api"}
	includedirs { "libs/api", "libs/common/include", "libs/zeromq/include", "libs/sqlite3" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "stream"
	kind "ConsoleApp"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	files {"StreamServer/**.h", "StreamServer/**.cpp"}
	links { "pthread", "rt", "zmq", "enet", "common", "api"}
	includedirs { "libs/enet/include", "libs/zeromq/include", "libs/common/include", "libs/api" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "starter"
	kind "ConsoleApp"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	files {"ServiceStarter/**.h", "ServiceStarter/**.cpp" }
	links { "pthread", "rt", "zmq", "api", "common", "boost_system", "boost_filesystem"}
	includedirs { "libs/api", "libs/zeromq/include", "libs/common/include"}
	libdirs {"libs/.libs"}

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

	files {"./Mapper/**.h", "./Mapper/**.cpp" }
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

	files {"./Plugins/Example/**.h", "./Plugins/Example/**.cpp" }
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

