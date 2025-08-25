// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GenericTeamAgentInterface.h"
#include "ZackPlayerController.generated.h"

UCLASS()
class FINALTURN_API AZackPlayerController : public APlayerController , public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	AZackPlayerController();

	virtual FGenericTeamId GetGenericTeamId() const;

private:
	FGenericTeamId ZackTeamID;
};
