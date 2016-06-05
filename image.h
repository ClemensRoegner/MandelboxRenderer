/////////////////////////////////////////////////
/// Contains functionaliy to save an image
/// to the hard drive
/// by Clemens Roegner 2016
/////////////////////////////////////////////////

#pragma once

#include <stdint.h>

bool saveFloatImagePFM(const char* filename, float* img, uint32_t w, uint32_t h);