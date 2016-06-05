/////////////////////////////////////////////////
/// Contains declerations for fractal.h
/// by Clemens Roegner 2016
/////////////////////////////////////////////////

#include "fractal.h"

/// <summary>
/// Folds a point along an inner and outer radius.
/// </summary>
/// <param name="p">[IN/OUT] The point.</param>
/// <param name="dr">[IN/OUT] The derivative that will change due to the fold.</param>
/// <param name="min_radius_sq">The square of the minimal radius.</param>
/// <param name="fixed_radius_sq">The fixed radius squared.</param>
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

/// <summary>
/// Folds a point along a boxes border.
/// </summary>
/// <param name="p">[IN/OUT] The point.</param>
/// <param name="folding_limit">The boxes limits.</param>
void boxFold(float3& p, const float& folding_limit)
{
	p = glm::clamp(p, -folding_limit, folding_limit) * 2.0f - p;
}

/// <summary>
/// Returns the distance to the closest point of the mandelbox fractal for a given position.
/// </summary>
/// <param name="pos">The position.</param>
/// <returns>Distance to the closest point within the fractal</returns>
float mandelBoxGetDistance(const float3& pos)
{
	float3 p = pos;
	float3 offset = p;
	float dr = 1.0f;

	const float scale = 2.0f; //original mandelbox parameter. Must be positive

	for (uint32_t i = 0; i < fractal_iterations; i++)
	{
		boxFold(p, 1.0f);
		sphereFold(p, dr, 0.25f, 1.0f);  //original mandelbox parameters

		p = p*scale + offset;
		dr = dr*scale + 1.0f;
	}

	return glm::length(p) / glm::abs(dr);
}

/// <summary>
/// Cartesian to spherical coordinate conversion.
/// </summary>
/// <param name="xyz">Cartesian coordinates.</param>
/// <returns>Spherical Coordinates</returns>
float3 CartesianToSpherical(float3 xyz) {

	float r = glm::length(xyz);
	xyz *= 1.f / r;
	float theta = acos(xyz.z);
	float phi = atan2(xyz.y, xyz.x);
	phi += (phi < 0) ? 2 * PI : 0;  // only if you want [0,2pi)

	return float3(phi, theta, r);
}

/// <summary>
/// Calculates the color of a point in the fractal by using an orbit trap, which is a fractal within itself. This function is a work of trial and error.
/// </summary>
/// <param name="pos">The position on the mandelbox fractal.</param>
/// <returns>A linear color for the surface point.</returns>
float3 mandelboxGetColor(const float3& pos)
{
	float3 p = pos;
	float dr = 1.0f;
	for (uint32_t i = 0; i < trap_iterations; i++) { //mandelbox iterations
		boxFold(p, 1.0f);
		sphereFold(p, dr, 0.25f, 1.0f); //original mandelbox parameters
	}
	return glm::normalize(glm::abs(p));
}