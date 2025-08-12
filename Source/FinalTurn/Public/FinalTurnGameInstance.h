// DynamicV

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "FinalTurnGameInstance.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunSpawnedDelegate, AActor*, SpawnedGunActor);
UCLASS()
class FINALTURN_API UFinalTurnGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FGunSpawnedDelegate OnGunSpawned;
protected:
	UPROPERTY(BlueprintReadOnly)
	TSoftClassPtr<AActor> ShotGun;
	UPROPERTY(BlueprintReadWrite)
	AActor* SpawnedShotGun;
private:
	UFUNCTION(BlueprintCallable)
	void LoadGunAsync();
	
};
