#include "image.h"

#include <stdio.h>

/// <summary>
/// Saves a buffer of float triplets to a file on the disk.
/// </summary>
/// <param name="filename">The filename/path.</param>
/// <param name="img">Pointer to the buffer containing the float triplets.</param>
/// <param name="w">The width of the image.</param>
/// <param name="h">The height of the image.</param>
/// <returns>True if the file was saved successfully, false otherwise.</returns>
bool saveFloatImagePFM(const char* filename, float* img, uint32_t w, uint32_t h)
{
	int32_t chk = 0;
	FILE * fs = NULL;
	fs = fopen(filename, "wb");
	if (fs == nullptr)
		return false;

	chk = fprintf(fs, "PF\n");
	if (chk <= 0)
		return false;

	chk = fprintf(fs, "%d %d\n", w, h);
	if (chk <= 0)
		return false;

	chk = fprintf(fs, "-1.0\n");
	if (chk <= 0)
		return false;

	size_t elements_to_write = 3 * w * h;
	size_t elements_written = fwrite(img, sizeof(float), elements_to_write, fs); //write the actual image bytes to the file
	if (elements_written != elements_to_write)
		return false;

	chk = fclose(fs);
	if (chk != 0)
		return false;

	return true;
}