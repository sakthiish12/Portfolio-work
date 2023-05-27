cd /d %~dp0
%Vulkan_SDK%/Bin/glslc.exe diffuse.frag -o diffuse_frag.spv
%Vulkan_SDK%/Bin/glslc.exe diffuse.vert -o diffuse_vert.spv

%Vulkan_SDK%/Bin/glslc.exe phong.frag -o phong_frag.spv
%Vulkan_SDK%/Bin/glslc.exe phong.vert -o phong_vert.spv


%Vulkan_SDK%/Bin/glslc.exe point_light.vert -o point_light_vert.spv
%Vulkan_SDK%/Bin/glslc.exe point_light.frag -o point_light_frag.spv


%Vulkan_SDK%/Bin/glslc.exe simple.vert -o simple_vert.spv
%Vulkan_SDK%/Bin/glslc.exe simple.frag -o simple_frag.spv

%Vulkan_SDK%/Bin/glslc.exe simple_shader.vert -o simple_shader_vert.spv
%Vulkan_SDK%/Bin/glslc.exe simple_shader.frag -o simple_shader_frag.spv

%Vulkan_SDK%/Bin/glslc.exe toon.vert -o toon_vert.spv
%Vulkan_SDK%/Bin/glslc.exe toon.frag -o toon_frag.spv

%Vulkan_SDK%/Bin/glslc.exe grid_vert.vert -o grid_vert.spv
%Vulkan_SDK%/Bin/glslc.exe grid_frag.frag -o grid_frag.spv

%Vulkan_SDK%/Bin/glslc.exe ui_shader.vert -o ui_shader_vert.spv
%Vulkan_SDK%/Bin/glslc.exe ui_shader.frag -o ui_shader_frag.spv

%Vulkan_SDK%/Bin/glslc.exe text_shader_vert.vert -o text_shader_vert.spv
%Vulkan_SDK%/Bin/glslc.exe text_shader_frag.frag -o text_shader_frag.spv

%Vulkan_SDK%/Bin/glslc.exe Particle_vert.vert -o Particle_vert.spv
%Vulkan_SDK%/Bin/glslc.exe Particle_frag.frag -o Particle_frag.spv

pause