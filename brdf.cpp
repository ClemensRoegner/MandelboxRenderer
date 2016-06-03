#include "brdf.h"

float3 brdfBlinnPhong(const float3& surface_normal, const float3& color_ambient, const float3& color_diffuse, const float3& color_specular, const float3& eye_dir, const float3& light_dir, const float3& light_color)
{
	float3 blinn_phong = color_ambient;
	float lambert_term = glm::dot(surface_normal, light_dir);
	if (lambert_term > 0.0f)
	{
		blinn_phong += color_diffuse * lambert_term;

		float3 halfway = glm::normalize(light_dir + eye_dir);
		float specular_dot = glm::max(glm::dot(surface_normal, halfway), 0.0f);
		float specular_term = glm::pow(specular_dot, 5.0f);
		blinn_phong += color_specular * specular_term * 0.35f;
	}

	blinn_phong *= light_color; //need to account for lights emission

	return blinn_phong;
}