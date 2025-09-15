<h1><i>TwoD</i></h1>

The engine is copied on the Hazel Engine: https://github.com/TheCherno/Hazel


> Setup

First, install the latest version of VulkanSDK on your computer (the Python script does not work and will be removed later)

VulkanSDK:

[![Lunarg](https://img.shields.io/badge/-Vulkan-090909?style=for-the-badge&logo=Vulkan&logoColor=47C5FB)](https://vulkan.lunarg.com/)
[![Lunarg](https://img.shields.io/badge/-For_Windows-090909?style=for-the-badge&logo=Vulkan&logoColor=47C5FB)](https://sdk.lunarg.com/sdk/download/1.4.321.1/windows/vulkansdk-windows-X64-1.4.321.1.exe)
[![Lunarg](https://img.shields.io/badge/-For_Linux-090909?style=for-the-badge&logo=Vulkan&logoColor=47C5FB)](https://sdk.lunarg.com/sdk/download/1.4.321.1/linux/vulkansdk-linux-x86_64-1.4.321.1.tar.xz)
[![Lunarg](https://img.shields.io/badge/-For_Mac-090909?style=for-the-badge&logo=Vulkan&logoColor=47C5FB)](https://sdk.lunarg.com/sdk/download/1.4.321.0/mac/vulkansdk-macos-1.4.321.0.zip)

Clone this repository:

```
git clone https://github.com/zandakho/DMEngine
```

Update submodules: 

```
git submodule update --init --recursive
```

If you have completed the step of installing VulkanSDK, run Setup.bat in the scripts/ folder.
After everything, run the file WinBuild.bat
