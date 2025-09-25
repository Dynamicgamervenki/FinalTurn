// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickups/PickupType.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Pickup.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThrowableImpact, AActor*, HitActor);

class URotatingMovementComponent;
class UFieldSystemComponent;
class URadialVector;
class UFieldSystemMetaData;
class URadialFalloff;
class USphereComponent;

UCLASS()
class FINALTURN_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	APickup();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	URotatingMovementComponent* RotatingMovement;
	
public:	
	virtual void Tick(float DeltaTime) override;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadWrite)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;
	
	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* PulseEffect;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPickUpSound(FVector Location);

	UPROPERTY(BlueprintReadWrite)
	EPickupType PickupType;

	UPROPERTY(EditInstanceOnly,BlueprintReadWrite)
	int PickupAmount = 0;

	UPROPERTY(BlueprintAssignable)
	FOnThrowableImpact OnThrowableImpact; 
	
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URadialFalloff> RadialFalloff;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UFieldSystemMetaData> FieldSystemMetaDataFilter;
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<URadialVector> RadialVector;

	UFUNCTION(BlueprintImplementableEvent)
	void Field(FVector Location);
	UFUNCTION(BlueprintCallable)
	void ActivateField(FVector Location);
	
	FORCEINLINE URotatingMovementComponent* GetRotatingMovement() const { return RotatingMovement; }

	UFUNCTION(BlueprintCallable)
	void BroadcastThrowableImpact(AActor* HitActor);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayPickupImpactSound(FVector Location);
};
