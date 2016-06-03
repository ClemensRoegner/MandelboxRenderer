#pragma once

#include <stdint.h>
#include "defines.h"

float3 brdfBlinnPhong(const float3& surface_normal, const float3& color_ambient, const float3& color_diffuse, const float3& color_specular, const float3& eye_dir, const float3& light_dir, const float3& light_color);