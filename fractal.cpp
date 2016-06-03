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

float3 mandelboxGetNormal(const float3& pos)
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

float mandelboxGetAmbientOcclusion(const float3& pos, const float3& normal, const float& occlusion_radius)
{
	float3 tangent = float3(normal.z, normal.x, normal.y);
	float3 bitangent = float3(normal.y, normal.z, normal.x);

	float checks = 0.0f;
	float checkfactor = 0.0f;
	float steps = 2.0f;
	float check_radius = occlusion_radius / steps * 0.5f;
	for (float z = 0.5f; z <= steps; z += 1.0f)
	{
		for (float y = -steps+0.5f; y <= steps; y += 1.0f)
		{
			for (float x = -steps+0.5f; x <= steps; x += 1.0f)
			{
				float3 test_pos = pos + tangent * check_radius * x + bitangent * check_radius * y + normal * check_radius * z;
				float distance = mandelBoxGetDistance(test_pos);
				checkfactor += glm::clamp(distance / check_radius, 0.0f, 1.0f);
				checks += 1.0f;
			}
		}
	}

	return checkfactor / checks;

	/*float3 test_pos = pos + normal * (occlusion_radius * (2.0f + EPS));
	float distance = mandelBoxGetDistance(test_pos);
	//return glm::clamp(distance/occlusion_radius,0.0f,1.0f);
	return distance / occlusion_radius;*/
}