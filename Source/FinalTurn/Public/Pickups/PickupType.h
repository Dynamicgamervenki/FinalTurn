#pragma once

#include "CoreMinimal.h"
#include "PickupType.generated.h"

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	Stone   UMETA(DisplayName = "Stone"),
	Granade UMETA(DisplayName = "Granade"),
	Dynamite UMETA(DisplayName = "Dynamite"),
	HeavyDynamite UMETA(DisplayName = "HeavyDynamite"),
	LavaCrystal UMETA(DisplayName = "LavaCrystal"),
	LavaOrb UMETA(DisplayName = "LavaOrb")
};

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	None     UMETA(DisplayName = "None"),
	Stone    UMETA(DisplayName = "Stone"),
	Grenade  UMETA(DisplayName = "Grenade"),
	Dynamite UMETA(DisplayName = "Dynamite"),
	HeavyDynamite UMETA(DisplayName = "HeavyDynamite"),
	LavaCrystal UMETA(DisplayName = "LavaCrystal"),
	LavaOrb UMETA(DisplayName = "LavaOrb")
};

