#pragma once

#include <string>

bool write_png
(
	size_t	 				width,
	size_t					height,
	const std::string&		filename,
	const unsigned char*	data
);

bool write_ppm
(
	size_t	 				width,
	size_t					height,
	const std::string&		filename,
	const unsigned char*	data
);

inline bool write_image
(
	size_t	 				width,
	size_t					height,
	const std::string&		filename,
	const unsigned char*	data
)
{
#if 0
	return write_ppm(width, height, filename, data);
#else
	return write_png(width, height, filename, data);
#endif
}

