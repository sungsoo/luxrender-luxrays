#include "pathgpu2/kernels/kernels.h"
std::string luxrays::KernelSource_PathGPU2_samplers = 
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
"void GenerateCameraPath(\n"
"		__global GPUTask *task,\n"
"		__global Ray *ray,\n"
"		Seed *seed\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"		, __global float *cameraData\n"
"#endif\n"
"		) {\n"
"	__global Sample *sample = &task->sample;\n"
"\n"
"	GenerateCameraRay(sample, ray\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"			, &seed\n"
"#endif\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"			, cameraData\n"
"#endif\n"
"			);\n"
"\n"
"	sample->radiance.r = 0.f;\n"
"	sample->radiance.g = 0.f;\n"
"	sample->radiance.b = 0.f;\n"
"\n"
"	// Initialize the path state\n"
"	task->pathState.depth = 0;\n"
"	task->pathState.throughput.r = 1.f;\n"
"	task->pathState.throughput.g = 1.f;\n"
"	task->pathState.throughput.b = 1.f;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"	task->pathState.specularBounce = TRUE;\n"
"#endif\n"
"	task->pathState.state = PATH_STATE_NEXT_VERTEX;\n"
"}\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Inlined Random Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"\n"
"void Sampler_Init(const size_t gid, Seed *seed, __global Sample *sample) {\n"
"	sample->pixelIndex = PixelIndexInt(gid);\n"
"\n"
"	sample->u[IDX_SCREEN_X] = RndFloatValue(seed);\n"
"	sample->u[IDX_SCREEN_Y] = RndFloatValue(seed);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"		, __global float *cameraData\n"
"#endif\n"
"		) {\n"
"	const size_t gid = get_global_id(0);\n"
"	if (gid >= PARAM_TASK_COUNT)\n"
"		return;\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	if (task->pathState.state == PATH_STATE_DONE) {\n"
"		__global Sample *sample = &task->sample;\n"
"\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		// Move to the next assigned pixel\n"
"		sample->pixelIndex = NextPixelIndex(sample->pixelIndex);\n"
"\n"
"		sample->u[IDX_SCREEN_X] = RndFloatValue(&seed);\n"
"		sample->u[IDX_SCREEN_Y] = RndFloatValue(&seed);\n"
"\n"
"		taskStats[gid].sampleCount += 1;\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"				, cameraData\n"
"#endif\n"
"				);\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Random Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 1)\n"
"\n"
"void Sampler_Init(const size_t gid, Seed *seed, __global Sample *sample) {\n"
"	sample->pixelIndex = PixelIndexInt(gid);\n"
"\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		sample->u[i] = RndFloatValue(seed);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"		, __global float *cameraData\n"
"#endif\n"
"		) {\n"
"	const size_t gid = get_global_id(0);\n"
"	if (gid >= PARAM_TASK_COUNT)\n"
"		return;\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	if (task->pathState.state == PATH_STATE_DONE) {\n"
"		__global Sample *sample = &task->sample;\n"
"\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		// Move to the next assigned pixel\n"
"		sample->pixelIndex = NextPixelIndex(sample->pixelIndex);\n"
"\n"
"		for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"			sample->u[i] = RndFloatValue(&seed);\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"				, cameraData\n"
"#endif\n"
"				);\n"
"\n"
"		taskStats[gid].sampleCount += 1;\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"#endif\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Metropolis Sampler Kernel\n"
"//------------------------------------------------------------------------------\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 2)\n"
"\n"
"void Sampler_Init(const size_t gid, Seed *seed, __global Sample *sample) {\n"
"	sample->totalI = 0.f;\n"
"	sample->sampleCount = 0.f;\n"
"\n"
"	sample->current = 0xffffffffu;\n"
"	sample->proposed = 1;\n"
"\n"
"	sample->smallMutationCount = 0;\n"
"	sample->consecutiveRejects = 0;\n"
"\n"
"	sample->weight = 0.f;\n"
"	sample->currentRadiance.r = 0.f;\n"
"	sample->currentRadiance.g = 0.f;\n"
"	sample->currentRadiance.b = 0.f;\n"
"\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i) {\n"
"		sample->u[0][i] = RndFloatValue(seed);\n"
"		sample->u[1][i] = RndFloatValue(seed);\n"
"	}\n"
"}\n"
"\n"
"void LargeStep(Seed *seed, __global float *proposedU) {\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		proposedU[i] = RndFloatValue(seed);\n"
"}\n"
"\n"
"float Mutate(Seed *seed, const float x) {\n"
"	const float s1 = 1.f / 512.f;\n"
"	const float s2 = 1.f / 16.f;\n"
"\n"
"	const float randomValue = RndFloatValue(seed);\n"
"\n"
"	const float dx = s1 / (s1 / s2 + fabs(2.f * randomValue - 1.f)) -\n"
"		s1 / (s1 / s2 + 1.f);\n"
"\n"
"	float mutatedX = x;\n"
"	if (randomValue < 0.5f) {\n"
"		mutatedX += dx;\n"
"		mutatedX = (mutatedX < 1.f) ? mutatedX : (mutatedX - 1.f);\n"
"	} else {\n"
"		mutatedX -= dx;\n"
"		mutatedX = (mutatedX < 0.f) ? (mutatedX + 1.f) : mutatedX;\n"
"	}\n"
"\n"
"	return mutatedX;\n"
"}\n"
"\n"
"void SmallStep(Seed *seed, __global float *currentU, __global float *proposedU) {\n"
"	for (int i = 0; i < TOTAL_U_SIZE; ++i)\n"
"		proposedU[i] = Mutate(seed, currentU[i]);\n"
"}\n"
"\n"
"__kernel void Sampler(\n"
"		__global GPUTask *tasks,\n"
"		__global GPUTaskStats *taskStats,\n"
"		__global Ray *rays\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"		, __global float *cameraData\n"
"#endif\n"
"		) {\n"
"	const size_t gid = get_global_id(0);\n"
"	if (gid >= PARAM_TASK_COUNT)\n"
"		return;\n"
"\n"
"	// Initialize the task\n"
"	__global GPUTask *task = &tasks[gid];\n"
"\n"
"	__global Sample *sample = &task->sample;\n"
"	const uint current = sample->current;\n"
"\n"
"	// Check if it is a complete path and not the very first sample\n"
"	if ((current != 0xffffffffu) && (task->pathState.state == PATH_STATE_DONE)) {\n"
"		// Read the seed\n"
"		Seed seed;\n"
"		seed.s1 = task->seed.s1;\n"
"		seed.s2 = task->seed.s2;\n"
"		seed.s3 = task->seed.s3;\n"
"\n"
"		const uint proposed = sample->proposed;\n"
"		__global float *proposedU = &sample->u[proposed][0];\n"
"\n"
"		if (RndFloatValue(&seed) < PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE) {\n"
"			LargeStep(&seed, proposedU);\n"
"			sample->smallMutationCount = 0;\n"
"		} else {\n"
"			__global float *currentU = &sample->u[current][0];\n"
"\n"
"			SmallStep(&seed, currentU, proposedU);\n"
"			sample->smallMutationCount += 1;\n"
"		}\n"
"\n"
"		taskStats[gid].sampleCount += 1;\n"
"\n"
"		GenerateCameraPath(task, &rays[gid], &seed\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"				, cameraData\n"
"#endif\n"
"				);\n"
"\n"
"		// Save the seed\n"
"		task->seed.s1 = seed.s1;\n"
"		task->seed.s2 = seed.s2;\n"
"		task->seed.s3 = seed.s3;\n"
"	}\n"
"}\n"
"\n"
"void Sampler_MLT_SplatSample(__global Pixel *frameBuffer, Seed *seed, __global Sample *sample) {\n"
"	uint current = sample->current;\n"
"	uint proposed = sample->proposed;\n"
"\n"
"	Spectrum radiance = sample->radiance;\n"
"\n"
"	if (current == 0xffffffffu) {\n"
"		// It is the very first sample, I have still to initialize the current\n"
"		// sample\n"
"\n"
"		sample->currentRadiance = radiance;\n"
"		sample->totalI = Spectrum_Y(&radiance);\n"
"\n"
"		// The following 2 lines could be moved in the initialization code\n"
"		sample->sampleCount = 1;\n"
"		sample->weight = 0.f;\n"
"\n"
"		current = proposed;\n"
"		proposed ^= 1;\n"
"	} else {\n"
"		const Spectrum currentL = sample->currentRadiance;\n"
"		const float currentI = Spectrum_Y(&currentL);\n"
"\n"
"		const Spectrum proposedL = radiance;\n"
"		float proposedI = Spectrum_Y(&proposedL);\n"
"		proposedI = isinf(proposedI) ? 0.f : proposedI;\n"
"\n"
"		float totalI = sample->totalI;\n"
"		uint sampleCount = sample->sampleCount;\n"
"		uint smallMutationCount = sample->smallMutationCount;\n"
"		if (smallMutationCount == 0) {\n"
"			// It is a large mutation\n"
"			totalI += Spectrum_Y(&proposedL);\n"
"			sampleCount += 1;\n"
"\n"
"			sample->totalI = totalI;\n"
"			sample->sampleCount = sampleCount;\n"
"		}\n"
"\n"
"		const float meanI = (totalI > 0.f) ? (totalI / sampleCount) : 1.f;\n"
"\n"
"		// Calculate accept probability from old and new image sample\n"
"		uint consecutiveRejects = sample->consecutiveRejects;\n"
"\n"
"		float accProb;\n"
"		if ((currentI > 0.f) && (consecutiveRejects < PARAM_SAMPLER_METROPOLIS_MAX_CONSECUTIVE_REJECT))\n"
"			accProb = min(1.f, proposedI / currentI);\n"
"		else\n"
"			accProb = 1.f;\n"
"\n"
"		const float newWeight = accProb + ((smallMutationCount == 0) ? 1.f : 0.f);\n"
"		float weight = sample->weight;\n"
"		weight += 1.f - accProb;\n"
"\n"
"		const float rndVal = RndFloatValue(seed);\n"
"\n"
"		/*if (get_global_id(0) == 0)\n"
"			printf(\"[%d] Current: (%f, %f, %f) [%f] Proposed: (%f, %f, %f) [%f] accProb: %f <%f>\\n\",\n"
"					smallMutationCount,\n"
"					currentL.r, currentL.g, currentL.b, weight,\n"
"					proposedL.r, proposedL.g, proposedL.b, newWeight,\n"
"					accProb, rndVal);*/\n"
"\n"
"		Spectrum contrib;\n"
"		float norm;\n"
"#if (PARAM_IMAGE_FILTER_TYPE != 0)\n"
"		float sx, sy;\n"
"#endif\n"
"		uint pixelIndex;\n"
"		if ((accProb == 1.f) || (rndVal < accProb)) {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\"\\t\\tACCEPTED !\\n\");*/\n"
"\n"
"			// Add accumulated contribution of previous reference sample\n"
"			norm = weight / (currentI / meanI + PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE);\n"
"			contrib = currentL;\n"
"\n"
"			pixelIndex = PixelIndexFloat(sample->u[current][IDX_PIXEL_INDEX]);\n"
"#if (PARAM_IMAGE_FILTER_TYPE != 0)\n"
"			sx = sample->u[current][IDX_SCREEN_X];\n"
"			sy = sample->u[current][IDX_SCREEN_Y];\n"
"#endif\n"
"\n"
"#if defined(PARAM_SAMPLER_METROPOLIS_DEBUG_SHOW_SAMPLE_DENSITY)\n"
"			// Debug code: to check sample distribution\n"
"			contrib.r = contrib.g = contrib.b = (consecutiveRejects + 1.f)  * .01f;\n"
"			SplatSample(frameBuffer, pixelIndex, &contrib, 1.f);\n"
"#endif\n"
"\n"
"			current ^= 1;\n"
"			proposed ^= 1;\n"
"			consecutiveRejects = 0;\n"
"\n"
"			weight = newWeight;\n"
"\n"
"			sample->currentRadiance = proposedL;\n"
"		} else {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\"\\t\\tREJECTED !\\n\");*/\n"
"\n"
"			// Add contribution of new sample before rejecting it\n"
"			norm = newWeight / (proposedI / meanI + PARAM_SAMPLER_METROPOLIS_LARGE_STEP_RATE);\n"
"			contrib = proposedL;\n"
"\n"
"			pixelIndex = PixelIndexFloat(sample->u[proposed][IDX_PIXEL_INDEX]);\n"
"#if (PARAM_IMAGE_FILTER_TYPE != 0)\n"
"			sx = sample->u[proposed][IDX_SCREEN_X];\n"
"			sy = sample->u[proposed][IDX_SCREEN_Y];\n"
"#endif\n"
"\n"
"			++consecutiveRejects;\n"
"\n"
"#if defined(PARAM_SAMPLER_METROPOLIS_DEBUG_SHOW_SAMPLE_DENSITY)\n"
"			// Debug code: to check sample distribution\n"
"			contrib.r = contrib.g = contrib.b = 1.f * .01f;\n"
"			SplatSample(frameBuffer, pixelIndex, &contrib, 1.f);\n"
"#endif\n"
"		}\n"
"\n"
"#if !defined(PARAM_SAMPLER_METROPOLIS_DEBUG_SHOW_SAMPLE_DENSITY)\n"
"		if (norm > 0.f) {\n"
"			/*if (get_global_id(0) == 0)\n"
"				printf(\"\\t\\tPixelIndex: %d Contrib: (%f, %f, %f) [%f] consecutiveRejects: %d\\n\",\n"
"						pixelIndex, contrib.r, contrib.g, contrib.b, norm, consecutiveRejects);*/\n"
"\n"
"#if (PARAM_IMAGE_FILTER_TYPE == 0)\n"
"			SplatSample(frameBuffer, pixelIndex, &contrib, norm);\n"
"#else\n"
"			SplatSample(frameBuffer, pixelIndex, sx, sy, &contrib, norm);\n"
"#endif\n"
"		}\n"
"#endif\n"
"\n"
"		sample->weight = weight;\n"
"		sample->consecutiveRejects = consecutiveRejects;\n"
"	}\n"
"\n"
"	sample->current = current;\n"
"	sample->proposed = proposed;\n"
"}\n"
"\n"
"#endif\n"
;
