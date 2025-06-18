#pragma once

#include "CoreMinimal.h"
#include "LevelProgressData.generated.h"

USTRUCT(BlueprintType)
struct FS_LevelProgressData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float BestTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 StarsEarned;
};
