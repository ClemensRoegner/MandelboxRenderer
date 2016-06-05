/**
 * Contains functionaliy to save an image
 * to the hard drive
 * by Clemens Roegner 2016
 */

#pragma once

#include <stdint.h>

bool saveFloatImagePFM(const char* filename, const float* img, const uint32_t& width, const uint32_t& height);

bool saveFloatImageBMP(const char* filename, const float* img, const uint32_t& width, const uint32_t& height);