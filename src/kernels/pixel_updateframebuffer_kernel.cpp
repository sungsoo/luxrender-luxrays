#include "luxrays/kernels/kernels.h"
std::string luxrays::KernelSource_Pixel_UpdateFrameBuffer = 
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
"typedef struct {\n"
"	float r, g, b;\n"
"} Spectrum;\n"
"\n"
"typedef struct {\n"
"	Spectrum radiance;\n"
"	float weight;\n"
"} SamplePixel;\n"
"\n"
"typedef Spectrum Pixel;\n"
"\n"
"static float Clamp(float val, float low, float high) {\n"
"	return (val > low) ? ((val < high) ? val : high) : low;\n"
"}\n"
"\n"
"static unsigned int Floor2UInt(float val) {\n"
"	return (val > 0.f) ? ((unsigned int)floor(val)) : 0;\n"
"}\n"
"\n"
"static float Radiance2PixelFloat(\n"
"		const float x,\n"
"		const unsigned int gammaTableSize,\n"
"		__global float *gammaTable) {\n"
"	//return powf(Clamp(x, 0.f, 1.f), 1.f / 2.2f);\n"
"\n"
"	const unsigned int index = min(\n"
"		Floor2UInt(gammaTableSize * Clamp(x, 0.f, 1.f)),\n"
"			gammaTableSize - 1);\n"
"	return gammaTable[index];\n"
"}\n"
"\n"
"__kernel void PixelUpdateFrameBuffer(\n"
"	const unsigned int width,\n"
"	const unsigned int height,\n"
"	__global SamplePixel *sampleFrameBuffer,\n"
"	__global Pixel *frameBuffer,\n"
"	const unsigned int gammaTableSize,\n"
"	__global float *gammaTable) {\n"
"	const unsigned int offset = get_global_id(0) + get_global_id(1) * get_global_size(0);\n"
"	__global SamplePixel *sp = &sampleFrameBuffer[offset];\n"
"	__global Pixel *p = &frameBuffer[offset];\n"
"\n"
"	const float weight = sp->weight;\n"
"	if (weight == 0.f)\n"
"		return;\n"
"\n"
"	const float invWeight = 1.f / weight;\n"
"	p->r = Radiance2PixelFloat(sp->radiance.r * invWeight, gammaTableSize, gammaTable);\n"
"	p->g = Radiance2PixelFloat(sp->radiance.g * invWeight, gammaTableSize, gammaTable);\n"
"	p->b = Radiance2PixelFloat(sp->radiance.b * invWeight, gammaTableSize, gammaTable);\n"
"}\n"
;
