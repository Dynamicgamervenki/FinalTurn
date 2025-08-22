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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGunUnequip,EPickupType,InPickupType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGunSpawnedDelegate, AActor*, SpawnedGunActor);


UCLASS()
class FINALTURN_API AZack : public ACharacter, public IPickupInterface
{
    GENERATED_BODY()

public:
    AZack();

    UPROPERTY(BlueprintAssignable)
    FOnPickupUpdated OnPickupUpdated;
    UPROPERTY(BlueprintAssignable)
    FOnGunUnequip OnGunUnequip;

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

    UFUNCTION(BlueprintCallable)
    void DoMoveTo(const FVector& Dest,float Offset = 20,bool IgnoreDistance = false);
    
    // --- Internal State ---
    UPROPERTY(BlueprintReadWrite)
    EEquipState CurrentEquipState = EEquipState::None;
    UPROPERTY(BlueprintReadWrite)
    EPickupType CurrentPickupType = EPickupType::None;
    void ThrowEquippedItem(const FVector& Dest,AActor* HitActor,bool IgnoreDistance = false);

    UFUNCTION(BlueprintImplementableEvent)
    void ShootGun(const FVector& Dest,AActor* HitActor,bool IgnoreDistance = false);
    UFUNCTION(BlueprintImplementableEvent)
    void UnEquipGun(EPickupType InPickupType);
    
    void PerformEquipStateAction(EEquipState State, const FVector& InteractLocation, AActor* HitActor);

    UFUNCTION(BlueprintCallable)
    void ReportNoise(AActor* NoiseMaker, float Loudness, const FVector& NoiseLocation);

    UFUNCTION(BlueprintImplementableEvent)
    void PlayPlacignHeavyDynamiteMontage();

    UPROPERTY(BlueprintReadWrite, Category = "Move")
    FVector HeavyDynamiteSpawnLocation;
    
    UPROPERTY(BlueprintReadWrite, Category = "Breakable")
    AActor* BreakableActor;

    UPROPERTY(VisibleInstanceOnly,BlueprintReadWrite,Category = Default)
    TArray<APickup*> PickupActors;

    
    UFUNCTION(BlueprintCallable)
    void AddPickUpItem(APickup* Pickup);
    
    UPROPERTY(BlueprintReadWrite)
    bool bOnFinalNode;
    
    UFUNCTION(BlueprintCallable)
    int32 GetAmmoOfState(EPickupType PickupType);

    UFUNCTION(BlueprintImplementableEvent)
    void ShowGameCompletedWidget();
    
    UFUNCTION(BlueprintCallable)
    void LoadGunAsync();

    UPROPERTY(BlueprintAssignable)
    FGunSpawnedDelegate OnGunSpawned;
    
protected:
    // --- Input Handling ---
    UFUNCTION(BlueprintCallable) void OnInteract();
    UFUNCTION(BlueprintCallable) void Equip(EPickupType Pickup);
    UFUNCTION() void EquipPickupFromInventory(FPickupVariantData PickupData);
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
    UPROPERTY(VisibleAnywhere,BlueprintReadWrite, Category = "Combat")
    APickup* EquippedItem;
    
    // --- Animation Montages ---
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* ThrowMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* EquipStoneMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* StealthMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* PlaceHeavyDynamiteMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* DrawShotGunMontage;
    

    UFUNCTION(BlueprintCallable)
    bool CanClickOnNode(const FVector &Dest,bool IgnoreDistance = false);
    
    UPROPERTY(BlueprintReadWrite)
    FVector MoveLocation;
    UPROPERTY(BlueprintReadWrite)
    FVector HitImpactLocation;

    UFUNCTION(BlueprintImplementableEvent)
    void PlayInteractionSound(FVector Location);
    
    UPROPERTY(EditAnywhere, Category="Pickup Variants")
    TMap<EPickupType, UPickupVariantAsset*> PickupVariantMap;

    UFUNCTION(BlueprintImplementableEvent)
    void HighlightNearByNodes();
    UFUNCTION(BlueprintImplementableEvent)
    void DisableHighlightEffect();
    UFUNCTION(BlueprintCallable)
    void InvokeDisableHiglightEffectThroughBp();

    UFUNCTION(BlueprintCallable)
    void PickupAsyncLoad(TSoftClassPtr<APickup> pickupClass);

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    TMap<EPickupType,int32> PickupCounts;

    UFUNCTION(BlueprintCallable)
    void UpdateInventoryAmmo(EPickupType PickupType, int32 Amount);
    UFUNCTION(BlueprintCallable)
    void BroadCastGunUnequip(EPickupType InPickUpType);
    
    UPROPERTY(EditAnywhere, Category = "UI")
    TSubclassOf<UUserWidget> GameCompletedWidget;

    UPROPERTY(BlueprintReadOnly)
    TSoftClassPtr<AActor> ShotGun;
    
    UPROPERTY(BlueprintReadWrite)
    AActor* SpawnedShotGun;

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
    void HandlePickupEquipped(APickup* Pickup,FName SocketName, EEquipState InEquipState,FVector PickupScale);
    UFUNCTION()
    void OnThrowableLoaded();
    UFUNCTION()
    void PickupAsyncLoaded(TSoftClassPtr<APickup> loadedPickup);
  //  UFUNCTION()
    //void HandleGunUnequipDelegate(EEquipState InEquipState,EPickupType InPickUpType);
    
};
