#pragma once

#include "CoreMinimal.h"
#include "LevelProgressData.h"
#include "PlayerSave.generated.h"

USTRUCT(BlueprintType)
struct FS_PlayerSave
{
	GENERATED_BODY()

	// This makes it visible and editable in Blueprints
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 LevelsCompleted;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<int32, FS_LevelProgressData> LevelProgress;
};
