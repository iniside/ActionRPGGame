// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "WorldArchitectEdModeToolkit.h"
#include "WorldArchitectEdMode.h"
#include "Engine/Selection.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/STextBlock.h"
#include "EditorModeManager.h"
#include "Landscape.h"
#include "LandscapeInfo.h"
#include "EngineUtils.h"
#include "LandscapeEditorUtils.h"
#include "LandscapeInfoMap.h"
#include "LandscapeStreamingProxy.h"
#include "PropertyCustomizationHelpers.h"
#include "Runtime/WALandscapeGraph.h"
#include "WALandscapeGraphFactory.h"
#include "LandscapeGraphEditor/WALandscapeGraphSchema.h"
#include "LandscapeGraphEditor/WALandscapeGraphEdNode_Output.h"

// Noise1234
// Author: Stefan Gustavson (stegu@itn.liu.se)
//
// This library is public domain software, released by the author
// into the public domain in February 2011. You may do anything
// you like with it. You may even remove all attributions,
// but of course I'd appreciate it if you kept my name somewhere.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.

/** \file
\brief Implements the Noise1234 class for producing Perlin noise.
\author Stefan Gustavson (stegu@itn.liu.se)
*/

/*
* This implementation is "Improved Noise" as presented by
* Ken Perlin at Siggraph 2002. The 3D function is a direct port
* of his Java reference code available on www.noisemachine.com
* (although I cleaned it up and made the code more readable),
* but the 1D, 2D and 4D cases were implemented from scratch
* by me.
*
* This is a highly reusable class. It has no dependencies
* on any other file, apart from its own header file.
*/


// This is the new and improved, C(2) continuous interpolant
#define FADE(t) ( t * t * t * ( t * ( t * 6 - 15 ) + 10 ) )

#define FASTFLOOR(x) ( ((x)>0) ? ((int)x) : ((int)x-1 ) )
#define LERP(t, a, b) ((a) + (t)*((b)-(a)))


//---------------------------------------------------------------------
// Static data

/*
* Permutation table. This is just a random jumble of all numbers 0-255,
* repeated twice to avoid wrapping the index at 255 for each lookup.
* This needs to be exactly the same for all instances on all platforms,
* so it's easiest to just keep it as static explicit data.
* This also removes the need for any initialisation of this class.
*
* Note that making this an int[] instead of a char[] might make the
* code run faster on platforms with a high penalty for unaligned single
* byte addressing. Intel x86 is generally single-byte-friendly, but
* some other CPUs are faster with 4-aligned reads.
* However, a char[] is smaller, which avoids cache trashing, and that
* is probably the most important aspect on most architectures.
* This array is accessed a *lot* by the noise functions.
* A vector-valued noise over 3D accesses it 96 times, and a
* float-valued 4D noise 64 times. We want this to fit in the cache!
*/
unsigned char Noise1234::perm[] = { 151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
151,160,137,91,90,15,
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

//---------------------------------------------------------------------

/*
* Helper functions to compute gradients-dot-residualvectors (1D to 4D)
* Note that these generate gradients of more than unit length. To make
* a close match with the value range of classic Perlin noise, the final
* noise values need to be rescaled. To match the RenderMan noise in a
* statistical sense, the approximate scaling values (empirically
* determined from test renderings) are:
* 1D noise needs rescaling with 0.188
* 2D noise needs rescaling with 0.507
* 3D noise needs rescaling with 0.936
* 4D noise needs rescaling with 0.87
* Note that these noise functions are the most practical and useful
* signed version of Perlin noise. To return values according to the
* RenderMan specification from the SL noise() and pnoise() functions,
* the noise values need to be scaled and offset to [0,1], like this:
* float SLnoise = (Noise1234::noise(x,y,z) + 1.0) * 0.5;
*/

float  Noise1234::grad(int hash, float x) {
	int h = hash & 15;
	float grad = 1.0 + (h & 7);  // Gradient value 1.0, 2.0, ..., 8.0
	if (h & 8) grad = -grad;         // and a random sign for the gradient
	return (grad * x);           // Multiply the gradient with the distance
}

float  Noise1234::grad(int hash, float x, float y) {
	int h = hash & 7;      // Convert low 3 bits of hash code
	float u = h<4 ? x : y;  // into 8 simple gradient directions,
	float v = h<4 ? y : x;  // and compute the dot product with (x,y).
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0*v : 2.0*v);
}

