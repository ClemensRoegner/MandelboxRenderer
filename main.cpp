/////////////////////////////////////////////////
/// Contains the main functionality of this
/// simple Mandelbox renderer
/// by Clemens Roegner 2016
/////////////////////////////////////////////////

#include <cstring>
#include <iostream>
#include <fstream>
#include <stdint.h>

#include "glm/glm.hpp"
#include "defines.h"
#include "fractal.h"
#include "brdf.h"
#include "image.h"

//-----------------------------------------|
// constants for ray tracing and the scene |
//-----------------------------------------|
const uint32_t max_iterations = 400;
const float max_distance = 25.0f;
const float ao_steps = 5.0f;
const uint32_t normal_iterations = 5;

const float3 light_dir = glm::normalize(float3(0.64, 0.57, 0.52)); //I choose those to be constant. For simplicity sake
const float3 light_color = float3(1, 1, 1);

//-----------------------------------------|
// configuration variables for rendering.  |
// those are the result of command line    |
// paramters. However, I choose to put     |
// some neat default values in ;)          |
//-----------------------------------------|

float3* image = nullptr;

uint32_t width = 200;
uint32_t height = 200;
uint32_t pixel_count = width * height;

float3 camera_pos(0, 0, -10);
float3 camera_view(0, 0, 1);
float3 camera_up(0, 1, 0);
float3 camera_side(1, 0, 0);
float fov = 0.3f * PI; //horizontal
float tan_hori = glm::tan(fov);
float tan_vert = glm::tan(fov);

float ao_radius = 0.05f;

//-----------------------------------------|
// functions for ray tracing and image     |
// generation in general. Here you find    |
// everything that is not really tied to   |
// the fractal itself. Meaning the mandel- |
// box specific funtions can be replaced   |
// to render another fractal.              |
//-----------------------------------------|

/// <summary>
/// Approximates the normal vector for the mandelbox fractal.
/// </summary>
/// <param name="pos">The position on the fractal for which the normal should be approximated.</param>
/// <returns>A normalized vector that represents the surface orientation.</returns>
float3 approxNormal(const float3& pos)
{
	float h = 2.0f * EPS;
	float3 normal = float3(0, 0, 0);
	float normal_length = 0.0f;

	for (uint32_t i = 0; i<normal_iterations && normal_length < EPS; i++)
	{
		normal.x = mandelBoxGetDistance(float3(pos.x + h, pos.y, pos.z)) - mandelBoxGetDistance(float3(pos.x - h, pos.y, pos.z));
		normal.y = mandelBoxGetDistance(float3(pos.x, pos.y + h, pos.z)) - mandelBoxGetDistance(float3(pos.x, pos.y - h, pos.z));
		normal.z = mandelBoxGetDistance(float3(pos.x, pos.y, pos.z + h)) - mandelBoxGetDistance(float3(pos.x, pos.y, pos.z - h));

		normal_length = glm::length(normal);
		h += EPS;
	}
	assert(normal_length>0.0f);

	return normal / normal_length;
}


/// <summary>
/// Approxes the ambient occlusion for the mandelbox fractal. Works with ao_radius to determin the area on which to check for occluders.
/// </summary>
/// <param name="pos">The position on the fractal for which the AO should be approximated.</param>
/// <param name="normal">The surface normal for pos.</param>
/// <returns>A value from 0 up to 1 representaing the AO</returns>
float approxAmbientOcclusion(const float3& pos, const float3& normal, const float& ao_distance)
{
	const float ao_offset = ao_distance / ao_steps;
	float walked_dist = ao_offset; //we need to offset from the start since we are approximating the fractal via a distance threshold
	for (float i = 0.0f; i < ao_steps; i += 1.0f) //simple ray marching
	{
		float3 test_pos = pos + normal * walked_dist; //march along the normal and test for the closest point of the fractal
		walked_dist += mandelBoxGetDistance(test_pos); 
	}
	return glm::min(1.0f,walked_dist / (ao_offset * (ao_steps + 1.0f))); //divide by the amount we could have idially traveled
}

/// <summary>
/// Ray traces the mandelbox. Termination via max_iterations.
/// </summary>
/// <param name="ray_pos">Startin position.</param>
/// <param name="ray_dir">Ray direction.</param>
/// <param name="pixel_radius">The inital pixels radius. Used to terminate the marching</param>
/// <param name="distance">[OUT] The distance of the ray marching until termination.</param>
/// <returns>true if the fractal was hit, false otherwise</returns>
bool rayTrace(float3& ray_pos, const float3& ray_dir, const float& pixel_radius, float& distance)
{
	distance = 0.0f;
	
	for (uint32_t it = 0; it < max_iterations; it++) //do the ray tracing
	{
		float d = mandelBoxGetDistance(ray_pos);

		distance += d;
		ray_pos += ray_dir * d;

		if (d < (pixel_radius * distance)) //terminate at sub-pixel width; radius is taken within the pixel and therefore not accurate, but good enough; this also does the AA but also introduces banding
		{
			return true;
		}

		if (distance > max_distance) //terminate at max distance
		{
			return false;
		}
	}

	return false;
}

