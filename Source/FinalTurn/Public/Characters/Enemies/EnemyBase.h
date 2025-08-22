// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractInterface.h"
#include "EnemyBase.generated.h"

class UPawnSensingComponent;

UCLASS()
class FINALTURN_API AEnemyBase : public ACharacter 
{
	GENERATED_BODY()

public:
	AEnemyBase();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

public:	
	virtual void Tick(float DeltaTime) override;
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(BlueprintReadWrite)
	bool IsDead = false;
};