float  Noise1234::grad(int hash, float x, float y, float z) {
	int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
	float u = h<8 ? x : y; // gradient directions, and compute dot product.
	float v = h<4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

float  Noise1234::grad(int hash, float x, float y, float z, float t) {
	int h = hash & 31;      // Convert low 5 bits of hash code into 32 simple
	float u = h<24 ? x : y; // gradient directions, and compute dot product.
	float v = h<16 ? y : z;
	float w = h<8 ? z : t;
	return ((h & 1) ? -u : u) + ((h & 2) ? -v : v) + ((h & 4) ? -w : w);
}

//---------------------------------------------------------------------
/** 1D float Perlin noise, SL "noise()"
*/
float Noise1234::noise(float x)
{
	int ix0, ix1;
	float fx0, fx1;
	float s, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	fx0 = x - ix0;       // Fractional part of x
	fx1 = fx0 - 1.0f;
	ix1 = (ix0 + 1) & 0xff;
	ix0 = ix0 & 0xff;    // Wrap to 0..255

	s = FADE(fx0);

	n0 = grad(perm[ix0], fx0);
	n1 = grad(perm[ix1], fx1);
	return 0.188f * (LERP(s, n0, n1));
}

//---------------------------------------------------------------------
/** 1D float Perlin periodic noise, SL "pnoise()"
*/
float Noise1234::pnoise(float x, int px)
{
	int ix0, ix1;
	float fx0, fx1;
	float s, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	fx0 = x - ix0;       // Fractional part of x
	fx1 = fx0 - 1.0f;
	ix1 = ((ix0 + 1) % px) & 0xff; // Wrap to 0..px-1 *and* wrap to 0..255
	ix0 = (ix0 % px) & 0xff;      // (because px might be greater than 256)

	s = FADE(fx0);

	n0 = grad(perm[ix0], fx0);
	n1 = grad(perm[ix1], fx1);
	return 0.188f * (LERP(s, n0, n1));
}


//---------------------------------------------------------------------
/** 2D float Perlin noise.
*/
float Noise1234::noise(float x, float y)
{
	int ix0, iy0, ix1, iy1;
	float fx0, fy0, fx1, fy1;
	float s, t, nx0, nx1, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	iy0 = FASTFLOOR(y); // Integer part of y
	fx0 = x - ix0;        // Fractional part of x
	fy0 = y - iy0;        // Fractional part of y
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	ix1 = (ix0 + 1) & 0xff;  // Wrap to 0..255
	iy1 = (iy0 + 1) & 0xff;
	ix0 = ix0 & 0xff;
	iy0 = iy0 & 0xff;

	t = FADE(fy0);
	s = FADE(fx0);

	nx0 = grad(perm[ix0 + perm[iy0]], fx0, fy0);
	nx1 = grad(perm[ix0 + perm[iy1]], fx0, fy1);
	n0 = LERP(t, nx0, nx1);

	nx0 = grad(perm[ix1 + perm[iy0]], fx1, fy0);
	nx1 = grad(perm[ix1 + perm[iy1]], fx1, fy1);
	n1 = LERP(t, nx0, nx1);

	return 0.507f * (LERP(s, n0, n1));
}

//---------------------------------------------------------------------
/** 2D float Perlin periodic noise.
*/
float Noise1234::pnoise(float x, float y, int px, int py)
{
	int ix0, iy0, ix1, iy1;
	float fx0, fy0, fx1, fy1;
	float s, t, nx0, nx1, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	iy0 = FASTFLOOR(y); // Integer part of y
	fx0 = x - ix0;        // Fractional part of x
	fy0 = y - iy0;        // Fractional part of y
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	ix1 = ((ix0 + 1) % px) & 0xff;  // Wrap to 0..px-1 and wrap to 0..255
	iy1 = ((iy0 + 1) % py) & 0xff;  // Wrap to 0..py-1 and wrap to 0..255
	ix0 = (ix0 % px) & 0xff;
	iy0 = (iy0 % py) & 0xff;

	t = FADE(fy0);
	s = FADE(fx0);

	nx0 = grad(perm[ix0 + perm[iy0]], fx0, fy0);
	nx1 = grad(perm[ix0 + perm[iy1]], fx0, fy1);
	n0 = LERP(t, nx0, nx1);

	nx0 = grad(perm[ix1 + perm[iy0]], fx1, fy0);
	nx1 = grad(perm[ix1 + perm[iy1]], fx1, fy1);
	n1 = LERP(t, nx0, nx1);

	return 0.507f * (LERP(s, n0, n1));
}


//---------------------------------------------------------------------
/** 3D float Perlin noise.
*/
float Noise1234::noise(float x, float y, float z)
{
	int ix0, iy0, ix1, iy1, iz0, iz1;
	float fx0, fy0, fz0, fx1, fy1, fz1;
	float s, t, r;
	float nxy0, nxy1, nx0, nx1, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	iy0 = FASTFLOOR(y); // Integer part of y
	iz0 = FASTFLOOR(z); // Integer part of z
	fx0 = x - ix0;        // Fractional part of x
	fy0 = y - iy0;        // Fractional part of y
	fz0 = z - iz0;        // Fractional part of z
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	fz1 = fz0 - 1.0f;
	ix1 = (ix0 + 1) & 0xff; // Wrap to 0..255
	iy1 = (iy0 + 1) & 0xff;
	iz1 = (iz0 + 1) & 0xff;
	ix0 = ix0 & 0xff;
	iy0 = iy0 & 0xff;
	iz0 = iz0 & 0xff;

	r = FADE(fz0);
	t = FADE(fy0);
	s = FADE(fx0);

	nxy0 = grad(perm[ix0 + perm[iy0 + perm[iz0]]], fx0, fy0, fz0);
	nxy1 = grad(perm[ix0 + perm[iy0 + perm[iz1]]], fx0, fy0, fz1);
	nx0 = LERP(r, nxy0, nxy1);

	nxy0 = grad(perm[ix0 + perm[iy1 + perm[iz0]]], fx0, fy1, fz0);
	nxy1 = grad(perm[ix0 + perm[iy1 + perm[iz1]]], fx0, fy1, fz1);
	nx1 = LERP(r, nxy0, nxy1);

	n0 = LERP(t, nx0, nx1);

	nxy0 = grad(perm[ix1 + perm[iy0 + perm[iz0]]], fx1, fy0, fz0);
	nxy1 = grad(perm[ix1 + perm[iy0 + perm[iz1]]], fx1, fy0, fz1);
	nx0 = LERP(r, nxy0, nxy1);

	nxy0 = grad(perm[ix1 + perm[iy1 + perm[iz0]]], fx1, fy1, fz0);
	nxy1 = grad(perm[ix1 + perm[iy1 + perm[iz1]]], fx1, fy1, fz1);
	nx1 = LERP(r, nxy0, nxy1);

	n1 = LERP(t, nx0, nx1);

	return 0.936f * (LERP(s, n0, n1));
}

//---------------------------------------------------------------------
/** 3D float Perlin periodic noise.
*/
float Noise1234::pnoise(float x, float y, float z, int px, int py, int pz)
{
	int ix0, iy0, ix1, iy1, iz0, iz1;
	float fx0, fy0, fz0, fx1, fy1, fz1;
	float s, t, r;
	float nxy0, nxy1, nx0, nx1, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	iy0 = FASTFLOOR(y); // Integer part of y
	iz0 = FASTFLOOR(z); // Integer part of z
	fx0 = x - ix0;        // Fractional part of x
	fy0 = y - iy0;        // Fractional part of y
	fz0 = z - iz0;        // Fractional part of z
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	fz1 = fz0 - 1.0f;
	ix1 = ((ix0 + 1) % px) & 0xff; // Wrap to 0..px-1 and wrap to 0..255
	iy1 = ((iy0 + 1) % py) & 0xff; // Wrap to 0..py-1 and wrap to 0..255
	iz1 = ((iz0 + 1) % pz) & 0xff; // Wrap to 0..pz-1 and wrap to 0..255
	ix0 = (ix0 % px) & 0xff;
	iy0 = (iy0 % py) & 0xff;
	iz0 = (iz0 % pz) & 0xff;

	r = FADE(fz0);
	t = FADE(fy0);
	s = FADE(fx0);

	nxy0 = grad(perm[ix0 + perm[iy0 + perm[iz0]]], fx0, fy0, fz0);
	nxy1 = grad(perm[ix0 + perm[iy0 + perm[iz1]]], fx0, fy0, fz1);
	nx0 = LERP(r, nxy0, nxy1);

	nxy0 = grad(perm[ix0 + perm[iy1 + perm[iz0]]], fx0, fy1, fz0);
	nxy1 = grad(perm[ix0 + perm[iy1 + perm[iz1]]], fx0, fy1, fz1);
	nx1 = LERP(r, nxy0, nxy1);

	n0 = LERP(t, nx0, nx1);

	nxy0 = grad(perm[ix1 + perm[iy0 + perm[iz0]]], fx1, fy0, fz0);
	nxy1 = grad(perm[ix1 + perm[iy0 + perm[iz1]]], fx1, fy0, fz1);
	nx0 = LERP(r, nxy0, nxy1);

	nxy0 = grad(perm[ix1 + perm[iy1 + perm[iz0]]], fx1, fy1, fz0);
	nxy1 = grad(perm[ix1 + perm[iy1 + perm[iz1]]], fx1, fy1, fz1);
	nx1 = LERP(r, nxy0, nxy1);

	n1 = LERP(t, nx0, nx1);

	return 0.936f * (LERP(s, n0, n1));
}


//---------------------------------------------------------------------
/** 4D float Perlin noise.
*/

float Noise1234::noise(float x, float y, float z, float w)
{
	int ix0, iy0, iz0, iw0, ix1, iy1, iz1, iw1;
	float fx0, fy0, fz0, fw0, fx1, fy1, fz1, fw1;
	float s, t, r, q;
	float nxyz0, nxyz1, nxy0, nxy1, nx0, nx1, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	iy0 = FASTFLOOR(y); // Integer part of y
	iz0 = FASTFLOOR(z); // Integer part of y
	iw0 = FASTFLOOR(w); // Integer part of w
	fx0 = x - ix0;        // Fractional part of x
	fy0 = y - iy0;        // Fractional part of y
	fz0 = z - iz0;        // Fractional part of z
	fw0 = w - iw0;        // Fractional part of w
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	fz1 = fz0 - 1.0f;
	fw1 = fw0 - 1.0f;
	ix1 = (ix0 + 1) & 0xff;  // Wrap to 0..255
	iy1 = (iy0 + 1) & 0xff;
	iz1 = (iz0 + 1) & 0xff;
	iw1 = (iw0 + 1) & 0xff;
	ix0 = ix0 & 0xff;
	iy0 = iy0 & 0xff;
	iz0 = iz0 & 0xff;
	iw0 = iw0 & 0xff;

	q = FADE(fw0);
	r = FADE(fz0);
	t = FADE(fy0);
	s = FADE(fx0);

	nxyz0 = grad(perm[ix0 + perm[iy0 + perm[iz0 + perm[iw0]]]], fx0, fy0, fz0, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy0 + perm[iz0 + perm[iw1]]]], fx0, fy0, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix0 + perm[iy0 + perm[iz1 + perm[iw0]]]], fx0, fy0, fz1, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy0 + perm[iz1 + perm[iw1]]]], fx0, fy0, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx0 = LERP(r, nxy0, nxy1);

	nxyz0 = grad(perm[ix0 + perm[iy1 + perm[iz0 + perm[iw0]]]], fx0, fy1, fz0, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy1 + perm[iz0 + perm[iw1]]]], fx0, fy1, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix0 + perm[iy1 + perm[iz1 + perm[iw0]]]], fx0, fy1, fz1, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy1 + perm[iz1 + perm[iw1]]]], fx0, fy1, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx1 = LERP(r, nxy0, nxy1);

	n0 = LERP(t, nx0, nx1);

	nxyz0 = grad(perm[ix1 + perm[iy0 + perm[iz0 + perm[iw0]]]], fx1, fy0, fz0, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy0 + perm[iz0 + perm[iw1]]]], fx1, fy0, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix1 + perm[iy0 + perm[iz1 + perm[iw0]]]], fx1, fy0, fz1, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy0 + perm[iz1 + perm[iw1]]]], fx1, fy0, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx0 = LERP(r, nxy0, nxy1);

	nxyz0 = grad(perm[ix1 + perm[iy1 + perm[iz0 + perm[iw0]]]], fx1, fy1, fz0, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy1 + perm[iz0 + perm[iw1]]]], fx1, fy1, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix1 + perm[iy1 + perm[iz1 + perm[iw0]]]], fx1, fy1, fz1, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy1 + perm[iz1 + perm[iw1]]]], fx1, fy1, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx1 = LERP(r, nxy0, nxy1);

	n1 = LERP(t, nx0, nx1);

	return 0.87f * (LERP(s, n0, n1));
}

