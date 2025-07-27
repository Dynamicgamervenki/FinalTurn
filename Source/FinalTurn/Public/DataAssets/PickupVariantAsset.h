// DynamicV

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Pickups/PickupType.h"
#include "PickupVariantAsset.generated.h"

class APickup;
class AThrowableItem;

USTRUCT(BlueprintType)
struct FPickupVariantData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	EPickupType PickupType; 

	UPROPERTY(EditAnywhere)
	EEquipState EquipState; 

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<APickup> PickupClass;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<AThrowableItem> ThrowableClass;

	UPROPERTY(EditAnywhere)
	FName SocketName; 
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
