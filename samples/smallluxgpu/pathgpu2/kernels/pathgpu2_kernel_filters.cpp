#include "pathgpu2/kernels/kernels.h"
std::string luxrays::KernelSource_PathGPU2_filters = 
"/***************************************************************************\n"
" *   Copyright (C) 1998-2010 by authors (see AUTHORS.txt )                 *\n"
" *                                                                         *\n"
" *   This file is part of LuxRays.                                         *\n"
" *                                                                         *\n"
" *   LuxRays is free software; you can redistribute it and/or modify       *\n"
" *   it under the terms of the GNU General Public License as published by  *\n"
" *   the Free Software Foundation; either version 3 of the License, or     *\n"
" *   (at your option) any later version.                                   *\n"
" *                                                                         *\n"
" *   LuxRays is distributed in the hope that it will be useful,            *\n"
" *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *\n"
" *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *\n"
" *   GNU General Public License for more details.                          *\n"
" *                                                                         *\n"
" *   You should have received a copy of the GNU General Public License     *\n"
" *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *\n"
" *                                                                         *\n"
" *   LuxRays website: http://www.luxrender.net                             *\n"
" ***************************************************************************/\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Pixel related functions\n"
"//------------------------------------------------------------------------------\n"
"\n"
"void PixelIndex2XY(const uint index, uint *x, uint *y) {\n"
"	*y = index / PARAM_IMAGE_WIDTH;\n"
"	*x = index - (*y) * PARAM_IMAGE_WIDTH;\n"
"}\n"
"\n"
"uint XY2PixelIndex(const uint x, const uint y) {\n"
"	return x + y * PARAM_IMAGE_WIDTH;\n"
"}\n"
"\n"
"uint PixelIndexInt(const size_t gid) {\n"
"	return gid % (PARAM_IMAGE_WIDTH * PARAM_IMAGE_HEIGHT);\n"
"}\n"
"\n"
"uint NextPixelIndex(const uint i) {\n"
"	return (i + PARAM_TASK_COUNT) % (PARAM_IMAGE_WIDTH * PARAM_IMAGE_HEIGHT);\n"
"}\n"
"\n"
"uint PixelIndexFloat(const float u) {\n"
"	const uint pixelCountPerTask = PARAM_IMAGE_WIDTH * PARAM_IMAGE_HEIGHT;\n"
"	const uint i = min((uint)floor(pixelCountPerTask * u), pixelCountPerTask - 1);\n"
"\n"
"	return i;\n"
"}\n"
"\n"
"uint PixelIndexFloat2D(const float ux, const float uy) {\n"
"	const uint x = min((uint)floor(PARAM_IMAGE_WIDTH * ux), (uint)(PARAM_IMAGE_WIDTH - 1));\n"
"	const uint y = min((uint)floor(PARAM_IMAGE_HEIGHT * uy), (uint)(PARAM_IMAGE_HEIGHT - 1));\n"
"\n"
"	return XY2PixelIndex(x, y);\n"
"}\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Image filtering related functions\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_IMAGE_FILTER_TYPE == 0)\n"
"\n"
"// Nothing\n"
"\n"
"#elif (PARAM_IMAGE_FILTER_TYPE == 1)\n"
"\n"
"// Box Filter\n"
"float ImageFilter_Evaluate(const float x, const float y) {\n"
"	return 1.f;\n"
"}\n"
"\n"
"#elif (PARAM_IMAGE_FILTER_TYPE == 2)\n"
"\n"
"float Gaussian(const float d, const float expv) {\n"
"	return max(0.f, native_exp(-PARAM_IMAGE_FILTER_GAUSSIAN_ALPHA * d * d) - expv);\n"
"}\n"
"\n"
"// Gaussian Filter\n"
"float ImageFilter_Evaluate(const float x, const float y) {\n"
"	return Gaussian(x,\n"
"			native_exp(-PARAM_IMAGE_FILTER_GAUSSIAN_ALPHA * PARAM_IMAGE_FILTER_WIDTH_X * PARAM_IMAGE_FILTER_WIDTH_X)) *\n"
"		Gaussian(y, \n"
"			native_exp(-PARAM_IMAGE_FILTER_GAUSSIAN_ALPHA * PARAM_IMAGE_FILTER_WIDTH_Y * PARAM_IMAGE_FILTER_WIDTH_Y));\n"
"}\n"
"\n"
"#elif (PARAM_IMAGE_FILTER_TYPE == 3)\n"
"\n"
"float Mitchell1D(float x) {\n"
"	const float B = PARAM_IMAGE_FILTER_MITCHELL_B;\n"
"	const float C = PARAM_IMAGE_FILTER_MITCHELL_C;\n"
"\n"
"	if (x >= 1.f)\n"
"		return 0.f;\n"
"	x = fabs(2.f * x);\n"
"\n"
"	if (x > 1.f)\n"
"		return (((-B / 6.f - C) * x + (B + 5.f * C)) * x +\n"
"			(-2.f * B - 8.f * C)) * x + (4.f / 3.f * B + 4.f * C);\n"
"	else\n"
"		return ((2.f - 1.5f * B - C) * x +\n"
"			(-3.f + 2.f * B + C)) * x * x +\n"
"			(1.f - B / 3.f);\n"
"}\n"
"\n"
"// Mitchell Filter\n"
"float ImageFilter_Evaluate(const float x, const float y) {\n"
"	const float distance = native_sqrt(\n"
"			x * x * (1.f / (PARAM_IMAGE_FILTER_WIDTH_X * PARAM_IMAGE_FILTER_WIDTH_X)) +\n"
"			y * y * (1.f / (PARAM_IMAGE_FILTER_WIDTH_Y * PARAM_IMAGE_FILTER_WIDTH_Y)));\n"
"\n"
"	return Mitchell1D(distance);\n"
"}\n"
"\n"
"#elif (PARAM_IMAGE_FILTER_TYPE == 4)\n"
"\n"
"float Mitchell1D(float x) {\n"
"	const float B = PARAM_IMAGE_FILTER_MITCHELL_B;\n"
"	const float C = PARAM_IMAGE_FILTER_MITCHELL_C;\n"
"\n"
"	if (x >= 1.f)\n"
"		return 0.f;\n"
"	x = fabs(2.f * x);\n"
"\n"
"	if (x > 1.f)\n"
"		return (((-B / 6.f - C) * x + (B + 5.f * C)) * x +\n"
"			(-2.f * B - 8.f * C)) * x + (4.f / 3.f * B + 4.f * C);\n"
"	else\n"
"		return ((2.f - 1.5f * B - C) * x +\n"
"			(-3.f + 2.f * B + C)) * x * x +\n"
"			(1.f - B / 3.f);\n"
"}\n"
"\n"
"// Mitchell Filter with Super Sampling\n"
"float ImageFilter_Evaluate(const float x, const float y) {\n"
"	const float distance = native_sqrt(\n"
"			x * x * (1.f / (PARAM_IMAGE_FILTER_WIDTH_X * 5.f / 3.f * PARAM_IMAGE_FILTER_WIDTH_X * 5.f / 3.f)) +\n"
"			y * y * (1.f / (PARAM_IMAGE_FILTER_WIDTH_Y * 5.f / 3.f * PARAM_IMAGE_FILTER_WIDTH_Y * 5.f / 3.f)));\n"
"\n"
"	const float dist = distance / .6f;\n"
"	const float B = PARAM_IMAGE_FILTER_MITCHELL_B;\n"
"	const float C = PARAM_IMAGE_FILTER_MITCHELL_C;\n"
"	const float a0 = (76.f - 16.f * B + 8.f * C) / 81.f;\n"
"	const float a1 = (1.f - a0)/ 2.f;\n"
"\n"
"	return a1 * Mitchell1D(dist - 2.f / 3.f) +\n"
"			a0 * Mitchell1D(dist) +\n"
"			a1 * Mitchell1D(dist + 2.f / 3.f);\n"
"}\n"
"\n"
"#else\n"
"\n"
"Error: unknown image filter !!!\n"
"\n"
"#endif\n"
"\n"
"void Pixel_AddRadiance(__global Pixel *pixel, Spectrum *rad, const float weight) {\n"
"	/*if (isnan(rad->r) || isinf(rad->r) ||\n"
"			isnan(rad->g) || isinf(rad->g) ||\n"
"			isnan(rad->b) || isinf(rad->b) ||\n"
"			isnan(weight) || isinf(weight))\n"
"		printf(\"(NaN/Inf. error: (%f, %f, %f) [%f]\\n\", rad->r, rad->g, rad->b, weight);*/\n"
"\n"
"	float4 s;\n"
"	s.x = rad->r;\n"
"	s.y = rad->g;\n"
"	s.z = rad->b;\n"
"	s.w = 1.f;\n"
"	s *= weight;\n"
"\n"
"#if defined(PARAM_USE_PIXEL_ATOMICS)\n"
"	AtomicAdd(&pixel->c.r, s.x);\n"
"	AtomicAdd(&pixel->c.g, s.y);\n"
"	AtomicAdd(&pixel->c.b, s.z);\n"
"	AtomicAdd(&pixel->count, s.w);\n"
"\n"
"#else\n"
"	float4 p = *((__global float4 *)pixel);\n"
"	p += s;\n"
"	*((__global float4 *)pixel) = p;\n"
"#endif\n"
"}\n"
"\n"
"#if (PARAM_IMAGE_FILTER_TYPE == 1) || (PARAM_IMAGE_FILTER_TYPE == 2) || (PARAM_IMAGE_FILTER_TYPE == 3) || (PARAM_IMAGE_FILTER_TYPE == 4)\n"
"void Pixel_AddFilteredRadiance(__global Pixel *pixel, Spectrum *rad,\n"
"	const float distX, const float distY, const float weight) {\n"
"	const float filterWeight = ImageFilter_Evaluate(distX, distY);\n"
"\n"
"	Pixel_AddRadiance(pixel, rad, weight * filterWeight)\n"
"}\n"
"#endif\n"
"\n"
"#if (PARAM_IMAGE_FILTER_TYPE == 0)\n"
"\n"
"void SplatSample(__global Pixel *frameBuffer, const uint pixelIndex, Spectrum *radiance, const float weight) {\n"
"		__global Pixel *pixel = &frameBuffer[pixelIndex];\n"
"\n"
"		Pixel_AddRadiance(pixel, radiance, weight);\n"
"}\n"
"\n"
"#elif (PARAM_IMAGE_FILTER_TYPE == 1) || (PARAM_IMAGE_FILTER_TYPE == 2) || (PARAM_IMAGE_FILTER_TYPE == 3) || (PARAM_IMAGE_FILTER_TYPE == 4)\n"
"\n"
"void SplatSample(__global Pixel *frameBuffer, const uint pixelIndex, const float sx, const float sy, Spectrum *radiance, const float weight) {\n"
"		uint bufferPixelIndex = pixelIndex * 9;\n"
"\n"
"		__global Pixel *pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx + 1.f, sy + 1.f, weight);\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx      , sy + 1.f, weight);\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx - 1.f, sy + 1.f, weight);\n"
"\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx + 1.f, sy, weight);\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx      , sy, weight);\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx - 1.f, sy, weight);\n"
"\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx + 1.f, sy - 1.f, weight);\n"
"		pixel = &frameBuffer[bufferPixelIndex++];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx      , sy - 1.f, weight);\n"
"		pixel = &frameBuffer[bufferPixelIndex];\n"
"		Pixel_AddFilteredRadiance(pixel, radiance, sx - 1.f, sy - 1.f, weight);\n"
"}\n"
"\n"
"#else\n"
"\n"
"Error: unknown image filter !!!\n"
"\n"
"#endif\n"
;
