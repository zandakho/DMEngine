workspace "DME"
    architecture "x86_64"
    startproject "DMEEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["sourcedir"] = "DME/src"
IncludeDir["vendordir"] = "DME/vendor"
IncludeDir["spdlog"] = "DME/vendor/spdlog/include"
IncludeDir["GLFW"] = "DME/vendor/GLFW/include"
IncludeDir["Glad"] = "DME/vendor/Glad/include"
IncludeDir["ImGui"] = "DME/vendor/ImGui"
IncludeDir["glm"] = "DME/vendor/glm"
IncludeDir["stb"] = "DME/vendor/stb"
IncludeDir["entt"] = "DME/vendor/entt/include"

group "Dependencies"
    include "DME/vendor/GLFW"
    include "DME/vendor/Glad"
    include "DME/vendor/ImGui"

group ""

project "DME"
    location "DME"
    kind "StaticLib"
    language "C++"
    cppdialect "C++23"
    staticruntime "on"

    buildoptions "/utf-8"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "dmepch.h"
    pchsource "DME/src/dmepch.cpp"

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp",
        "%{prj.name}/vendor/stb/**.h",
        "%{prj.name}/vendor/stb/**.cpp",
        "%{prj.name}/vendor/glm/glm/**.hpp",
        "%{prj.name}/vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
		"GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "%{IncludeDir.sourcedir}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.entt}"

    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "opengl32.lib"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "DME_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "DME_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "DME_DIST"
        runtime "Release"
        optimize "on"

project "Sandbox"
    location "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "on"

    buildoptions "/utf-8"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.sourcedir}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.vendordir}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "DME"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "DME_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "DME_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "DME_DIST"
        runtime "Release"
        optimize "on"

project "DMEEditor"
    location "DMEEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++23"
    staticruntime "on"

    buildoptions "/utf-8"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "%{prj.name}/src/**.h",
        "%{prj.name}/src/**.cpp"
    }

    includedirs
    {
        "%{IncludeDir.sourcedir}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.vendordir}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}"
    }

    links
    {
        "DME"
    }

    filter "system:windows"
        systemversion "latest"

    filter "configurations:Debug"
        defines "DME_DEBUG"
        runtime "Debug"
        symbols "on"

    filter "configurations:Release"
        defines "DME_RELEASE"
        runtime "Release"
        optimize "on"

    filter "configurations:Dist"
        defines "DME_DIST"
        runtime "Release"
        optimize "on"
