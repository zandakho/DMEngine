include "./vendor/premake/PremakeCustomization/SolutionItems.lua"
include "Dependencies.lua"
include "Config.lua"

workspace "DME"
    architecture "x86_64"
    startproject "DMEEditor"

    configurations
    {
        "Debug",
        "Release",
        "Dist"
    }

    SolutionItems
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
group ""