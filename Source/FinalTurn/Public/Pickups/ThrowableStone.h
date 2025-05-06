// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableStone.generated.h"

UCLASS()
class FINALTURN_API AThrowableStone : public AActor
{
	GENERATED_BODY()
	
public:	
	AThrowableStone();

protected:
	virtual void BeginPlay() override;



public:	
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* SM_Stone;
};
