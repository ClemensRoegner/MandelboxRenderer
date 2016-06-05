/**
 * Contains functionaliy that defines our
 * Mandelbox fractal as described at
 * https://sites.google.com/site/mandelbox/what-is-a-mandelbox
 * by Clemens Roegner 2016
 */

#pragma once

#include <stdint.h>
#include "defines.h"

const uint32_t fractal_iterations = 25; //those values seem good enough for our purposes. You dont wanna go too high, as calculatiosn would increase
const uint32_t trap_iterations = 5;

float mandelBoxGetDistance(const float3& pos);

float3 mandelboxGetColor(const float3& pos);