//---------------------------------------------------------------------
/** 4D float Perlin periodic noise.
*/

float Noise1234::pnoise(float x, float y, float z, float w,
	int px, int py, int pz, int pw)
{
	int ix0, iy0, iz0, iw0, ix1, iy1, iz1, iw1;
	float fx0, fy0, fz0, fw0, fx1, fy1, fz1, fw1;
	float s, t, r, q;
	float nxyz0, nxyz1, nxy0, nxy1, nx0, nx1, n0, n1;

	ix0 = FASTFLOOR(x); // Integer part of x
	iy0 = FASTFLOOR(y); // Integer part of y
	iz0 = FASTFLOOR(z); // Integer part of y
	iw0 = FASTFLOOR(w); // Integer part of w
	fx0 = x - ix0;        // Fractional part of x
	fy0 = y - iy0;        // Fractional part of y
	fz0 = z - iz0;        // Fractional part of z
	fw0 = w - iw0;        // Fractional part of w
	fx1 = fx0 - 1.0f;
	fy1 = fy0 - 1.0f;
	fz1 = fz0 - 1.0f;
	fw1 = fw0 - 1.0f;
	ix1 = ((ix0 + 1) % px) & 0xff;  // Wrap to 0..px-1 and wrap to 0..255
	iy1 = ((iy0 + 1) % py) & 0xff;  // Wrap to 0..py-1 and wrap to 0..255
	iz1 = ((iz0 + 1) % pz) & 0xff;  // Wrap to 0..pz-1 and wrap to 0..255
	iw1 = ((iw0 + 1) % pw) & 0xff;  // Wrap to 0..pw-1 and wrap to 0..255
	ix0 = (ix0 % px) & 0xff;
	iy0 = (iy0 % py) & 0xff;
	iz0 = (iz0 % pz) & 0xff;
	iw0 = (iw0 % pw) & 0xff;

	q = FADE(fw0);
	r = FADE(fz0);
	t = FADE(fy0);
	s = FADE(fx0);

	nxyz0 = grad(perm[ix0 + perm[iy0 + perm[iz0 + perm[iw0]]]], fx0, fy0, fz0, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy0 + perm[iz0 + perm[iw1]]]], fx0, fy0, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix0 + perm[iy0 + perm[iz1 + perm[iw0]]]], fx0, fy0, fz1, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy0 + perm[iz1 + perm[iw1]]]], fx0, fy0, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx0 = LERP(r, nxy0, nxy1);

	nxyz0 = grad(perm[ix0 + perm[iy1 + perm[iz0 + perm[iw0]]]], fx0, fy1, fz0, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy1 + perm[iz0 + perm[iw1]]]], fx0, fy1, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix0 + perm[iy1 + perm[iz1 + perm[iw0]]]], fx0, fy1, fz1, fw0);
	nxyz1 = grad(perm[ix0 + perm[iy1 + perm[iz1 + perm[iw1]]]], fx0, fy1, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx1 = LERP(r, nxy0, nxy1);

	n0 = LERP(t, nx0, nx1);

	nxyz0 = grad(perm[ix1 + perm[iy0 + perm[iz0 + perm[iw0]]]], fx1, fy0, fz0, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy0 + perm[iz0 + perm[iw1]]]], fx1, fy0, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix1 + perm[iy0 + perm[iz1 + perm[iw0]]]], fx1, fy0, fz1, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy0 + perm[iz1 + perm[iw1]]]], fx1, fy0, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx0 = LERP(r, nxy0, nxy1);

	nxyz0 = grad(perm[ix1 + perm[iy1 + perm[iz0 + perm[iw0]]]], fx1, fy1, fz0, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy1 + perm[iz0 + perm[iw1]]]], fx1, fy1, fz0, fw1);
	nxy0 = LERP(q, nxyz0, nxyz1);

	nxyz0 = grad(perm[ix1 + perm[iy1 + perm[iz1 + perm[iw0]]]], fx1, fy1, fz1, fw0);
	nxyz1 = grad(perm[ix1 + perm[iy1 + perm[iz1 + perm[iw1]]]], fx1, fy1, fz1, fw1);
	nxy1 = LERP(q, nxyz0, nxyz1);

	nx1 = LERP(r, nxy0, nxy1);

	n1 = LERP(t, nx0, nx1);

	return 0.87f * (LERP(s, n0, n1));
}

