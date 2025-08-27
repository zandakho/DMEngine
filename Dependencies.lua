VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/DME/vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/DME/vendor/Glad/include"
IncludeDir["ImGui"] = "%{wks.location}/DME/vendor/ImGui"
IncludeDir["glm"] = "%{wks.location}/DME/vendor/glm"
IncludeDir["stb"] = "%{wks.location}/DME/vendor/stb"
IncludeDir["entt"] = "%{wks.location}/DME/vendor/entt/include"
IncludeDir["yaml_cpp"] = "%{wks.location}/DME/vendor/yaml-cpp/include"
IncludeDir["ImGuizmo"] = "%{wks.location}/DME/vendor/ImGuizmo"
IncludeDir["shaderc"] = "%{wks.location}/DME/vendor/shaderc/include"
IncludeDir["SPIRV_Cross"] = "%{wks.location}/DME/vendor/SPIRV-Cross"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
LibraryDir["VulkanSDK_Debug"] = "%{wks.location}/DME/vendor/VulkanSDK/Lib"
LibraryDir["VulkanSDK_DebugDLL"] = "%{wks.location}/DME/vendor/VulkanSDK/Bin"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK_Debug}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"