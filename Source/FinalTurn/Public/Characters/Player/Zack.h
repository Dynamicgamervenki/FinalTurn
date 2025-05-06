// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PickupInterface.h"
#include "Zack.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

class APickup;

UENUM(BlueprintType)
enum class EEquipState : uint8
{
	None     UMETA(DisplayName = "None"),
	 Stone    UMETA(DisplayName = "Stone"),
	 Grenade  UMETA(DisplayName = "Grenade"),
	 Gun      UMETA(DisplayName = "Gun")
};

UCLASS()
class FINALTURN_API AZack : public ACharacter ,public IPickupInterface
{
	GENERATED_BODY()

public:
	AZack();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;
	UPROPERTY(VisibleAnywhere,Category= "Input")
	APickup* PickupItem;

	UPROPERTY(BlueprintReadWrite,Category= "Pickups")
	int32 BulletCount;
	UPROPERTY(BlueprintReadWrite,Category= "Pickups")
	int32 StoneCount;
	UPROPERTY(BlueprintReadWrite,Category= "Pickups")
	int32 GranadeCount;
	UPROPERTY(BlueprintReadWrite,Category= "Move")
	FVector TargetLocation;
	UPROPERTY(EditAnywhere,Category= "Move")
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	
private:
	
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputMappingContext> IMC_FinalTurn;

	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> IA_Move;

	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> IA_EquipWeapon;
	
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> IA_EquipStone;
	
	UPROPERTY(EditAnywhere,Category = "Input")
	TObjectPtr<UInputAction> IA_EquipGranade;

UPROPERTY(EditAnywhere)
	UClass* ThrowableStoneClass;
UPROPERTY(EditAnywhere)
	UClass* StoneClass;
	
	void PrintOutData();
	
	UFUNCTION(BlueprintCallable)
	void OnInteract();
	UFUNCTION(BlueprintCallable)
	void EquipWeapon();
	UFUNCTION(BlueprintCallable)
	void EquipStone();
	UFUNCTION(BlueprintCallable)
	void EquipGranade();

	EEquipState EquipState = EEquipState::None;

	void DoMoveTo(const FVector& Dest);
	void DoThrowStoneAt(const FVector& Dest);
	void DoThrowGrenadeAt(const FVector& Dest);
	

	/*
	 * ANIMATION MONTAGES
	 */
	UPROPERTY(EditDefaultsOnly,Category = "Montages")
	UAnimMontage* ThrowMontage;
	
	UFUNCTION()
	void HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload);


public:
	FORCEINLINE void SetPickupItem(APickup* Pickup) {PickupItem= Pickup;}

	virtual void OnPickedUp(EPickupType PickupType ,int32 Amount) override;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Move")
	bool IsMoving = false;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Life")
	bool IsAlive = true;
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category= "Combat")
	bool CanClickNode = true;

};
