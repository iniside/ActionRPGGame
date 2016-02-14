// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Object.h"
#include "GASInputOverride.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, meta=(EditInLineNew))
class GAMEABILITIES_API UGASInputOverride : public UObject
{
	GENERATED_BODY()
public:
	/* 
		Abilities given by this input override.
		Ie if you have weapon, which is giving abilities, those should be handled here.

		Order of abilities here, will indicate order of abilities in given Set in GASAbilityComponent.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Config")
		TArray<TSubclassOf<class UGASAbilityBase> > AbilitiesToGive;
	
	/* Index of ability set, using this Input Override. */
	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
		int32 SetIndex;

	UPROPERTY(BlueprintReadOnly, Category = "Abilities")
	class UGASAbilitiesComponent* AbilityComp;


	UFUNCTION(BlueprintImplementableEvent)
		void OnInputPressed(int32 Index);

	UFUNCTION(BlueprintImplementableEvent)
		void OnInputReleased(int32 Index);
	/*
		Index in both functions indicates button.
		For example:
		0 - Left Mouse Button
		1 - Right Mouse Button
		2 - Middle Mouse Button
		3 - Q
		etc.
		What each index means, will really depend on how inputs are setup inside PlayerController,
		Character or other actor which will provide input information.
	*/
	virtual void NativeInputPressed(int32 Index);
	virtual void NativeInputReleased(int32 Index);

	bool IsNameStableForNetworking() const override
	{
		return false;
	}

	bool IsSupportedForNetworking() const override
	{
		return true;
	}
	void SetNetAddressable() {}
};
