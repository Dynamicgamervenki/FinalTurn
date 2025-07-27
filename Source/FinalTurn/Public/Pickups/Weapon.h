// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UCLASS()
class FINALTURN_API AWeapon : public APickup
{
	GENERATED_BODY()
	
public:	
	AWeapon();
	void Equip(USceneComponent* InParent ,FName InSoketName);

protected:
	virtual void BeginPlay() override;
	 void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult) override;
	 void OnSphereEndOverlap( UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

public:	
	virtual void Tick(float DeltaTime) override;
	
};
