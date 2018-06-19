# VulkanPractice
Vulkan is a powerful new library proposed to takeover from OpenGL all graphical rendering needs.
This repository was created while I was trying to understand how Vulkan works. The tutorials provided at 
https://vulkan-tutorial.com/Introduction acted as a reference. The eventual plan is to render a surface
/mesh using Vulkan and explore the possibility of handling different computational algorithms by GPU using
the freedom and flexibility provided by Vulkan.

For sake of practice, added OpenGL rendering capability as well. Two rendering pipelines currently work independently. Though they use the same GUI infrastructure. The OpenGL renderer currently supports Bezier Curve Rendering with the help of Tesselation shaders.

Used Libraries: GLFW, Vulkun, OpenGL, GLM
