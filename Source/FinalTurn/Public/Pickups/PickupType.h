#pragma once

#include "CoreMinimal.h"
#include "PickupType.generated.h"

UENUM(BlueprintType)
enum class EPickupType : uint8
{
	Bullet  UMETA(DisplayName = "Bullet"),
	Stone   UMETA(DisplayName = "Stone"),
	Granade UMETA(DisplayName = "Granade"),
	Dynamite UMETA(DisplayName = "Dynamite"),
	HeavyDynamite UMETA(DisplayName = "HeavyDynamite")
};
