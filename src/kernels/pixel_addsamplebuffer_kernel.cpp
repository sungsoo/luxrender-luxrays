#include "luxrays/kernels/kernels.h"
std::string luxrays::KernelSource_Pixel_AddSampleBuffer = 
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
"// NOTE: this kernel assume samples do not overlap\n"
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
"typedef struct {\n"
"	float screenX, screenY;\n"
"	Spectrum radiance;\n"
"} SampleBufferElem;\n"
"\n"
"void AddSample(__global SamplePixel *sp, const float4 sample) {\n"
"    __global float4 *p = (__global float4 *)sp;\n"
"    *p += sample;\n"
"}\n"
"\n"
"__kernel __attribute__((reqd_work_group_size(64, 1, 1))) void PixelAddSampleBuffer(\n"
"	const unsigned int width,\n"
"	const unsigned int height,\n"
"	__global SamplePixel *sampleFrameBuffer,\n"
"	const unsigned int sampleCount,\n"
"	__global SampleBufferElem *sampleBuff) {\n"
"	const unsigned int index = get_global_id(0);\n"
"	if (index >= sampleCount)\n"
"		return;\n"
"\n"
"	__global SampleBufferElem *sampleElem = &sampleBuff[index];\n"
"	const unsigned int x = (unsigned int)sampleElem->screenX;\n"
"	const unsigned int y = (unsigned int)sampleElem->screenY;\n"
"    const float4 sample = (float4)(sampleElem->radiance.r, sampleElem->radiance.g, sampleElem->radiance.b, 1.f);\n"
"\n"
"    AddSample(&sampleFrameBuffer[x + y * width], sample);\n"
"}\n"
;
