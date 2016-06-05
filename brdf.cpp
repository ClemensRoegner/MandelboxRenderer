#include "brdf.h"

/// <summary>
/// Shades a point with the Blinn/Phong model, given one light source.
/// </summary>
/// <param name="surface_normal">Surface normal of the point.</param>
/// <param name="color_ambient">Ambient color of the point.</param>
/// <param name="color_diffuse">Diffuse color of the point.</param>
/// <param name="color_specular">Specular color of the point.</param>
/// <param name="eye_dir">Vector pointing from the surface point to the eye.</param>
/// <param name="light_dir">Vector pointing from the surface point to the light source.</param>
/// <param name="light_color">Color of the light source.</param>
/// <returns>Shaded linear color.</returns>
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