//---------------------------------------------------------------------

#define LOCTEXT_NAMESPACE "FWorldArchitectEdModeToolkit"

FWorldArchitectEdModeToolkit::FWorldArchitectEdModeToolkit()
{
}

void FWorldArchitectEdModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost)
{
	struct Locals
	{
		static bool IsWidgetEnabled()
		{
			return GEditor->GetSelectedActors()->Num() != 0;
		}

		static FReply OnButtonClick(FVector InOffset)
		{
			USelection* SelectedActors = GEditor->GetSelectedActors();

			// Let editor know that we're about to do something that we want to undo/redo
			GEditor->BeginTransaction(LOCTEXT("MoveActorsTransactionName", "MoveActors"));

			// For each selected actor
			for (FSelectionIterator Iter(*SelectedActors); Iter; ++Iter)
			{
				if (AActor* LevelActor = Cast<AActor>(*Iter))
				{
					// Register actor in opened transaction (undo/redo)
					LevelActor->Modify();
					// Move actor to given location
					LevelActor->TeleportTo(LevelActor->GetActorLocation() + InOffset, FRotator(0, 0, 0));
				}
			}

			// We're done moving actors so close transaction
			GEditor->EndTransaction();

			return FReply::Handled();
		}

		static TSharedRef<SWidget> MakeButton(FText InLabel, const FVector InOffset)
		{
			return SNew(SButton)
				.Text(InLabel)
				.OnClicked_Static(&Locals::OnButtonClick, InOffset);
		}
	};

	const float Factor = 256.0f;
	FAssetData DefaultAsset;
	FOnGetAllowedClasses OnGetAllowedClassesDel = FOnGetAllowedClasses::CreateSP(this, &FWorldArchitectEdModeToolkit::OnGetAllowedClasses);
	FOnAssetSelected OnAssetSelectedFromPickerDel = FOnAssetSelected::CreateSP(this, &FWorldArchitectEdModeToolkit::OnAssetSelected);
	FOnShouldFilterAsset OnShouldFilterAssetDel = FOnShouldFilterAsset::CreateSP(this, &FWorldArchitectEdModeToolkit::OnShouldFilterAsset);
	FSimpleDelegate OnCloseDel;
	TArray<const UClass*> AllowedClasses;
	AllowedClasses.Add(UWALandscapeGraph::StaticClass());
	TArray<UFactory*> NewAssetFactory;
	NewAssetFactory.Add(UWALandscapeGraphFactory::StaticClass()->GetDefaultObject<UFactory>());

	SAssignNew(ToolkitWidget, SBorder)
		.HAlign(HAlign_Center)
		.Padding(25)
		//.IsEnabled_Static(&Locals::IsWidgetEnabled)
		[
			SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Center)
			.Padding(50)
			[
				SNew(STextBlock)
				.AutoWrapText(true)
				.Text(LOCTEXT("HelperLabel", "Select some actors and move them around using buttons below"))
			]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("UpButtonLabel", "Up"), FVector(0, 0, Factor))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SHorizontalBox)
					+ SHorizontalBox::Slot()
					.AutoWidth()
					[
						Locals::MakeButton(LOCTEXT("LeftButtonLabel", "Left"), FVector(0, -Factor, 0))
					]
					+ SHorizontalBox::Slot()
						.AutoWidth()
						[
							Locals::MakeButton(LOCTEXT("RightButtonLabel", "Right"), FVector(0, Factor, 0))
						]
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					Locals::MakeButton(LOCTEXT("DownButtonLabel", "Down"), FVector(0, 0, -Factor))
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString("Make Noise"))
					.OnClicked(this, &FWorldArchitectEdModeToolkit::OnGenerateNoise)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(SButton)
					.Text(FText::FromString("Generate Landscape"))
					.OnClicked(this, &FWorldArchitectEdModeToolkit::OnGenerateLandscape)
				]
			+ SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					SNew(STextBlock)
					.Text(this, &FWorldArchitectEdModeToolkit::OnGetGraphName)
				
				]
			+SVerticalBox::Slot()
				.HAlign(HAlign_Center)
				.AutoHeight()
				[
					//PropertyCustomizationHelpers::MakeAssetPickerAnchorButton(OnGetAllowedClassesDel, OnAssetSelectedFromPickerDel)
					PropertyCustomizationHelpers::MakeAssetPickerWithMenu(DefaultAsset, true, AllowedClasses, NewAssetFactory, OnShouldFilterAssetDel, OnAssetSelectedFromPickerDel, OnCloseDel)
				]

		];
		
	FModeToolkit::Init(InitToolkitHost);
}

