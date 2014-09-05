#include "image_file.hpp"

#include <fstream>
#include <string>
#include <vector>

using namespace std;

#include <cstdio>
#include <png.h>

bool write_png
(
	size_t	 				width,
	size_t					height,
	const std::string&		filename,
	const unsigned char*	data
)
{
	FILE* fp = fopen(filename.c_str(), "wb");
	if(!fp)
	{
		return false;
	}

	png_structp	png_ptr;
	png_infop	info_ptr;

	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if(!png_ptr)
		return false;

	info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr)
	{
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
	}

	png_init_io(png_ptr, fp);
	png_set_IHDR(png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_BASE);


	png_color_8 sig_bit;
	sig_bit.red = 8;
	sig_bit.green = 8;
	sig_bit.blue = 8;
	sig_bit.alpha = 0;
	png_set_sBIT(png_ptr, info_ptr, &sig_bit);

	png_write_info(png_ptr, info_ptr);

	std::vector<const unsigned char*> rows(height);

	for(size_t i=0; i<height; ++i)
	{
		rows[height-i-1] = data+i*width*3;
	}

	png_write_image(png_ptr, const_cast<png_byte**>(rows.data()));
	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);

	fclose(fp);

	return true;
}

bool write_ppm
(
	size_t	 				width,
	size_t					height,
	const std::string&		filename,
	const unsigned char*	data
)
{
	ofstream ofs
	(
		(filename+".ppm").c_str(),
		ios_base::out | ios_base::binary | ios_base::out
	);

	if(!ofs.good())
	{
		return false;
	}

	ofs <<
		"P6\n" <<
		width << ' ' << height << '\n' <<
		"255\n";

	for(size_t i=height; i!=0; --i)
	{
		ofs.write(reinterpret_cast<const char*>(data+(i-1)*width*3), width*3);
	}

	return true;
}

