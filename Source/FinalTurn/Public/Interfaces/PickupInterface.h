// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Pickups/Pickup.h"
#include "PickupInterface.generated.h"

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	Bullet  UMETA(DisplayName = "Bullet"),
	Stone  UMETA(DisplayName = "Stone"),
    Granade UMETA(DisplayName = "Granade")
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FINALTURN_API IPickupInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void OnPickedUp(EPickupType PickupType, int32 Amount) = 0;
	
	TArray<APickup*> Pickups;

	FORCEINLINE void AddToPickupArray(APickup* Pickup) {Pickups.Add(Pickup);}

};
