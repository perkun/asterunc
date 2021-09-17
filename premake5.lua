workspace "asterunc"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}"

project "asterunc"
	location "./"
	--possible values: ConsoleApp, SharedLib, StaticLib, Makefile, Utility, Packaging
	kind "ConsoleApp"

	language "C++"
	cppdialect "C++17"
	--buildoptions { "-pthread" }

	targetdir ("bin/" .. outputdir)
	objdir ("build/" .. outputdir)


	files
	{
		"src/**.h",
		"src/**.cpp",
		"lib/cppargs/*.h",
		"lib/cppargs/**.cpp"
	}

	removefiles { "lib/cppargs/src/main.cpp" }

	includedirs
	{
		"lib/cppargs"
	}

	links {}

	filter "configurations:Debug"
		symbols "On"
		defines "DEBUG"

	filter "configurations:Release"
		-- defines...
		optimize "Off"
		defines "RELEASE"

	filter "configurations:Dist"
		-- defines...
		optimize "On"
