// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickups/Pickup.h"
#include "Pickups/PickupType.h"
#include "PickupInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};
class FINALTURN_API IPickupInterface 
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnPickedUp(EPickupType PickupType, int32 Amount) = 0;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCanClickOnNode(bool click);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetDetectedByEnemy(bool bDetected);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool GetIsHiding();
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetIsHiding(bool isHiding);
	
	TArray<APickup*> Pickups;

	FORCEINLINE void AddToPickupArray(APickup* Pickup) {Pickups.Add(Pickup);}

};
