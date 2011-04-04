#include "pathgpu2/kernels/kernels.h"
std::string luxrays::KernelSource_PathGPU2_scene = 
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
"void TexMap_GetTexel(__global Spectrum *pixels, const uint width, const uint height,\n"
"		const int s, const int t, Spectrum *col) {\n"
"	const uint u = Mod(s, width);\n"
"	const uint v = Mod(t, height);\n"
"\n"
"	const unsigned index = v * width + u;\n"
"\n"
"	col->r = pixels[index].r;\n"
"	col->g = pixels[index].g;\n"
"	col->b = pixels[index].b;\n"
"}\n"
"\n"
"float TexMap_GetAlphaTexel(__global float *alphas, const uint width, const uint height,\n"
"		const int s, const int t) {\n"
"	const uint u = Mod(s, width);\n"
"	const uint v = Mod(t, height);\n"
"\n"
"	const unsigned index = v * width + u;\n"
"\n"
"	return alphas[index];\n"
"}\n"
"\n"
"void TexMap_GetColor(__global Spectrum *pixels, const uint width, const uint height,\n"
"		const float u, const float v, Spectrum *col) {\n"
"	const float s = u * width - 0.5f;\n"
"	const float t = v * height - 0.5f;\n"
"\n"
"	const int s0 = (int)floor(s);\n"
"	const int t0 = (int)floor(t);\n"
"\n"
"	const float ds = s - s0;\n"
"	const float dt = t - t0;\n"
"\n"
"	const float ids = 1.f - ds;\n"
"	const float idt = 1.f - dt;\n"
"\n"
"	Spectrum c0, c1, c2, c3;\n"
"	TexMap_GetTexel(pixels, width, height, s0, t0, &c0);\n"
"	TexMap_GetTexel(pixels, width, height, s0, t0 + 1, &c1);\n"
"	TexMap_GetTexel(pixels, width, height, s0 + 1, t0, &c2);\n"
"	TexMap_GetTexel(pixels, width, height, s0 + 1, t0 + 1, &c3);\n"
"\n"
"	const float k0 = ids * idt;\n"
"	const float k1 = ids * dt;\n"
"	const float k2 = ds * idt;\n"
"	const float k3 = ds * dt;\n"
"\n"
"	col->r = k0 * c0.r + k1 * c1.r + k2 * c2.r + k3 * c3.r;\n"
"	col->g = k0 * c0.g + k1 * c1.g + k2 * c2.g + k3 * c3.g;\n"
"	col->b = k0 * c0.b + k1 * c1.b + k2 * c2.b + k3 * c3.b;\n"
"}\n"
"\n"
"float TexMap_GetAlpha(__global float *alphas, const uint width, const uint height,\n"
"		const float u, const float v) {\n"
"	const float s = u * width - 0.5f;\n"
"	const float t = v * height - 0.5f;\n"
"\n"
"	const int s0 = (int)floor(s);\n"
"	const int t0 = (int)floor(t);\n"
"\n"
"	const float ds = s - s0;\n"
"	const float dt = t - t0;\n"
"\n"
"	const float ids = 1.f - ds;\n"
"	const float idt = 1.f - dt;\n"
"\n"
"	const float c0 = TexMap_GetAlphaTexel(alphas, width, height, s0, t0);\n"
"	const float c1 = TexMap_GetAlphaTexel(alphas, width, height, s0, t0 + 1);\n"
"	const float c2 = TexMap_GetAlphaTexel(alphas, width, height, s0 + 1, t0);\n"
"	const float c3 = TexMap_GetAlphaTexel(alphas, width, height, s0 + 1, t0 + 1);\n"
"\n"
"	const float k0 = ids * idt;\n"
"	const float k1 = ids * dt;\n"
"	const float k2 = ds * idt;\n"
"	const float k3 = ds * dt;\n"
"\n"
"	return k0 * c0 + k1 * c1 + k2 * c2 + k3 * c3;\n"
"}\n"
"\n"
"#if defined(PARAM_HAVE_INFINITELIGHT)\n"
"void InfiniteLight_Le(__global Spectrum *infiniteLightMap, Spectrum *le, const Vector *dir) {\n"
"	const float u = 1.f - SphericalPhi(dir) * INV_TWOPI +  PARAM_IL_SHIFT_U;\n"
"	const float v = SphericalTheta(dir) * INV_PI + PARAM_IL_SHIFT_V;\n"
"\n"
"	TexMap_GetColor(infiniteLightMap, PARAM_IL_WIDTH, PARAM_IL_HEIGHT, u, v, le);\n"
"\n"
"	le->r *= PARAM_IL_GAIN_R;\n"
"	le->g *= PARAM_IL_GAIN_G;\n"
"	le->b *= PARAM_IL_GAIN_B;\n"
"}\n"
"#endif\n"
"\n"
"void Mesh_InterpolateColor(__global Spectrum *colors, __global Triangle *triangles,\n"
"		const uint triIndex, const float b1, const float b2, Spectrum *C) {\n"
"	__global Triangle *tri = &triangles[triIndex];\n"
"\n"
"	const float b0 = 1.f - b1 - b2;\n"
"	C->r = b0 * colors[tri->v0].r + b1 * colors[tri->v1].r + b2 * colors[tri->v2].r;\n"
"	C->g = b0 * colors[tri->v0].g + b1 * colors[tri->v1].g + b2 * colors[tri->v2].g;\n"
"	C->b = b0 * colors[tri->v0].b + b1 * colors[tri->v1].b + b2 * colors[tri->v2].b;\n"
"}\n"
"\n"
"void Mesh_InterpolateNormal(__global Vector *normals, __global Triangle *triangles,\n"
"		const uint triIndex, const float b1, const float b2, Vector *N) {\n"
"	__global Triangle *tri = &triangles[triIndex];\n"
"\n"
"	const float b0 = 1.f - b1 - b2;\n"
"	N->x = b0 * normals[tri->v0].x + b1 * normals[tri->v1].x + b2 * normals[tri->v2].x;\n"
"	N->y = b0 * normals[tri->v0].y + b1 * normals[tri->v1].y + b2 * normals[tri->v2].y;\n"
"	N->z = b0 * normals[tri->v0].z + b1 * normals[tri->v1].z + b2 * normals[tri->v2].z;\n"
"\n"
"	Normalize(N);\n"
"}\n"
"\n"
"void Mesh_InterpolateUV(__global UV *uvs, __global Triangle *triangles,\n"
"		const uint triIndex, const float b1, const float b2, UV *uv) {\n"
"	__global Triangle *tri = &triangles[triIndex];\n"
"\n"
"	const float b0 = 1.f - b1 - b2;\n"
"	uv->u = b0 * uvs[tri->v0].u + b1 * uvs[tri->v1].u + b2 * uvs[tri->v2].u;\n"
"	uv->v = b0 * uvs[tri->v0].v + b1 * uvs[tri->v1].v + b2 * uvs[tri->v2].v;\n"
"}\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Materials\n"
"//------------------------------------------------------------------------------\n"
"\n"
"void Matte_Sample_f(__global MatteParam *mat, const Vector *wo, Vector *wi,\n"
"		float *pdf, Spectrum *f, const Vector *shadeN,\n"
"		const float u0, const float u1\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"		) {\n"
"	Vector dir;\n"
"	CosineSampleHemisphere(&dir, u0, u1);\n"
"	*pdf = dir.z * INV_PI;\n"
"\n"
"	Vector v1, v2;\n"
"	CoordinateSystem(shadeN, &v1, &v2);\n"
"\n"
"	wi->x = v1.x * dir.x + v2.x * dir.y + shadeN->x * dir.z;\n"
"	wi->y = v1.y * dir.x + v2.y * dir.y + shadeN->y * dir.z;\n"
"	wi->z = v1.z * dir.x + v2.z * dir.y + shadeN->z * dir.z;\n"
"\n"
"	const float dp = Dot(shadeN, wi);\n"
"	// Using 0.0001 instead of 0.0 to cut down fireflies\n"
"	if (dp <= 0.0001f)\n"
"		*pdf = 0.f;\n"
"	else {\n"
"		*pdf /=  dp;\n"
"\n"
"		f->r = mat->r * INV_PI;\n"
"		f->g = mat->g * INV_PI;\n"
"		f->b = mat->b * INV_PI;\n"
"	}\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"	*specularBounce = FALSE;\n"
"#endif\n"
"}\n"
"\n"
"void Mirror_Sample_f(__global MirrorParam *mat, const Vector *wo, Vector *wi,\n"
"		float *pdf, Spectrum *f, const Vector *shadeN\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"		) {\n"
"    const float k = 2.f * Dot(shadeN, wo);\n"
"	wi->x = k * shadeN->x - wo->x;\n"
"	wi->y = k * shadeN->y - wo->y;\n"
"	wi->z = k * shadeN->z - wo->z;\n"
"\n"
"	*pdf = 1.f;\n"
"\n"
"	f->r = mat->r;\n"
"	f->g = mat->g;\n"
"	f->b = mat->b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"	*specularBounce = mat->specularBounce;\n"
"#endif\n"
"}\n"
"\n"
"void Glass_Sample_f(__global GlassParam *mat,\n"
"    const Vector *wo, Vector *wi, float *pdf, Spectrum *f, const Vector *N, const Vector *shadeN,\n"
"    const float u0\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"        ) {\n"
"    Vector reflDir;\n"
"    const float k = 2.f * Dot(N, wo);\n"
"    reflDir.x = k * N->x - wo->x;\n"
"    reflDir.y = k * N->y - wo->y;\n"
"    reflDir.z = k * N->z - wo->z;\n"
"\n"
"    // Ray from outside going in ?\n"
"    const bool into = (Dot(N, shadeN) > 0.f);\n"
"\n"
"    const float nc = mat->ousideIor;\n"
"    const float nt = mat->ior;\n"
"    const float nnt = into ? (nc / nt) : (nt / nc);\n"
"    const float ddn = -Dot(wo, shadeN);\n"
"    const float cos2t = 1.f - nnt * nnt * (1.f - ddn * ddn);\n"
"\n"
"    // Total internal reflection\n"
"    if (cos2t < 0.f) {\n"
"        *wi = reflDir;\n"
"        *pdf = 1.f;\n"
"\n"
"        f->r = mat->refl_r;\n"
"        f->g = mat->refl_g;\n"
"        f->b = mat->refl_b;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"        *specularBounce = mat->reflectionSpecularBounce;\n"
"#endif\n"
"    } else {\n"
"        const float kk = (into ? 1.f : -1.f) * (ddn * nnt + sqrt(cos2t));\n"
"        Vector nkk = *N;\n"
"        nkk.x *= kk;\n"
"        nkk.y *= kk;\n"
"        nkk.z *= kk;\n"
"\n"
"        Vector transDir;\n"
"        transDir.x = -nnt * wo->x - nkk.x;\n"
"        transDir.y = -nnt * wo->y - nkk.y;\n"
"        transDir.z = -nnt * wo->z - nkk.z;\n"
"        Normalize(&transDir);\n"
"\n"
"        const float c = 1.f - (into ? -ddn : Dot(&transDir, N));\n"
"\n"
"        const float R0 = mat->R0;\n"
"        const float Re = R0 + (1.f - R0) * c * c * c * c * c;\n"
"        const float Tr = 1.f - Re;\n"
"        const float P = .25f + .5f * Re;\n"
"\n"
"        if (Tr == 0.f) {\n"
"            if (Re == 0.f)\n"
"                *pdf = 0.f;\n"
"            else {\n"
"                *wi = reflDir;\n"
"                *pdf = 1.f;\n"
"\n"
"                f->r = mat->refl_r;\n"
"                f->g = mat->refl_g;\n"
"                f->b = mat->refl_b;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"                *specularBounce = mat->reflectionSpecularBounce;\n"
"#endif\n"
"            }\n"
"        } else if (Re == 0.f) {\n"
"            *wi = transDir;\n"
"            *pdf = 1.f;\n"
"\n"
"            f->r = mat->refrct_r;\n"
"            f->g = mat->refrct_g;\n"
"            f->b = mat->refrct_b;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = mat->transmitionSpecularBounce;\n"
"#endif\n"
"        } else if (u0 < P) {\n"
"            *wi = reflDir;\n"
"            *pdf = P / Re;\n"
"\n"
"            f->r = mat->refl_r;\n"
"            f->g = mat->refl_g;\n"
"            f->b = mat->refl_b;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = mat->reflectionSpecularBounce;\n"
"#endif\n"
"        } else {\n"
"            *wi = transDir;\n"
"            *pdf = (1.f - P) / Tr;\n"
"\n"
"            f->r = mat->refrct_r;\n"
"            f->g = mat->refrct_g;\n"
"            f->b = mat->refrct_b;\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = mat->transmitionSpecularBounce;\n"
"#endif\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"void MatteMirror_Sample_f(__global MatteMirrorParam *mat, const Vector *wo, Vector *wi,\n"
"		float *pdf, Spectrum *f, const Vector *shadeN,\n"
"		const float u0, const float u1, const float u2\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"		) {\n"
"    const float totFilter = mat->totFilter;\n"
"    const float comp = u2 * totFilter;\n"
"\n"
"    if (comp > mat->matteFilter) {\n"
"        Mirror_Sample_f(&mat->mirror, wo, wi, pdf, f, shadeN\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            , specularBounce\n"
"#endif\n"
"            );\n"
"        *pdf *= mat->mirrorPdf;\n"
"    } else {\n"
"        Matte_Sample_f(&mat->matte, wo, wi, pdf, f, shadeN, u0, u1\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            , specularBounce\n"
"#endif\n"
"            );\n"
"        *pdf *= mat->mattePdf;\n"
"    }\n"
"}\n"
"\n"
"void GlossyReflection(const Vector *wo, Vector *wi, const float exponent,\n"
"		const Vector *shadeN, const float u0, const float u1) {\n"
"    const float phi = 2.f * M_PI * u0;\n"
"    const float cosTheta = pow(1.f - u1, exponent);\n"
"    const float sinTheta = sqrt(1.f - cosTheta * cosTheta);\n"
"    const float x = cos(phi) * sinTheta;\n"
"    const float y = sin(phi) * sinTheta;\n"
"    const float z = cosTheta;\n"
"\n"
"    Vector w;\n"
"    const float RdotShadeN = Dot(shadeN, wo);\n"
"	w.x = (2.f * RdotShadeN) * shadeN->x - wo->x;\n"
"	w.y = (2.f * RdotShadeN) * shadeN->y - wo->y;\n"
"	w.z = (2.f * RdotShadeN) * shadeN->z - wo->z;\n"
"\n"
"    Vector u, a;\n"
"    if (fabs(shadeN->x) > .1f) {\n"
"        a.x = 0.f;\n"
"        a.y = 1.f;\n"
"    } else {\n"
"        a.x = 1.f;\n"
"        a.y = 0.f;\n"
"    }\n"
"    a.z = 0.f;\n"
"    Cross(&u, &a, &w);\n"
"    Normalize(&u);\n"
"    Vector v;\n"
"    Cross(&v, &w, &u);\n"
"\n"
"    wi->x = x * u.x + y * v.x + z * w.x;\n"
"    wi->y = x * u.y + y * v.y + z * w.y;\n"
"    wi->z = x * u.z + y * v.z + z * w.z;\n"
"}\n"
"\n"
"void Metal_Sample_f(__global MetalParam *mat, const Vector *wo, Vector *wi,\n"
"		float *pdf, Spectrum *f, const Vector *shadeN,\n"
"		const float u0, const float u1\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"		) {\n"
"        GlossyReflection(wo, wi, mat->exponent, shadeN, u0, u1);\n"
"\n"
"		if (Dot(wi, shadeN) > 0.f) {\n"
"			*pdf = 1.f;\n"
"\n"
"            f->r = mat->r;\n"
"            f->g = mat->g;\n"
"            f->b = mat->b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = mat->specularBounce;\n"
"#endif\n"
"		} else\n"
"			*pdf = 0.f;\n"
"}\n"
"\n"
"void MatteMetal_Sample_f(__global MatteMetalParam *mat, const Vector *wo, Vector *wi,\n"
"		float *pdf, Spectrum *f, const Vector *shadeN,\n"
"		const float u0, const float u1, const float u2\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"		) {\n"
"        const float totFilter = mat->totFilter;\n"
"        const float comp = u2 * totFilter;\n"
"\n"
"		if (comp > mat->matteFilter) {\n"
"            Metal_Sample_f(&mat->metal, wo, wi, pdf, f, shadeN, u0, u1\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"                , specularBounce\n"
"#endif\n"
"                );\n"
"			*pdf *= mat->metalPdf;\n"
"		} else {\n"
"            Matte_Sample_f(&mat->matte, wo, wi, pdf, f, shadeN, u0, u1\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"                , specularBounce\n"
"#endif\n"
"                );\n"
"			*pdf *= mat->mattePdf;\n"
"		}\n"
"}\n"
"\n"
"void Alloy_Sample_f(__global AlloyParam *mat, const Vector *wo, Vector *wi,\n"
"		float *pdf, Spectrum *f, const Vector *shadeN,\n"
"		const float u0, const float u1, const float u2\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"		) {\n"
"    // Schilick's approximation\n"
"    const float c = 1.f - Dot(wo, shadeN);\n"
"    const float R0 = mat->R0;\n"
"    const float Re = R0 + (1.f - R0) * c * c * c * c * c;\n"
"\n"
"    const float P = .25f + .5f * Re;\n"
"\n"
"    if (u2 < P) {\n"
"        GlossyReflection(wo, wi, mat->exponent, shadeN, u0, u1);\n"
"        *pdf = P / Re;\n"
"\n"
"        f->r = Re * mat->refl_r;\n"
"        f->g = Re * mat->refl_g;\n"
"        f->b = Re * mat->refl_b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"        *specularBounce = mat->specularBounce;\n"
"#endif\n"
"    } else {\n"
"        Vector dir;\n"
"        CosineSampleHemisphere(&dir, u0, u1);\n"
"        *pdf = dir.z * INV_PI;\n"
"\n"
"        Vector v1, v2;\n"
"        CoordinateSystem(shadeN, &v1, &v2);\n"
"\n"
"        wi->x = v1.x * dir.x + v2.x * dir.y + shadeN->x * dir.z;\n"
"        wi->y = v1.y * dir.x + v2.y * dir.y + shadeN->y * dir.z;\n"
"        wi->z = v1.z * dir.x + v2.z * dir.y + shadeN->z * dir.z;\n"
"\n"
"        const float dp = Dot(shadeN, wi);\n"
"        // Using 0.0001 instead of 0.0 to cut down fireflies\n"
"        if (dp <= 0.0001f)\n"
"            *pdf = 0.f;\n"
"        else {\n"
"            *pdf /=  dp;\n"
"\n"
"            const float iRe = 1.f - Re;\n"
"            *pdf *= (1.f - P) / iRe;\n"
"\n"
"            f->r = iRe * mat->diff_r;\n"
"            f->g = iRe * mat->diff_g;\n"
"            f->b = iRe * mat->diff_b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = FALSE;\n"
"#endif\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"void ArchGlass_Sample_f(__global ArchGlassParam *mat,\n"
"    const Vector *wo, Vector *wi, float *pdf, Spectrum *f, const Vector *N, const Vector *shadeN,\n"
"    const float u0\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"		, __global int *specularBounce\n"
"#endif\n"
"        ) {\n"
"    // Ray from outside going in ?\n"
"    const bool into = (Dot(N, shadeN) > 0.f);\n"
"\n"
"    if (!into) {\n"
"        // No internal reflections\n"
"        wi->x = -wo->x;\n"
"        wi->y = -wo->y;\n"
"        wi->z = -wo->z;\n"
"        *pdf = 1.f;\n"
"\n"
"        f->r = mat->refrct_r;\n"
"        f->g = mat->refrct_g;\n"
"        f->b = mat->refrct_b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"        *specularBounce = mat->transmitionSpecularBounce;\n"
"#endif\n"
"    } else {\n"
"        // RR to choose if reflect the ray or go trough the glass\n"
"        const float comp = u0 * mat->totFilter;\n"
"\n"
"        if (comp > mat->transFilter) {\n"
"            const float k = 2.f * Dot(N, wo);\n"
"            wi->x = k * N->x - wo->x;\n"
"            wi->y = k * N->y - wo->y;\n"
"            wi->z = k * N->z - wo->z;\n"
"            *pdf =  mat->reflPdf;\n"
"\n"
"            f->r = mat->refl_r;\n"
"            f->g = mat->refl_g;\n"
"            f->b = mat->refl_b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = mat->reflectionSpecularBounce;\n"
"#endif\n"
"        } else {\n"
"            wi->x = -wo->x;\n"
"            wi->y = -wo->y;\n"
"            wi->z = -wo->z;\n"
"            *pdf =  mat->transPdf;\n"
"\n"
"            f->r = mat->refrct_r;\n"
"            f->g = mat->refrct_g;\n"
"            f->b = mat->refrct_b;\n"
"\n"
"#if defined(PARAM_DIRECT_LIGHT_SAMPLING)\n"
"            *specularBounce = mat->transmitionSpecularBounce;\n"
"#endif\n"
"        }\n"
"    }\n"
"}\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// Lights\n"
"//------------------------------------------------------------------------------\n"
"\n"
"void AreaLight_Le(__global AreaLightParam *mat, const Vector *wo, const Vector *lightN, Spectrum *Le) {\n"
"	const bool brightSide = (Dot(lightN, wo) > 0.f);\n"
"\n"
"	Le->r = brightSide ? mat->gain_r : 0.f;\n"
"	Le->g = brightSide ? mat->gain_g : 0.f;\n"
"	Le->b = brightSide ? mat->gain_b : 0.f;\n"
"}\n"
"\n"
"void SampleTriangleLight(__global TriangleLight *light,	const float u0, const float u1, Point *p) {\n"
"	Point v0, v1, v2;\n"
"	v0 = light->v0;\n"
"	v1 = light->v1;\n"
"	v2 = light->v2;\n"
"\n"
"	// UniformSampleTriangle(u0, u1, b0, b1);\n"
"	const float su1 = sqrt(u0);\n"
"	const float b0 = 1.f - su1;\n"
"	const float b1 = u1 * su1;\n"
"	const float b2 = 1.f - b0 - b1;\n"
"\n"
"	p->x = b0 * v0.x + b1 * v1.x + b2 * v2.x;\n"
"	p->y = b0 * v0.y + b1 * v1.y + b2 * v2.y;\n"
"	p->z = b0 * v0.z + b1 * v1.z + b2 * v2.z;\n"
"}\n"
"\n"
"void TriangleLight_Sample_L(__global TriangleLight *l,\n"
"		const Point *hitPoint,\n"
"		float *pdf, Spectrum *f, Ray *shadowRay,\n"
"		const float u0, const float u1) {\n"
"	Point samplePoint;\n"
"	SampleTriangleLight(l, u0, u1, &samplePoint);\n"
"\n"
"	shadowRay->d.x = samplePoint.x - hitPoint->x;\n"
"	shadowRay->d.y = samplePoint.y - hitPoint->y;\n"
"	shadowRay->d.z = samplePoint.z - hitPoint->z;\n"
"	const float distanceSquared = Dot(&shadowRay->d, &shadowRay->d);\n"
"	const float distance = sqrt(distanceSquared);\n"
"	const float invDistance = 1.f / distance;\n"
"	shadowRay->d.x *= invDistance;\n"
"	shadowRay->d.y *= invDistance;\n"
"	shadowRay->d.z *= invDistance;\n"
"\n"
"	Vector sampleN = l->normal;\n"
"	const float sampleNdotMinusWi = -Dot(&sampleN, &shadowRay->d);\n"
"	if (sampleNdotMinusWi <= 0.f)\n"
"		*pdf = 0.f;\n"
"	else {\n"
"		*pdf = distanceSquared / (sampleNdotMinusWi * l->area);\n"
"\n"
"		// Using 0.1 instead of 0.0 to cut down fireflies\n"
"		if (*pdf <= 0.1f)\n"
"			*pdf = 0.f;\n"
"		else {\n"
"            shadowRay->o = *hitPoint;\n"
"            shadowRay->mint = PARAM_RAY_EPSILON;\n"
"            shadowRay->maxt = distance - PARAM_RAY_EPSILON;\n"
"\n"
"            f->r = l->gain_r;\n"
"            f->g = l->gain_g;\n"
"            f->b = l->gain_b;\n"
"        }\n"
"	}\n"
"}\n"
"\n"
"//------------------------------------------------------------------------------\n"
"// GenerateCameraRay\n"
"//------------------------------------------------------------------------------\n"
"\n"
"void GenerateCameraRay(\n"
"		__global Sample *sample,\n"
"		__global Ray *ray\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"		, Seed *seed\n"
"#endif\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"		, __global float *cameraData\n"
"#endif\n"
"		) {\n"
"#if (PARAM_SAMPLER_TYPE == 0) || (PARAM_SAMPLER_TYPE == 1) || (PARAM_SAMPLER_TYPE == 3)\n"
"	__global float *sampleData = &sample->u[IDX_SCREEN_X];\n"
"	const uint pixelIndex = sample->pixelIndex;\n"
"\n"
"	const float scrSampleX = sampleData[IDX_SCREEN_X];\n"
"	const float scrSampleY = sampleData[IDX_SCREEN_Y];\n"
"\n"
"	const float screenX = pixelIndex % PARAM_IMAGE_WIDTH + scrSampleX - 0.5f;\n"
"	const float screenY = pixelIndex / PARAM_IMAGE_WIDTH + scrSampleY - 0.5f;\n"
"#elif (PARAM_SAMPLER_TYPE == 2)\n"
"	__global float *sampleData = &sample->u[sample->proposed][IDX_SCREEN_X];\n"
"	const float screenX = min((uint)(sampleData[IDX_SCREEN_X] * PARAM_IMAGE_WIDTH), (uint)(PARAM_IMAGE_WIDTH - 1));\n"
"	const float screenY = min((uint)(sampleData[IDX_SCREEN_Y] * PARAM_IMAGE_HEIGHT), (uint)(PARAM_IMAGE_HEIGHT - 1));\n"
"#endif\n"
"\n"
"	Point Pras;\n"
"	Pras.x = screenX;\n"
"	Pras.y = PARAM_IMAGE_HEIGHT - screenY - 1.f;\n"
"	Pras.z = 0;\n"
"\n"
"	Point orig;\n"
"	// RasterToCamera(Pras, &orig);\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"	const float iw = 1.f / (cameraData[12] * Pras.x + cameraData[13] * Pras.y + cameraData[14] * Pras.z + cameraData[15]);\n"
"	orig.x = (cameraData[0] * Pras.x + cameraData[1] * Pras.y + cameraData[2] * Pras.z + cameraData[3]) * iw;\n"
"	orig.y = (cameraData[4] * Pras.x + cameraData[5] * Pras.y + cameraData[6] * Pras.z + cameraData[7]) * iw;\n"
"	orig.z = (cameraData[8] * Pras.x + cameraData[9] * Pras.y + cameraData[10] * Pras.z + cameraData[11]) * iw;\n"
"#else\n"
"	const float iw = 1.f / (PARAM_RASTER2CAMERA_30 * Pras.x + PARAM_RASTER2CAMERA_31 * Pras.y + PARAM_RASTER2CAMERA_32 * Pras.z + PARAM_RASTER2CAMERA_33);\n"
"	orig.x = (PARAM_RASTER2CAMERA_00 * Pras.x + PARAM_RASTER2CAMERA_01 * Pras.y + PARAM_RASTER2CAMERA_02 * Pras.z + PARAM_RASTER2CAMERA_03) * iw;\n"
"	orig.y = (PARAM_RASTER2CAMERA_10 * Pras.x + PARAM_RASTER2CAMERA_11 * Pras.y + PARAM_RASTER2CAMERA_12 * Pras.z + PARAM_RASTER2CAMERA_13) * iw;\n"
"	orig.z = (PARAM_RASTER2CAMERA_20 * Pras.x + PARAM_RASTER2CAMERA_21 * Pras.y + PARAM_RASTER2CAMERA_22 * Pras.z + PARAM_RASTER2CAMERA_23) * iw;\n"
"#endif\n"
"\n"
"	Vector dir;\n"
"	dir.x = orig.x;\n"
"	dir.y = orig.y;\n"
"	dir.z = orig.z;\n"
"\n"
"#if defined(PARAM_CAMERA_HAS_DOF)\n"
"\n"
"#if (PARAM_SAMPLER_TYPE == 0)\n"
"	const float dofSampleX = RndFloatValue(seed);\n"
"	const float dofSampleY = RndFloatValue(seed);\n"
"#elif (PARAM_SAMPLER_TYPE == 1) || (PARAM_SAMPLER_TYPE == 2) || (PARAM_SAMPLER_TYPE == 3)\n"
"	const float dofSampleX = sampleData[IDX_DOF_X];\n"
"	const float dofSampleY = sampleData[IDX_DOF_Y];\n"
"#endif\n"
"\n"
"	// Sample point on lens\n"
"	float lensU, lensV;\n"
"	ConcentricSampleDisk(dofSampleX, dofSampleY, &lensU, &lensV);\n"
"	lensU *= PARAM_CAMERA_LENS_RADIUS;\n"
"	lensV *= PARAM_CAMERA_LENS_RADIUS;\n"
"\n"
"	// Compute point on plane of focus\n"
"	const float ft = (PARAM_CAMERA_FOCAL_DISTANCE - PARAM_CLIP_HITHER) / dir.z;\n"
"	Point Pfocus;\n"
"	Pfocus.x = orig.x + dir.x * ft;\n"
"	Pfocus.y = orig.y + dir.y * ft;\n"
"	Pfocus.z = orig.z + dir.z * ft;\n"
"\n"
"	// Update ray for effect of lens\n"
"	orig.x += lensU * ((PARAM_CAMERA_FOCAL_DISTANCE - PARAM_CLIP_HITHER) / PARAM_CAMERA_FOCAL_DISTANCE);\n"
"	orig.y += lensV * ((PARAM_CAMERA_FOCAL_DISTANCE - PARAM_CLIP_HITHER) / PARAM_CAMERA_FOCAL_DISTANCE);\n"
"\n"
"	dir.x = Pfocus.x - orig.x;\n"
"	dir.y = Pfocus.y - orig.y;\n"
"	dir.z = Pfocus.z - orig.z;\n"
"#endif\n"
"\n"
"	Normalize(&dir);\n"
"\n"
"	// CameraToWorld(*ray, ray);\n"
"	Point torig;\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"	const float iw2 = 1.f / (cameraData[16 + 12] * orig.x + cameraData[16 + 13] * orig.y + cameraData[16 + 14] * orig.z + cameraData[16 + 15]);\n"
"	torig.x = (cameraData[16 + 0] * orig.x + cameraData[16 + 1] * orig.y + cameraData[16 + 2] * orig.z + cameraData[16 + 3]) * iw2;\n"
"	torig.y = (cameraData[16 + 4] * orig.x + cameraData[16 + 5] * orig.y + cameraData[16 + 6] * orig.z + cameraData[16 + 7]) * iw2;\n"
"	torig.z = (cameraData[16 + 8] * orig.x + cameraData[16 + 9] * orig.y + cameraData[16 + 12] * orig.z + cameraData[16 + 11]) * iw2;\n"
"#else\n"
"	const float iw2 = 1.f / (PARAM_CAMERA2WORLD_30 * orig.x + PARAM_CAMERA2WORLD_31 * orig.y + PARAM_CAMERA2WORLD_32 * orig.z + PARAM_CAMERA2WORLD_33);\n"
"	torig.x = (PARAM_CAMERA2WORLD_00 * orig.x + PARAM_CAMERA2WORLD_01 * orig.y + PARAM_CAMERA2WORLD_02 * orig.z + PARAM_CAMERA2WORLD_03) * iw2;\n"
"	torig.y = (PARAM_CAMERA2WORLD_10 * orig.x + PARAM_CAMERA2WORLD_11 * orig.y + PARAM_CAMERA2WORLD_12 * orig.z + PARAM_CAMERA2WORLD_13) * iw2;\n"
"	torig.z = (PARAM_CAMERA2WORLD_20 * orig.x + PARAM_CAMERA2WORLD_21 * orig.y + PARAM_CAMERA2WORLD_22 * orig.z + PARAM_CAMERA2WORLD_23) * iw2;\n"
"#endif\n"
"\n"
"	Vector tdir;\n"
"#if defined(PARAM_CAMERA_DYNAMIC)\n"
"	tdir.x = cameraData[16 + 0] * dir.x + cameraData[16 + 1] * dir.y + cameraData[16 + 2] * dir.z;\n"
"	tdir.y = cameraData[16 + 4] * dir.x + cameraData[16 + 5] * dir.y + cameraData[16 + 6] * dir.z;\n"
"	tdir.z = cameraData[16 + 8] * dir.x + cameraData[16 + 9] * dir.y + cameraData[16 + 10] * dir.z;\n"
"#else\n"
"	tdir.x = PARAM_CAMERA2WORLD_00 * dir.x + PARAM_CAMERA2WORLD_01 * dir.y + PARAM_CAMERA2WORLD_02 * dir.z;\n"
"	tdir.y = PARAM_CAMERA2WORLD_10 * dir.x + PARAM_CAMERA2WORLD_11 * dir.y + PARAM_CAMERA2WORLD_12 * dir.z;\n"
"	tdir.z = PARAM_CAMERA2WORLD_20 * dir.x + PARAM_CAMERA2WORLD_21 * dir.y + PARAM_CAMERA2WORLD_22 * dir.z;\n"
"#endif\n"
"\n"
"	ray->o = torig;\n"
"	ray->d = tdir;\n"
"	ray->mint = PARAM_RAY_EPSILON;\n"
"	ray->maxt = (PARAM_CLIP_YON - PARAM_CLIP_HITHER) / dir.z;\n"
"\n"
"	/*printf(\"(%f, %f, %f) (%f, %f, %f) [%f, %f]\\n\",\n"
"		ray->o.x, ray->o.y, ray->o.z, ray->d.x, ray->d.y, ray->d.z,\n"
"		ray->mint, ray->maxt);*/\n"
"}\n"
;
