#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FastArrayTagCounter.generated.h"

struct FGameplayTag;
struct FInventoryListItem;

USTRUCT(BlueprintType)
struct FFastArrayTagCounterRecord : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY()
	FGameplayTag Tag;

	UPROPERTY()
	int32 Count;
};

USTRUCT(BlueprintType)
struct FFastArrayTagCounter : public FFastArraySerializer
{
	GENERATED_BODY()

public:
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FFastArrayTagCounterRecord, FFastArrayTagCounter>(
			TagArray, DeltaParms, *this
		);
	}

	int32 GetTagCount(FGameplayTag InTag) const;

	void AddTagCount(FGameplayTag InTag, int32 Delta);

	const TArray<FFastArrayTagCounterRecord>& GetTagArray() const;
	
protected:
	UPROPERTY()
	TArray<FFastArrayTagCounterRecord> TagArray;
};

template<>
struct TStructOpsTypeTraits<FFastArrayTagCounter> : public TStructOpsTypeTraitsBase2<FFastArrayTagCounter>
{
	enum  { WithNetDeltaSerializer = true };
};


