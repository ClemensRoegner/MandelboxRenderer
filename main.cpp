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

float3 camera_pos(0, 0, -15);
float3 camera_view(0, 0, 1);
float3 camera_up(0, 1, 0);
float3 camera_side(1, 0, 0);
float fov = 0.3f * PI; //horizontal
float tan_hori = glm::tan(fov);
float tan_vert = glm::tan(fov);

int32_t rayTrace(float3& ray_pos, const float3& ray_dir, const float& pixel_radius, float& distance)
{
	distance = 0.0f;
	
	for (uint32_t it = 0; it < max_iterations; it++)
	{
		float d = mandelBoxGetDistance(ray_pos);

		distance += d;
		ray_pos += ray_dir * d;

		if (d < (pixel_radius * distance)) //terminate at sub-pixel width; radius is taken within the pixel and therefore not accurate, but good enough; this also does the AA but also introduces banding
		{
			return 1;
		}

		if (distance > max_distance)
		{
			return -1;
		}
	}

	return -1;
}

void renderThread(const uint32_t& pixel_num, const uint32_t& x, const uint32_t& y)
{
	const float u = float(x) / float(width - 1);
	const float v = float(y) / float(height - 1);
	const float s = u * 2.0f - 1.0f;
	const float t = v * 2.0f - 1.0f;

	float3& pixel = image[y*width+x];

	float3 ray_dir = camera_view + camera_side * tan_hori * s + camera_up * tan_vert * t;
	ray_dir = glm::normalize(ray_dir);

	float pixel_radius = tan_hori / float(width) * 0.25f; //0.5 half side * 0.5 radius

	float distance = 0.0f;
	float3 fractal_pos = camera_pos;

	int32_t res = rayTrace(fractal_pos, ray_dir, pixel_radius, distance);

	if (res > 0)
	{
		float3 surface_normal = mandelboxGetNormal(fractal_pos);
		surface_normal = glm::normalize(surface_normal);
		float3 surface_color = mandelboxGetColor(fractal_pos);
		float surface_ao = mandelboxGetAmbientOcclusion(fractal_pos, surface_normal, distance * pixel_radius); //TODO 2 times the pixel radius seems about right :)
		//float surface_ao = mandelboxGetAmbientOcclusion(fractal_pos, -ray_dir, distance * pixel_radius);
		//float surface_ao = mandelboxGetAmbientOcclusion(fractal_pos, surface_normal, 0.1f);

		float3 ambient_color = surface_color * surface_ao * 0.2f;
		float3 diffuse_color = surface_color * 0.4f;
		float3 specular_color = float3(1,1,1) * 0.4f;

		float3 blinn_phong = brdfBlinnPhong(surface_normal, ambient_color, diffuse_color, specular_color, -ray_dir, light_dir, light_color);

		//pixel = glm::pow(blinn_phong, float3(inverse_gamma, inverse_gamma, inverse_gamma));
		pixel = float3(surface_ao, surface_ao, surface_ao);
		//pixel = glm::abs(surface_normal);
		//pixel = float3(distance, distance, distance);
	}
}

bool startsWith(const char *pre, const char *str) {
	size_t len_pre = strlen(pre);
	size_t len_str = strlen(str);
	return len_str < len_pre ? false : strncmp(pre, str, len_pre) == 0;
}

int32_t main(int32_t argc, char** argv)
{
	if(argc<2) 
	{
		std::cout << "You must at least define the name of the output file!" << std::endl;
		return EXIT_FAILURE;
	}

	int32_t ret = 0;

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
			int32_t res = sscanf(arg+4, "%f", &tmp);
			if (res == 1)
			{
				fov = glm::clamp(PI * tmp / 180.0f, PI * 0.523599f, PI * 0.666666f) * 0.5f; //from 30 to 120 degrees and we want half the fov for our calculations
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

	/*#pragma omp parallel for schedule(dynamic,1)
	for (int32_t pixel_num = 0; pixel_num < pixel_count; pixel_num++) 
	{
		uint32_t pn = uint32_t(pixel_num);
		uint32_t x = pn % width;
		uint32_t y = pn / width;*/
	{
		uint32_t x = 276;
		uint32_t y = 264;
		uint32_t pn = y*width + x;

		renderThread(pn, x, y);
	}

	ret = saveFloatImagePFM(argv[1], (float*)image, width, height);

	safe_delete_a(image);

	if (ret < 0)
	{
		std::cout << "Writing the output file went wrong!" << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}