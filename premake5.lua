include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

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

group "Vendor"
    include "vendor/premake"
group ""

group "Dependencies"
    include "DME/vendor/Box2D"
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
    include "Sandbox"
group ""

group "Other"
group ""
