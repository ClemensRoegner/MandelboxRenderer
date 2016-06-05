/**
 * Contains declerations for image.h
 * by Clemens Roegner 2016
 */

#include "image.h"
#include <stdio.h>
#include <cstdlib>
#include <vector>
#include "cimg\CImg.h"
#include "defines.h"

/// <summary>
/// Saves a buffer of float triplets to a file on the disk using the .PFM format. This function follows the definition of the PFM format, described by Paul Debevec at http://www.pauldebevec.com/Research/HDR/PFM/
/// </summary>
/// <param name="filename">The filename/path.</param>
/// <param name="img">Pointer to the buffer containing the float triplets.</param>
/// <param name="width">The width of the image.</param>
/// <param name="height">The height of the image.</param>
/// <returns>True if the file was saved successfully, false otherwise.</returns>
bool saveFloatImagePFM(const char* filename, const float* img, const uint32_t& width, const uint32_t& height)
{
	int32_t chk = 0;
	FILE * fs = NULL;
	fs = fopen(filename, "wb");
	if (fs == nullptr)
		return false;

	chk = fprintf(fs, "PF\n");
	if (chk <= 0)
		return false;

	chk = fprintf(fs, "%d %d\n", width, height);
	if (chk <= 0)
		return false;

	chk = fprintf(fs, "-1.0\n");
	if (chk <= 0)
		return false;

	size_t elements_to_write = 3 * width * height;
	size_t elements_written = fwrite(img, sizeof(float), elements_to_write, fs); //write the actual image bytes to the file
	if (elements_written != elements_to_write)
		return false;

	chk = fclose(fs);
	if (chk != 0)
		return false;

	return true;
}

/// <summary>
/// Saves a buffer of float triplets to a file on the disk using the BMP format via CImg library: http://cimg.eu/.
/// </summary>
/// <param name="filename">The filename/path.</param>
/// <param name="img">Pointer to the buffer containing the float triplets.</param>
/// <param name="width">The width of the image.</param>
/// <param name="height">The height of the image.</param>
/// <returns>True if the file was saved successfully, false otherwise.</returns>
bool saveFloatImageBMP(const char* filename, const float* img, const uint32_t& width, const uint32_t& height)
{
	//we actually have to de-interleave the image, requireing further allocations. The CImg functionality was added later to provide a more common output format.
	const size_t buffer_size = sizeof(float)*3*width*height;
	float* planar = (float*)std::malloc(buffer_size);
	if (planar == nullptr)
	{
		return false;
	}

	//do the planar conversion sequential to be more write chache friendly
	const uint32_t pixel_count = width * height;
	float* planarR = planar + pixel_count * 0;
	float* planarG = planar + pixel_count * 1;
	float* planarB = planar + pixel_count * 2;
	for (uint32_t i = 0; i < pixel_count; i++)
		planarR[i] = img[i * 3 + 0];
	for (uint32_t i = 0; i < pixel_count; i++)
		planarG[i] = img[i * 3 + 1];
	for (uint32_t i = 0; i < pixel_count; i++)
		planarB[i] = img[i * 3 + 2];

	//setup Cimg
	cimg_library::CImg<float> image(planar, width, height, 1, 3, true);
	image.normalize(0.0f, 255.0f);
	image.save_bmp(filename);

	safe_delete(planar);

	return true;
}