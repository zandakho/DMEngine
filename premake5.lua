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
IncludeDir["GLFW"] = "%{wks.location}/DME/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/DME/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/DME/vendor/ImGui"
IncludeDir["glm"] = "%{wks.location}/DME/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/DME/vendor/stb"
IncludeDir["entt"] = "%{wks.location}/DME/vendor/entt/include"
    
group "Dependencies"
    include "DME/vendor/GLFW"
    include "DME/vendor/Glad"
    include "DME/vendor/ImGui"

group ""

group "Core"
    include "DME"
group ""

group "Tools"
    include "DMEEditor"
group ""

group "Other"
    include "Sandbox"
group ""
