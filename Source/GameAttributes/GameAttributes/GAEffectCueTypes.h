#pragma once
#include "GameplayTagContainer.h"
#include "GAGlobalTypes.h"
#include "GAEffectCueTypes.generated.h"

USTRUCT()
struct FGAActiveEffectCue
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		FGAEffectHandle MyHandle;
	UPROPERTY()
		FGAEffectContext Context;

	UPROPERTY()
		float Duration;
	UPROPERTY()
		float Period;
	/*
	Weak reference to cue associated with this effect.
	*/
	UPROPERTY()
		TWeakObjectPtr<class AGAEffectCue> EffectCue;
private:
	FTimerHandle DurationTimerHandle;
	FTimerHandle PeriodTimerHandle;
public:
	void CleanUp();

	void CueRemoved();

	void CueExpired();

	/*
	Called when effect has been activated or replicated to client.
	*/
	void OnActivated();

	void OnApplied();
	/* Called on period of this effect. */
	void OnPeriod();
	/* Called when effect externally removed. */
	void OnRemoved();
	/* Called when effect naturally expires. */
	void OnEnded();

	inline bool operator==(FGAActiveEffectCue& OtherIn) const
	{
		return MyHandle == OtherIn.MyHandle;
	}

	FGAActiveEffectCue()
	{};

	FGAActiveEffectCue(const FGAEffectHandle& HandleIn, TWeakObjectPtr<class AGAEffectCue> CueActorIn)
		: MyHandle(HandleIn),
		EffectCue(CueActorIn)
	{};

	~FGAActiveEffectCue();
};

USTRUCT()
struct FGAActiveEffectCuesContainer
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY()
		TArray<FGAActiveEffectCue> ActiveCues;

	void AddCue(const FGAActiveEffectCue& CueIn);

	void CueRemoved(const FGAEffectHandle& HandleIn);

	void CueExpired(const FGAEffectHandle& HandleIn);
};