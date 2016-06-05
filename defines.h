/**
 * Includes definitions that make coding easier
 * by Clemens Roegner 2016
 */

#pragma once

#include <stdint.h>
#include "glm/glm.hpp"

typedef glm::vec2 float2;
typedef glm::vec3 float3;
typedef glm::vec4 float4;

#define safe_delete(a) if(a!=nullptr) {delete a;}
#define safe_delete_a(a) if(a!=nullptr) {delete[] a;}

const float PI = 3.14159265359f;
const float EPS = 0.0001f;

const float inverse_gamma = 1.0f / 2.2f;