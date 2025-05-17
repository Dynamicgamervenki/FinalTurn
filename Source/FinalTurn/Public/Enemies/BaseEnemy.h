// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractInterface.h"
#include "BaseEnemy.generated.h"

UCLASS()
class FINALTURN_API ABaseEnemy : public ACharacter , public IInteractInterface
{
	GENERATED_BODY()

public:
	ABaseEnemy();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual FVector InteractPosition() override;
	virtual TArray<AActor*> GetOverlappingActorsOnNode() override;
};
