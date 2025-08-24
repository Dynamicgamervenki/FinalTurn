// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractInterface.h"
#include "Node.generated.h"

class AZack;
class UBoxComponent;

UCLASS()
class FINALTURN_API ANode : public AActor ,public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	ANode();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SM_Node;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UBoxComponent> Box;
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> SM_Ring;

protected:
	virtual void BeginPlay() override;	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	 UFUNCTION()
	virtual  void OnBoxEndOverlap( UPrimitiveComponent* OverlappedComponent ,AActor* OtherActor ,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	virtual FVector InteractPosition_Implementation() override;
	virtual TArray<AActor*> GetOverlappingActorsOnNode_Implementation() override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void Glow_Implementation() override;
	virtual void ResetGlow_Implementation() override;
	
public :
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool Is_EndNode;
	UPROPERTY(EditAnywhere, Category=Default, meta=(EditCondition="Is_EndNode"))
	FName LevelName;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool bCannotMoveToNode = false;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool bStopBeforeUnits;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Default,meta=(EditCondition="bStopBeforeUnits"))
	float UnitsBeforeStop = 100.0f;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool isTeleportNode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Default,meta=(EditCondition="isTeleportNode"))
	ANode* TeleportNode;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Default,meta=(EditCondition="isTeleportNode"))
	ANode* PostTeleportNode;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool HiddenNode = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Default,meta=(EditCondition="HiddenNode"))
	AActor* HidingLocationActor;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	FLinearColor Color = FLinearColor::White;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool isGasNode = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(EditCondition="isGasNode"))
	bool isGasNodeActivated = false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= Default)
	bool isTempNode = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(EditCondition="isTempNode"))
	bool isTempNodeActivated = false;
	
	

private:
	UFUNCTION()
	void GlowNode();
	UFUNCTION()
	void HandleTeleportNode(AZack* Zack);
	UFUNCTION()
	void HandleFinalNodeTransition(AZack* Zack);
	
};
  