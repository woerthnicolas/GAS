// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/InventoryList.h"
#include "InventoryComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GAS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryComponent();

	virtual void InitializeComponent() override;

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

	UFUNCTION(BlueprintCallable)
	void AddItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	UFUNCTION(BlueprintCallable)
	void RemoveItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	UFUNCTION(BlueprintCallable)
	void EquipItem(TSubclassOf<UItemStaticData> InItemStaticDataClass);
	UFUNCTION(BlueprintCallable)
	void UnequipItem();
	UFUNCTION(BlueprintCallable)
	UInventoryItemInstance* GetEquippedItem() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	FInventoryList InventoryList;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UItemStaticData>> DefaultItems;

	UPROPERTY(Replicated)
	UInventoryItemInstance* CurrentItem = nullptr;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
