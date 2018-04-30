// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AMTypes.generated.h"

UENUM(BlueprintType)
enum class EAMSlot : uint8
{
	Slot001 = 0,
	Slot002 = 1,
	Slot003 = 2,
	Slot004 = 3,
	Slot005 = 4,
	Slot006 = 5,
	Slot007 = 6,
	Slot008 = 7,
	Slot009 = 8,
	Slot010 = 9,
	Slot011 = 10,
	Slot012 = 11,
	Slot013 = 12,
	Slot014 = 13,
	Slot015 = 14,
	Slot016 = 15,
	Slot017 = 16,
	Slot018 = 17,
	Slot019 = 18,
	Slot020 = 19,
	Slot021 = 20,
	Slot022 = 21,
	Slot023 = 22,
	Slot024 = 23,
	Slot025 = 24,
	Slot026 = 25,
	Slot027 = 26,
	Slot028 = 27,
	Slot029 = 28,
	Slot030 = 29,
	Slot031 = 30,
	Slot032 = 31,
	MAX
};

UENUM(BlueprintType)
enum class EAMGroup : uint8
{
	Group001 = 0,
	Group002 = 1,
	Group003 = 2,
	Group004 = 3,
	Group005 = 4,
	Group006 = 5,
	Group007 = 6,
	Group008 = 7,
	Group009 = 8,
	Group010 = 9,
	Group011 = 10,
	Group012 = 11,
	Group013 = 12,
	Group014 = 13,
	Group015 = 14,
	Group016 = 15,
	Group017 = 16,
	Group018 = 17,
	Group019 = 18,
	Group020 = 19,
	Group021 = 20,
	Group022 = 21,
	Group023 = 22,
	Group024 = 23,
	Group025 = 24,
	Group026 = 25,
	Group027 = 26,
	Group028 = 27,
	Group029 = 28,
	Group030 = 29,
	Group031 = 30,
	Group032 = 31,

	MAX
};

template<typename T>
int32 AMEnumToInt(T InVal)
{
	return static_cast<int32>(InVal);
}

template<typename T>
T AMIntToEnum(int32 InVal)
{
	return static_cast<T>(InVal);
}