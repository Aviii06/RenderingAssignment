# Rendering Assignment
The goal of the Assignment was to render a scene where you can see 36 spheres, and they have different materials properties in both direction. 
When you click on the sphere it should change the color of the particular sphere to the normal of the point it was clicked on.

Here is a [demo-video](https://drive.google.com/file/d/1cArXfnPL2pZB5O6UapjdAnNFZesSzyv6/view?usp=sharing)

The two properties were, directional light specular power and diffuse intensity.


# Build Instructions

## Dependencies
Only dependency needed is `openGL` for mac and windows. 
For linux run the below command.
```
sudo apt-get update && sudo apt-get install xorg-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev libgl1-mesa-dev libglew-dev mesa-utils freeglut3-dev
```

After this you just need to build the cpp project using cmake.

## Building
```
git clone https://github.com/Aviii06/RenderingAssignment/
cd RenderingAssignment
git submodule update --init --recursive
mkdir build && cd build
cmake ..
cmake --build . -j8
cp ../imgui.ini imgui.ini
./RenderingAssignment
```

# Approach
I made two key observations:
1) We ideally should be having only one draw call for all the spheres. And in the vertex shader we can offset all of these, with using uniforms. The only problem is how do you get which offset is the one you're currently clicking on. Thankfully openGL provides a specific way to do it. I used instanced rendering to render all the spheres in one draw call. Then I had a uniform of offset of the size 36. I used the gl_InstanceID to get the offset of the sphere I was currently rendering. This is still not ideal, as for 36 spheres it is enough but it does not scale well as you can hit the limit of the number of uniforms you can pass to the shader. For that we can use instanced arrays. But for this specific example this was enough. This gl_InstancedID is also then used to create the cariation for the phong shading model.

2) For clicking we need to know whcih object i've selected and at which point in sphere. This is an incredible task, and I tried to implement it by creating a frameBuffer which would just render and objectID instead of the color. Then we can click on the screen to get the color and hence the ObjectID. The problem with this approach is that it requires a lot of refactoring in the already existing framework. Another approach is obviously raycasting. I intially thought of implementing a proper raycasting algorithm, but then I realized that for orthographic projection, we can quite easily implement a very specific algorithm. After that you can easily find the point of intersection, as you know the x,y coordinate and the radius of the sphere. 

During the implementation I made 2 main PRs on the rendering framework. Those can be found here:
1) https://github.com/Aviii06/Vivid/pull/27
2) https://github.com/Aviii06/Vivid/pull/28

Other than it all the code for using the framework can be found on `src/main.cpp` and `assets/shaders/instancing.pixelShader.glsl` and `assets/shaders/instancing.vertexShader.glsl`

I also implemented a simple phong shading model for the spheres. All the normal and mvp matrix calculations are done in the vertex shader.
