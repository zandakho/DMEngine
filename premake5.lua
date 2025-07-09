include "./vendor/premake/premake_customization/solution_items.lua"

workspace "DME"
    architecture "x86_64"
    startproject "DMEEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    solution_items
    {
        ".editorconfig"
    }

    flags
    {
        "MultiProcessorCompile"
    }

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/DME/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/DME/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/DME/vendor/ImGui"
IncludeDir["glm"] = "%{wks.location}/DME/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/DME/vendor/stb"
IncludeDir["entt"] = "%{wks.location}/DME/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/DME/vendor/yaml-cpp/include"

group "Vendor"
    include "vendor/premake"
group ""

group "Dependencies"
    include "DME/vendor/GLFW"
    include "DME/vendor/Glad"
    include "DME/vendor/ImGui"
    include "DME/vendor/yaml-cpp"

group ""

group "Core"
    include "DME"
group ""

group "Tools"
    include "DMEEditor"
group ""

group "Other"
group ""