/// <summary>
/// On render thread represents on pixel. Writes a color into the pixels location within the buffer, if the ray tracing hits the fractal.
/// </summary>
/// <param name="pixel_num">The number of the pixel to render.</param>
/// <param name="x">The x of the pixel to render.</param>
/// <param name="y">The y of the pixel to render.</param>
void renderThread(const uint32_t& pixel_num, const uint32_t& x, const uint32_t& y)
{
	//some const inits used to setup the tracing
	const float u = float(x) / float(width - 1);
	const float v = float(y) / float(height - 1);
	const float s = u * 2.0f - 1.0f;
	const float t = v * 2.0f - 1.0f;

	float3& pixel = image[pixel_num];

	//calculated the ray direction
	float3 ray_dir = camera_view + camera_side * tan_hori * s + camera_up * tan_vert * t;
	ray_dir = glm::normalize(ray_dir);

	float pixel_radius = tan_hori / (float(width) * 0.5f) * 0.5f; //0.5 half side; 0.5 radius

	//init and do the ray tracing
	float distance = 0.0f;
	float3 fractal_pos = camera_pos;

	bool res = rayTrace(fractal_pos, ray_dir, pixel_radius, distance);

	if (res) //we actually hit the fractal
	{
		//gather attributes of the hit
		float3 surface_color = mandelboxGetColor(fractal_pos);
		float3 surface_normal = approxNormal(fractal_pos);
		float surface_ao = approxAmbientOcclusion(fractal_pos, surface_normal, ao_radius); //for simplicity we use a global radius

		float3 ambient_color = surface_color * surface_ao * 0.2f;
		float3 diffuse_color = surface_color * 0.4f;
		float3 specular_color = float3(1,1,1) * 0.4f;

		//do the lighting and write to our image buffer
		float3 blinn_phong = brdfBlinnPhong(surface_normal, ambient_color, diffuse_color, specular_color, -ray_dir, light_dir, light_color);

		pixel = glm::pow(blinn_phong, float3(inverse_gamma, inverse_gamma, inverse_gamma));
	}
}

//-----------------------------------------|
// Main                                    |
//-----------------------------------------|

/// <summary>
/// Checks if str starts with the char sequence pre
/// </summary>
/// <param name="pre">The char sequence to check for.</param>
/// <param name="str">The string which may or may not contain pre.</param>
/// <returns>True if str starts with the sequence pre. False otherwise</returns>
bool startsWith(const char *pre, const char *str) {
	size_t len_pre = strlen(pre);
	size_t len_str = strlen(str);
	return len_str < len_pre ? false : strncmp(pre, str, len_pre) == 0;
}

/// <summary>
/// Entry point
/// </summary>
/// <param name="argc">Number of commandline parameters.</param>
/// <param name="argv">Command line parameters.</param>
/// <returns>EXIT_FAILURE in case of an error. EXIT_SUCCESS otherwise.</returns>
int32_t main(int32_t argc, char** argv)
{
	if(argc<2) 
	{
		std::cout << "You must at least define the name of the output file!" << std::endl;
		return EXIT_FAILURE;
	}

	//read command line
	for (int32_t argn = 2; argn < argc; argn++)
	{
		char* arg = argv[argn];

		if (startsWith("width:", arg))
		{
			uint32_t tmp = 0;
			int32_t res = sscanf(arg+6,"%u",&tmp);
			if (res == 1)
			{
				width = tmp;
			}
		}
		else if (startsWith("height:", arg))
		{
			uint32_t tmp = 0;
			int32_t res = sscanf(arg+7, "%u", &tmp);
			if (res == 1)
			{
				height = tmp;
			}
		}
		else if (startsWith("cam:", arg))
		{
			if (strcmp("front", arg + 4) == 0)
			{
				camera_pos = float3(10, 0, 2);
				camera_view = float3(-1, 0, 0);
				camera_up = float3(0, 1, 0);
				camera_side = float3(0, 0, -1);
			}
			else if (strcmp("edge", arg + 4) == 0)
			{
				camera_pos = float3(5.15f, 6.15f, -7.65f);
				camera_view = glm::normalize(float3(-1.0f, -1.0f, 1.0f));
				camera_side = glm::normalize(glm::cross(camera_view, float3(0, -1, 0)));
				camera_up = glm::normalize(glm::cross(camera_view, camera_side));
			}
			else if (strcmp("back", arg + 4) == 0)
			{
				camera_pos = float3(-3.75, 0, +7.25);
				camera_view = float3(0, 0, -1);
				camera_up = glm::normalize(float3(0.25f, 1.0f, 0.0f));
				camera_side = glm::normalize(glm::cross(camera_up, camera_view));
			}
		}
		else if (startsWith("fov:", arg))
		{
			float tmp = 0;
			int32_t res = sscanf(arg + 4, "%f", &tmp);
			if (res == 1)
			{
				fov = glm::clamp(PI * tmp / 180.0f, PI * 0.523599f, PI * 0.666666f) * 0.5f; //from 30 to 120 degrees and we want half the fov for our calculations
			}
		}
		else if (startsWith("ao:", arg))
		{
			float tmp = 0;
			int32_t res = sscanf(arg + 3, "%f", &tmp);
			if (res == 1)
			{
				ao_radius = glm::clamp(tmp,EPS,4.0f);
			}
		}
	}

	//process command line paramters
	pixel_count = width * height;
	float screen_ratio = float(height) / float(width);
	tan_hori = glm::tan(fov);
	tan_vert = tan_hori * screen_ratio;
	
	image = new float3[width*height]; //TODO do allocation check
	std::memset(image, 0, sizeof(float3)*width*height);

	#pragma omp parallel for schedule(dynamic,1)
	for (int32_t pixel_num = 0; pixel_num < pixel_count; pixel_num++) 
	{
		uint32_t pn = uint32_t(pixel_num);
		uint32_t x = pn % width;
		uint32_t y = pn / width;

		renderThread(pn, x, y);
	}

	//write the image to the file and delete the buffer
	bool chk = saveFloatImagePFM(argv[1], (float*)image, width, height);

	safe_delete_a(image);

	if (chk == false)
	{
		std::cout << "Writing the output file went wrong!" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}