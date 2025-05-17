// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Node.generated.h"

class UBoxComponent;

UCLASS()
class FINALTURN_API ANode : public AActor ,public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ANode();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SM_Node;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> Box;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	UFUNCTION()
	virtual  void OnBoxEndOverlap( UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual FVector InteractPosition() override;
	virtual TArray<AActor*> GetOverlappingActorsOnNode() override;
	
	UPROPERTY(EditAnywhere)
	bool bIsEndNode;
	
};
  