// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ThrowableItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnThrowableImpact, AActor*, HitActor);

class USphereComponent;
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
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	USphereComponent* Sphere;

	UPROPERTY(BlueprintAssignable)
	FOnThrowableImpact OnThrowableImpact;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
};
