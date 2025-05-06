// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickups/Pickup.h"
#include "Stone.generated.h"

/**
 * 
 */
UCLASS()
class FINALTURN_API AStone : public APickup
{
	GENERATED_BODY()

	public:
	AStone();

protected:
	virtual void BeginPlay() override;
	virtual void Tick( float DeltaSeconds ) override;

public:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	virtual  void OnSphereEndOverlap( UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	int32 StoneCount;
};
