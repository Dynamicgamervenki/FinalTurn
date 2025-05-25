// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;

UCLASS()
class FINALTURN_API ABreakableActor : public AActor , public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ABreakableActor();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	virtual FVector InteractPosition() override;
	virtual TArray<AActor*> GetOverlappingActorsOnNode() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Breakable")
	int AmountToGetDestoryed = 1;
	UPROPERTY(BlueprintReadWrite, Category="Breakable")
	int ThrownCount;

private:
	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

};
