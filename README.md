# RayTracer

### Authors: Liam Walker

## Overview 

A simple software ray tracer. Has an OpenGL rasterizer that allows for the scene geometry and general appearance to be viewed at a reasonable framerate. Uses a non biased pathtracing algorithm that uses the Russian-Roulette algorithm (probabilistic ray termination). Implments ray traced illumination, soft shadows, and pixel antialiasing. 

## Structure

All of the code is located in the src folder. Most of the "important" code is located in scene.cpp, as this is where the ray tracing is performed, and the aforementioned features are implemented. The other files contain important classes such as the material system, the shader system (for OpenGL rendering), and the bounding volume heirarchies. 

## Output

Here are some pretty images produced by the ray tracing algorithm. 

![raytrace](https://user-images.githubusercontent.com/37026953/119881721-9e7d1b80-befb-11eb-9ad2-1dfed8b30285.PNG)

![raytrace2](https://user-images.githubusercontent.com/37026953/119883719-d8e7b800-befd-11eb-933a-b32b520801f8.PNG)

![raytrace3](https://user-images.githubusercontent.com/37026953/119884046-38de5e80-befe-11eb-9380-82ff5dfee32d.PNG)