FName FWorldArchitectEdModeToolkit::GetToolkitFName() const
{
	return FName("WorldArchitectEdMode");
}

FText FWorldArchitectEdModeToolkit::GetBaseToolkitName() const
{
	return NSLOCTEXT("WorldArchitectEdModeToolkit", "DisplayName", "WorldArchitectEdMode Tool");
}

class FEdMode* FWorldArchitectEdModeToolkit::GetEditorMode() const
{
	return GLevelEditorModeTools().GetActiveMode(FWorldArchitectEdMode::EM_WorldArchitectEdModeId);
}
uint16 FWorldArchitectEdModeToolkit::PerlinNoise2D(float x, float y, float amp, int32 octaves, int32 px, int32 py)
{	
	float noise = 0.f;  
	for (int octave = 1; octave < octaves; octave *= 2) 
		noise += Noise1234::pnoise( x*px*octave, y*py*octave, px, py ) / octave;  

	return USHRT_MAX/2.f + amp*noise;
}
float FWorldArchitectEdModeToolkit::PerlinNoise2DFloat(float x, float y, float amp, int32 octaves, int32 px, int32 py)
{
	float noise = 0.f;
	for (int octave = 1; octave < octaves; octave *= 2)
		noise += Noise1234::pnoise(x*px*octave, y*py*octave, px, py) / octave;

	return noise;
}
FReply FWorldArchitectEdModeToolkit::OnGenerateNoise()
{
	GEditor->GetWorld();
	TArray<ALandscape*> Landscapes;
	for (TActorIterator<ALandscape> LIter(GetEditorMode()->GetWorld()); LIter; ++LIter)
	{
		Landscapes.Add(*LIter);
	}
	auto& LandscapeInfoMap = ULandscapeInfoMap::GetLandscapeInfoMap(GetEditorMode()->GetWorld());
	

	if (Landscapes.Num() > 0)
	{
		FIntRect bounds = Landscapes[0]->GetBoundingRect();

		int32 numHeights = (bounds.Width() + 1)*(bounds.Height() + 1);
		TArray<uint16> Data; 
		Data.Init(0, numHeights); 
		TArray<float> DataFloat;
		DataFloat.Init(0, numHeights);
		TArray<float> DataFloat2;
		DataFloat2.Init(0, numHeights);
		TArray<float> DataFloat3;
		DataFloat3.Init(0, numHeights);

		int32 cols = bounds.Width() + 1, rows = bounds.Height() + 1;
		int32 octaves = 16, px = 4, py = 4;
		float amplitude = 20000.f;
		//for (int i = 0; i < Data.Num(); i++) 
		//{
		//	float nx = (i % cols) / (float)cols; //normalized col
		//	float ny = (i / cols) / (float)rows; //normalized row
		//	Data[i] = FMath::Clamp<uint16>(PerlinNoise2D(nx, ny, amplitude, octaves, px, py), 0, 32768);
		//}
		for (int i = 0; i < DataFloat.Num(); i++)
		{
			float nx = (i % cols) / (float)cols; //normalized col
			float ny = (i / cols) / (float)rows; //normalized row
			DataFloat[i] = PerlinNoise2DFloat(nx, ny, amplitude, octaves, px, py);
			DataFloat2[i] = PerlinNoise2DFloat(nx, ny, amplitude, 4, 12, 12);
			DataFloat3[i] = DataFloat[i] * DataFloat2[i];
		}
		for (int i = 0; i < Data.Num(); i++)
		{
			Data[i] = (USHRT_MAX / 2.f) + (DataFloat3[i] * amplitude);
		}
		ULandscapeInfo::RecreateLandscapeInfo(GetEditorMode()->GetWorld(), 1);
		LandscapeEditorUtils::SetHeightmapData(Landscapes[0], Data);
		ULandscapeInfo::RecreateLandscapeInfo(GetEditorMode()->GetWorld(), 1);
		for (auto It = LandscapeInfoMap.Map.CreateIterator(); It; ++It)
		{
			ULandscapeInfo* LandscapeInfo = It.Value();
			if (LandscapeInfo && !LandscapeInfo->IsPendingKill())
			{
				ALandscapeProxy* LandscapeProxy = LandscapeInfo->GetLandscapeProxy();
				if (LandscapeProxy)
				{
					for (ALandscapeStreamingProxy* Proxy : LandscapeInfo->Proxies)
					{
						ULandscapeInfo::RecreateLandscapeInfo(GetEditorMode()->GetWorld(), 1);
						LandscapeEditorUtils::SetHeightmapData(Proxy, Data);
						ULandscapeInfo::RecreateLandscapeInfo(GetEditorMode()->GetWorld(), 1);
					}
					
					//if (CurrentToolTarget.LandscapeInfo == LandscapeInfo)
					{
						//CurrentIndex = Index;

						// Update GizmoActor Landscape Target (is this necessary?)
						//if (CurrentGizmoActor.IsValid())
						{
						//	CurrentGizmoActor->SetTargetLandscape(LandscapeInfo);
						}
					}

					int32 MinX, MinY, MaxX, MaxY;
					int32 Width = 0, Height = 0;
					if (LandscapeInfo->GetLandscapeExtent(MinX, MinY, MaxX, MaxY))
					{
						Width = MaxX - MinX + 1;
						Height = MaxY - MinY + 1;
					}

					/*LandscapeList.Add(FLandscapeListInfo(*LandscapeProxy->GetName(), LandscapeInfo,
						LandscapeInfo->ComponentSizeQuads, LandscapeInfo->ComponentNumSubsections, Width, Height));
					Index++;*/
				}
			}
		}
	}


		//LandscapeEditorUtils::SetHeightmapData(Landscapes[0], Data);

	return FReply::Unhandled();
}
FReply FWorldArchitectEdModeToolkit::OnGenerateLandscape()
{
	if(!LandscapeGraph.IsValid())
		return FReply::Unhandled();

	//LandscapeGraph->GenerateLandscape();
	if (!LandscapeGraph->Schema)
		return FReply::Unhandled();

	if (!LandscapeGraph->Schema->Output)
		return FReply::Unhandled();

	TArray<uint16> Out;
	LandscapeGraph->Schema->Output->GenerateHeightMap(Out);
	return FReply::Handled();
}
void FWorldArchitectEdModeToolkit::OnAssetSelected(const FAssetData& InAssetData)
{
	UObject* Object = InAssetData.GetAsset();
	if (UWALandscapeGraph* Graph = Cast<UWALandscapeGraph>(Object))
	{
		LandscapeGraph = Graph;
		return;
	}

	LandscapeGraph.Reset();
}
void FWorldArchitectEdModeToolkit::OnGetAllowedClasses(TArray<const UClass*>& OutAllowedClasses)
{
	OutAllowedClasses.Add(UWALandscapeGraph::StaticClass());
}
bool FWorldArchitectEdModeToolkit::OnShouldFilterAsset(const FAssetData& InAssetData)
{
	return false;
}
FText FWorldArchitectEdModeToolkit::OnGetGraphName() const
{
	if (LandscapeGraph.IsValid())
	{
		return FText::FromString(LandscapeGraph->GetName());
	}

	return FText::FromString("No Graph Selected");
}
#undef LOCTEXT_NAMESPACE
