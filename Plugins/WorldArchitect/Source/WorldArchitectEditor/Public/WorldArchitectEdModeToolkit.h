// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Toolkits/BaseToolkit.h"
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
\brief Declares the Noise1234 class for producing Perlin noise.
\author Stefan Gustavson (stegu@itn.liu.se)
*/

/*
* This is a clean, fast, modern and free Perlin noise class in C++.
* Being a stand-alone class with no external dependencies, it is
* highly reusable without source code modifications.
*
* Note:
* Replacing the "float" type with "double" can actually make this run faster
* on some platforms. A templatized version of Noise1234 could be useful.
*/

class Noise1234 {

public:
	Noise1234() {}
	~Noise1234() {}

	/** 1D, 2D, 3D and 4D float Perlin noise, SL "noise()"
	*/
	static float noise(float x);
	static float noise(float x, float y);
	static float noise(float x, float y, float z);
	static float noise(float x, float y, float z, float w);

	/** 1D, 2D, 3D and 4D float Perlin periodic noise, SL "pnoise()"
	*/
	static float pnoise(float x, int px);
	static float pnoise(float x, float y, int px, int py);
	static float pnoise(float x, float y, float z, int px, int py, int pz);
	static float pnoise(float x, float y, float z, float w,
		int px, int py, int pz, int pw);

private:
	static unsigned char perm[];
	static float  grad(int hash, float x);
	static float  grad(int hash, float x, float y);
	static float  grad(int hash, float x, float y, float z);
	static float  grad(int hash, float x, float y, float z, float t);

};
class FWorldArchitectEdModeToolkit : public FModeToolkit
{
	
	TWeakObjectPtr<class UWALandscapeGraph> LandscapeGraph;
public:

	FWorldArchitectEdModeToolkit();
	
	/** FModeToolkit interface */
	virtual void Init(const TSharedPtr<IToolkitHost>& InitToolkitHost) override;

	/** IToolkit interface */
	virtual FName GetToolkitFName() const override;
	virtual FText GetBaseToolkitName() const override;
	virtual class FEdMode* GetEditorMode() const override;
	virtual TSharedPtr<class SWidget> GetInlineContent() const override { return ToolkitWidget; }
	uint16 PerlinNoise2D(float x, float y, float amp, int32 octaves, int32 px, int32 py);
	float PerlinNoise2DFloat(float x, float y, float amp, int32 octaves, int32 px, int32 py);
	FReply OnGenerateNoise();
	FReply OnGenerateLandscape();
	void OnAssetSelected(const FAssetData& InAssetData);
	void OnGetAllowedClasses(TArray<const UClass*>& OutAllowedClasses);
	bool OnShouldFilterAsset(const FAssetData& InAssetData);
	FText OnGetGraphName() const;
private:

	TSharedPtr<SWidget> ToolkitWidget;
};
