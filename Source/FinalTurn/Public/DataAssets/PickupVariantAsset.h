// DynamicV

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Characters/Player/Zack.h"
#include "PickupVariantAsset.generated.h"

class APickup;
class AThrowableItem;
class PickupType;

USTRUCT(BlueprintType)
struct FPickupVariantData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EPickupType PickupType; // Stone or Grenade or Bullet

	UPROPERTY(EditAnywhere)
	EEquipState EquipState; // Helps decide equip/throw logic

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APickup> PickupClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AThrowableItem> ThrowableClass;

	UPROPERTY(EditAnywhere)
	FName SocketName; // Optional, override default socket if needed
};
UCLASS()
class FINALTURN_API UPickupVariantAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	FName VariantName;

	UPROPERTY(EditAnywhere)
	FPickupVariantData PickupData;
	
};
