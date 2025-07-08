project "DME"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    buildoptions "/utf-8"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "dmepch.h"
    pchsource "src/dmepch.cpp"

    files
    {
        "src/**.h",
        "src/**.cpp",
        "vendor/stb/**.h",
        "vendor/stb/**.cpp",
        "vendor/glm/glm/**.hpp",
        "vendor/glm/glm/**.inl"
    }

    defines
    {
        "_CRT_SECURE_NO_WARNINGS",
        "YAML_CPP_STATIC_DEFINE",
		"GLFW_INCLUDE_NONE"
    }

    includedirs
    {
        "src",
        "vendor/spdlog/include",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.Glad}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.glm}",
        "%{IncludeDir.stb}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.yaml_cpp}"

    }

    links
    {
        "GLFW",
        "Glad",
        "ImGui",
        "yaml-cpp",
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