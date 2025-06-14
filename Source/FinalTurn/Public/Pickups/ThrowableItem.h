// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableItem.generated.h"

UCLASS()
class FINALTURN_API AThrowableItem : public AActor
{
	GENERATED_BODY()
	
public:	
	AThrowableItem();

protected:
	virtual void BeginPlay() override;



public:	
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* SM_Throwable;
};
