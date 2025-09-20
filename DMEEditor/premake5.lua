project "DMEEditor"
    if HideConsole == true then
       kind "WindowedApp"
       else
        kind "ConsoleApp"
    end
    language "C++"
    cppdialect "C++20"
    staticruntime "off"

    buildoptions "/utf-8"

    targetdir ("%{wks.location}/Build/Binaries/" .. outputdir .. "/%{prj.name}")
    objdir ("%{wks.location}/Build/Intermediate/" .. outputdir .. "/%{prj.name}")

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
        linkoptions { "/ENTRY:mainCRTStartup" }
        
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
