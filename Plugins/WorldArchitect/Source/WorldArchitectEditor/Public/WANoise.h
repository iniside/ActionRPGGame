// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once



class FNoise
{
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
public:
	static unsigned char perm[];
	FNoise()
	{

	}
	static float grad(int hash, float x) {
		int h = hash & 15;
		float grad = 1.0 + (h & 7);  // Gradient value 1.0, 2.0, ..., 8.0
		if (h & 8) grad = -grad;         // and a random sign for the gradient
		return (grad * x);           // Multiply the gradient with the distance
	}

	static float grad(int hash, float x, float y) {
		int h = hash & 7;      // Convert low 3 bits of hash code
		float u = h<4 ? x : y;  // into 8 simple gradient directions,
		float v = h<4 ? y : x;  // and compute the dot product with (x,y).
		return ((h & 1) ? -u : u) + ((h & 2) ? -2.0*v : 2.0*v);
	}

	static float grad(int hash, float x, float y, float z) {
		int h = hash & 15;     // Convert low 4 bits of hash code into 12 simple
		float u = h<8 ? x : y; // gradient directions, and compute dot product.
		float v = h<4 ? y : h == 12 || h == 14 ? x : z; // Fix repeats at h = 12 to 15
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
	}

	static float grad(int hash, float x, float y, float z, float t) {
		int h = hash & 31;      // Convert low 5 bits of hash code into 32 simple
		float u = h<24 ? x : y; // gradient directions, and compute dot product.
		float v = h<16 ? y : z;
		float w = h<8 ? z : t;
		return ((h & 1) ? -u : u) + ((h & 2) ? -v : v) + ((h & 4) ? -w : w);
	}

	//---------------------------------------------------------------------
	/** 1D float Perlin noise, SL "noise()"
	*/
	static float noise(float x)
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
	static float pnoise(float x, int px)
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
	static float noise(float x, float y)
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
	static float pnoise(float x, float y, int px, int py)
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
	static float noise(float x, float y, float z)
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
	static float pnoise(float x, float y, float z, int px, int py, int pz)
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

	static float noise(float x, float y, float z, float w)
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

	static float pnoise(float x, float y, float z, float w,
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

};