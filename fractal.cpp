#include "fractal.h"

void sphereFold(float3& p, float& dr, const float& min_radius_sq, const float& fixed_radius_sq)
{
	float r2 = glm::dot(p, p);
	if (r2<min_radius_sq)
	{
		// linear inner scaling
		float temp = (fixed_radius_sq / min_radius_sq);
		p *= temp;
		dr *= temp;
	}
	else if (r2<fixed_radius_sq)
	{
		// this is the actual sphere inversion
		float temp = (fixed_radius_sq / r2);
		p *= temp;
		dr *= temp;
	}
}

void boxFold(float3& p, const float& folding_limit)
{
	p = glm::clamp(p, -folding_limit, folding_limit) * 2.0f - p;
}

float mandelBoxGetDistance(const float3& pos)
{
	float3 p = pos;
	float scale = 2.0f;
	float3 offset = p;
	float dr = 1.0f;

	for (uint32_t i = 0; i < fractal_iterations; i++)
	{
		boxFold(p, 1.0f);
		sphereFold(p, dr, 0.25f, 1.0f);

		p = p*scale + offset;
		dr = dr*abs(scale) + 1.0f;
	}

	return glm::length(p) / abs(dr);
}

float3 CartesianToSpherical(float3 xyz) {

	float r = glm::length(xyz);
	xyz *= 1.f / r;
	float theta = acos(xyz.z);
	float phi = atan2(xyz.y, xyz.x);
	phi += (phi < 0) ? 2 * PI : 0;  // only if you want [0,2pi)

	return float3(phi, theta, r);
}

float3 mandelboxGetColor(const float3& pos)
{
	float3 p = pos;
	float dr = 1.0f;
	for (uint32_t i = 0; i < trap_iterations; i++) {
		boxFold(p, 1.0f);
		sphereFold(p, dr, 0.25f, 1.0f);
	}
	return glm::normalize(glm::abs(p));
}