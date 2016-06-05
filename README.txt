MandelboxRender
by Clemens Roegner

-------------------------------
--    Project Description    --
-------------------------------

MandelboxRenderer is a small and very simplistic renderer written in C++ that lets you generate images of the Mandelbox fractal. This project is mainly to display some clean and portable code.

--> External Code/Functionality used:
 * GLM for having some GLSL style functionality in C++
 * OpenMP to parallize the rendering process

--> External Resources used:
 * PFM file format: http://www.pauldebevec.com/Research/HDR/PFM/
 * Mandelbox description: https://sites.google.com/site/mandelbox/what-is-a-mandelbox

-------------------------------
--           Usage           --
-------------------------------

--> Compilation
 * Use cmake to buil the compilation environemtn you want it to have. See https://cmake.org/ on how to achive that. The CMakeLists.txt should be enough for a simple setup.
 * This was tested with CMake 3.5.2 with windows 10 and Visual Studio 2015
 * A Win32 Binary is provided within the bin directory

--> Run
The executable takes the following parameters (seperated via a blank):
* [REQUIRED] <filename> - the first parameter passed to the application must be a valid file location.
* [OPTIONAL] width:<pixels> - Width of the desired image in pixels
* [OPTIONAL] height:<pixels> - Height of the desired image in pixels
* [OPTIONAL] fov:<degrees> - Field of view of the camera. Allowed range: from 30 to 120 degrees (clamped automatically)
* [OPTIONAL] ao:<worldunits> - Radius of the ambient occlusion check in world units. Allowed range: 0.0001 to 4.0 (clamped automatically)
* [OPTIONAL] cam:<position> - The camera position. You can choose between the positions: front, edge and back or do not use it for the default camera position.

--> View Results
 * In the tools/ directory you find the HDRView.exe thats lets you display the image file under Windows
 * XnView runs on multiple platforms and can display the .pfm format as well: http://www.xnview.com/en/xnview/#downloads (Minimal installation is sufficient)