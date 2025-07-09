project "DMEEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    buildoptions "/utf-8"

    targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "src/**.h",
        "src/**.cpp"
    }

    includedirs
    {
        "%{wks.location}/DME/vendor/spdlog/include",
        "%{wks.location}/DME/src",
        "%{wks.location}/DME/vendor",
        "%{IncludeDir.glm}",
        "%{IncludeDir.entt}",
        "%{IncludeDir.ImGuizmo}"
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
