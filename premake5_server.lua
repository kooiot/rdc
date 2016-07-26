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
		flags { "Symbols" }

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
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "acc_srv"
	kind "ConsoleApp"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	files {"AccServer/**.h", "AccServer/**.cpp", "libs/sqlite3/sqlite3.c" }
	links { "pthread", "rt", "zmq", "common", "dl"}
	includedirs { "libs/api", "libs/common/include", "libs/zeromq/include", "libs/sqlite3" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

project "stream_srv"
	kind "ConsoleApp"
	language "C++"
	flags {"C++11"}
	defines {"RDC_LINUX_SYS"}
	location "build"
	targetdir "bin/%{cfg.buildcfg}"

	files {"StreamServer/**.h", "StreamServer/**.cpp"}
	links { "pthread", "rt", "zmq", "enet", "common"}
	includedirs { "libs/enet/include", "libs/zeromq/include", "libs/common/include", "libs/api" }
	libdirs {"libs/.libs"}

	filter "configurations:Debug"
		defines { "DEBUG" }
		flags { "Symbols" }

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
		flags { "Symbols" }

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

