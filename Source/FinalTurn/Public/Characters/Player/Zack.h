#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/PickupInterface.h"
#include "Zack.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class UInputAction;
class APickup;
class UAnimMontage;
class AThrowableItem;

UENUM(BlueprintType)
enum class EEquipState : uint8
{
    None     UMETA(DisplayName = "None"),
    Stone    UMETA(DisplayName = "Stone"),
    Grenade  UMETA(DisplayName = "Grenade"),
    Dynamite UMETA(DisplayName = "Dynamite"),
    Gun      UMETA(DisplayName = "Gun")
};

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
    virtual ACharacter* GetZackReference_Implementation() override;
    virtual void SetDetectedByEnemy_Implementation(bool bDetected) override;
    
    /** Is character currently moving? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    bool IsMoving = false;

    /** Can click nodes? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool CanClickNode = true;
    virtual void SetCanClickOnNode_Implementation(bool click) override;
    
    /** Detected By Enemy? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    bool IsAlive = true;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    bool GotDetectedByEnemy = false;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool IsGunAiming = false;
    
    // --- Inline Setters for Pickup Items ---
    FORCEINLINE void SetPickupItem(APickup* Pickup)      { PickupItem = Pickup; }
    FORCEINLINE void SetEquippedItem(APickup* Equipped) { EquippedItem = Equipped; }
    
    void DoMoveTo(const FVector& Dest,float Offset = 20);
    
    // --- Internal State ---
    EEquipState EquipState = EEquipState::None;
    void DoThrowEquipItem(const FVector& Dest,AActor* HitActor);

    void PerformEquipStateAction(EEquipState State, const FVector& InteractLocation, AActor* HitActor);

    UFUNCTION(BlueprintCallable)
    void ReportNoise(AActor* NoiseMaker, float Loudness, const FVector& NoiseLocation);

protected:
    // --- Input Handling ---
    UFUNCTION(BlueprintCallable) void OnInteract();
    UFUNCTION(BlueprintCallable) void EquipWeapon();
    UFUNCTION(BlueprintCallable) void EquipPickUp(TSoftClassPtr<APickup> InPickUpClass,FName SocketName,EEquipState InEquipState);

    // --- Movement & Actions ---
    void DoShootAt(const FVector& Dest);

    // --- Animation Notifications ---
    UFUNCTION() void HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

    // --- Debug/Utility ---
    void PrintOutData();


    // --- Movement Data ---
    UPROPERTY(BlueprintReadWrite, Category = "Move")
    FVector TargetLocation;
    
    UPROPERTY(EditAnywhere, Category = "Move")
    double moveDistance;

    UPROPERTY(EditAnywhere, Category = "Move")
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;

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
    int32 BulletCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 StoneCount;
    UPROPERTY(BlueprintReadWrite, Category = "Pickups")
    int32 GranadeCount;

    // --- Projectile Classes ---
  //  UPROPERTY(EditAnywhere) UClass* ThrowableStoneClass;
    //UPROPERTY(EditAnywhere) UClass* ThrowableGrenadeClass;
    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableStoneClass;

    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableGrenadeClass;

    UPROPERTY(EditAnywhere)
    TSoftClassPtr<AThrowableItem> ThrowableDynamiteClass;
    
    UPROPERTY(EditAnywhere) UClass* PickUpClass;
   // UPROPERTY(EditAnywhere) UClass* BulletClass;

    // --- Animation Montages ---
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* ThrowMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* DrawGunMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* EquipStoneMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* StealthMontage;

    UFUNCTION(BlueprintCallable)
    bool CanClickOnNode(const FVector &Dest);

    UPROPERTY(BlueprintReadWrite)
    FVector MoveLocation;
    
    UFUNCTION(BlueprintImplementableEvent)
    void PlayInteractionSound(FVector Location);

    UFUNCTION()
    bool HasAmmoForEquipState(EEquipState State);

    UFUNCTION()
    void OnPickupClassLoaded(TSoftClassPtr<APickup> LoadedClass, FName SocketName, EEquipState InEquipState);

    UFUNCTION()
    void OnThrowableLoaded(TSoftClassPtr<AThrowableItem> LoadedClass);

    UPROPERTY()
    TArray<AThrowableItem*> Throwables;

    
private:
    UFUNCTION()
    void PlayAnimMontages(UAnimMontage* MontageToPlay);
    UFUNCTION()
    virtual void OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    UFUNCTION()
    void PlayAnimMontageInReverse(UAnimMontage* MontageToPlay);
    UFUNCTION()
    void HandleThrowableImpact(AActor* HitActor);

};
