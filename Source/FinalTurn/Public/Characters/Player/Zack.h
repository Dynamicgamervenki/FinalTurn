#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PickupInterface.h"
#include "Zack.generated.h"

struct FPickupVariantData;
class UPickupVariantAsset;
struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class APickup;
class UAnimMontage;
class AThrowableItem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPickupUpdated, EPickupType, PickupType, int32, NewAmount);

UCLASS()
class FINALTURN_API AZack : public ACharacter, public IPickupInterface
{
    GENERATED_BODY()

public:
    AZack();

    UPROPERTY(BlueprintAssignable)
    FOnPickupUpdated OnPickupUpdated;
    // --- Unreal Overrides ---
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    
    UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Components")
    UPawnNoiseEmitterComponent* PawnNoiseEmitter;

    // --- Pickup Interface ---
    virtual void OnPickedUp(EPickupType PickupType, int32 Amount) override;
    virtual void SetDetectedByEnemy_Implementation(bool bDetected) override;
    virtual void SetIsHiding_Implementation(bool isHiding) override;
    
    /** Is character currently moving? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    bool IsMoving = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    bool IsHiding = false;
    virtual bool GetIsHiding_Implementation() override; 

    /** Can click nodes? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    bool CanClickNode = true;
    virtual void SetCanClickOnNode_Implementation(bool click) override;
    
    /** Detected By Enemy? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    bool IsAlive = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    bool GotDetectedByEnemy = false;
    
    // --- Inline Setters for Pickup Items ---
    FORCEINLINE void SetEquippedItem(APickup* Equipped) { EquippedItem = Equipped;}

    UFUNCTION()
    void DoMoveTo(const FVector& Dest,float Offset = 20,bool IgnoreDistance = false);
    
    // --- Internal State ---
    EEquipState EquipState = EEquipState::None;
    void DoThrowEquipItem(const FVector& Dest,AActor* HitActor);

    void PerformEquipStateAction(EEquipState State, const FVector& InteractLocation, AActor* HitActor);

    UFUNCTION(BlueprintCallable)
    void ReportNoise(AActor* NoiseMaker, float Loudness, const FVector& NoiseLocation);

    UFUNCTION(BlueprintCallable)
    void AmmoUpdateBroadCast(EPickupType type,int Ammo);
    
    UFUNCTION(BlueprintImplementableEvent)
    void PlayPlacignHeavyDynamiteMontage();

    UPROPERTY(BlueprintReadWrite, Category = "Move")
    FVector HeavyDynamiteSpawnLocation;
    
    UPROPERTY(BlueprintReadWrite, Category = "Breakable")
    AActor* BreakableActor;

protected:
    // --- Input Handling ---
    UFUNCTION(BlueprintCallable) void OnInteract();
    UFUNCTION(BlueprintCallable) void Equip(EPickupType Pickup);
    UFUNCTION() void EquipPickUp(FPickupVariantData PickupData);
    
    // --- Debug/Utility ---
    UFUNCTION()
    void PrintOutData();
    
    // --- Movement Data ---
    UPROPERTY(BlueprintReadWrite, Category = "Move")
    FVector TargetLocation;
    UPROPERTY(BlueprintReadWrite, Category = "Move")
    FVector PreviousNodeLocation;
    UPROPERTY(EditAnywhere, Category = "Move")
    double moveDistance;
    UPROPERTY(EditAnywhere, Category = "Move")
    TArray<TEnumAsByte<EObjectTypeQuery>> InteractableObjectTypes;

    // --- Input Bindings ---
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputMappingContext> IMC_FinalTurn;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> IA_Move;
    
    // --- Pickup Items ---
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    APickup* PickupItem;
    UPROPERTY(VisibleAnywhere, Category = "Combat")
    APickup* EquippedItem;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 StoneCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 GranadeCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 DynamiteCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 HeavyDynamiteCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 LavaCrystalCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 LavaOrbCount;

    // --- Projectile Classes ---
    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableStoneClass;

    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableGrenadeClass;

    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableDynamiteClass;
    
    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableHeavyDynamiteClass;
    
    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableLavaCrystalClass;

    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableLavaOrbClass;
    
    // --- Animation Montages ---
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* ThrowMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* EquipStoneMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* StealthMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* PlaceHeavyDynamiteMontage;

    UFUNCTION(BlueprintCallable)
    bool CanClickOnNode(const FVector &Dest);

    UPROPERTY(BlueprintReadWrite)
    FVector MoveLocation;
    
    UFUNCTION(BlueprintImplementableEvent)
    void PlayInteractionSound(FVector Location);
    
    UPROPERTY(EditAnywhere, Category="Pickup Variants")
    TMap<EPickupType, UPickupVariantAsset*> PickupVariantMap;

private:
    UFUNCTION()
    void PlayAnimMontages(UAnimMontage* MontageToPlay);
    UFUNCTION()
    virtual void OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    UFUNCTION()
    void PlayAnimMontageInReverse(UAnimMontage* MontageToPlay);
    UFUNCTION()
    void HandleThrowableImpact(AActor* HitActor);
    UFUNCTION()
    void HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);
    UFUNCTION()
    bool HasAmmoForEquipState(EEquipState State);
    UFUNCTION()
    void OnPickupClassLoaded(TSoftClassPtr<APickup> LoadedClass, FName SocketName, EEquipState InEquipState);
    UFUNCTION()
    void OnThrowableLoaded(TSoftClassPtr<AThrowableItem> LoadedClass);
    
};
