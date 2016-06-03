#include "image.h"

#include <stdio.h>

int32_t saveFloatImagePFM(const char* filename, float* img, uint32_t w, uint32_t h)
{
	int32_t chk = 0;
	FILE * fs = NULL;
	fs = fopen(filename, "wb");
	if (fs == nullptr)
		return -1;

	chk = fprintf(fs, "PF\n");
	if (chk <= 0)
		return -1;

	chk = fprintf(fs, "%d %d\n", w, h);
	if (chk <= 0)
		return -1;

	chk = fprintf(fs, "-1.0\n");
	if (chk <= 0)
		return -1;

	fwrite(img, sizeof(float), 3 * w * h, fs); //write the actual image bytes to the file

	chk = fclose(fs);
	if (chk != 0)
		return -1;

	return 0;
}