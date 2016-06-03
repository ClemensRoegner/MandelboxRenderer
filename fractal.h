#pragma once

#include <stdint.h>
#include "defines.h"

const uint32_t fractal_iterations = 25;
const uint32_t trap_iterations = 5;

float mandelBoxGetDistance(const float3& pos);

float3 mandelboxGetColor(const float3& pos);