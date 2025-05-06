// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ZackPlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class FINALTURN_API AZackPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AZackPlayerController();

protected:
	virtual void BeginPlay() override;
private:

};
