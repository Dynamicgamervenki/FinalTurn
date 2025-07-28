// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "BreakableActor.generated.h"

enum class EPickupType : uint8;
class USphereComponent;
class UGeometryCollectionComponent;

UCLASS()
class FINALTURN_API ABreakableActor : public AActor , public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<USphereComponent> SphereCollision;


protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual FVector InteractPosition_Implementation() override;
	virtual TArray<AActor*> GetOverlappingActorsOnNode_Implementation() override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breakable")
	int AmountToGetDestoryed = 1;
	UPROPERTY(BlueprintReadWrite, Category="Breakable")
	int ThrownCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category=Default)
	bool bPlaceHeavyDynamiteOnClick;
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category=Default)
	AActor* HeavydynamitePlacingPositionActor;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

public:
	FORCEINLINE bool ShouldPlaceHeavyDynamiteOnClick() const{ return bPlaceHeavyDynamiteOnClick; }
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool bStopBeforeUnits;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	float UnitsBeforeStop = 100.0f;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	EPickupType DestoroyablePickup;
	
};
