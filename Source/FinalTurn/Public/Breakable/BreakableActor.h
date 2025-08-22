// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "BreakableActor.generated.h"

class ANode;
struct FChaosBreakEvent;
enum class EPickupType : uint8;
class USphereComponent;
class UGeometryCollectionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBreakableDestroyed, AActor*, HitActor);

UCLASS()
class FINALTURN_API ABreakableActor : public AActor , public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<USphereComponent> SphereCollision;

	UPROPERTY()
	FOnBreakableDestroyed OnBreakableDestroyed;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	virtual FVector InteractPosition_Implementation() override;
	virtual TArray<AActor*> GetOverlappingActorsOnNode_Implementation() override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void Glow_Implementation() override;
	virtual void ResetGlow_Implementation() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breakable")
	int AmountToGetDestoryed = 1;
	UPROPERTY(BlueprintReadWrite, Category="Breakable")
	int ThrownCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category=Breakable)
	bool bPlaceHeavyDynamiteOnClick;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Breakable,meta=(EditCondition="bPlaceHeavyDynamiteOnClick"))
	AActor* HeavydynamitePlacingPositionActor;

	UFUNCTION()
	void OnGeometryCollectionBreak(const FChaosBreakEvent& BreakEvent);

	UPROPERTY(EditAnywhere,Category=Breakable)
	ANode* NodeToMoveAfterDestroyingBreakable;
	
private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;
	int Hit = 0;

	UFUNCTION()
	void HandleZackOverlap(AZack* Zack);
	UFUNCTION()
	void HandlePickupOverlap(APickup* Pickup);
	UFUNCTION()
	void DestroyBreakable();


public:
	FORCEINLINE bool ShouldPlaceHeavyDynamiteOnClick() const{ return bPlaceHeavyDynamiteOnClick; }
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Breakable)
	TArray<EPickupType> DestoroyablePickup;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Breakable)
	bool bStopBeforeUnits;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Breakable,meta=(EditCondition="bStopBeforeUnits"))
	float UnitsBeforeStop = 100.0f;
};
