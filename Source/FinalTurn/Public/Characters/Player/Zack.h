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

UENUM(BlueprintType)
enum class EEquipState : uint8
{
    None     UMETA(DisplayName = "None"),
    Stone    UMETA(DisplayName = "Stone"),
    Grenade  UMETA(DisplayName = "Grenade"),
    Gun      UMETA(DisplayName = "Gun")
};

UCLASS()
class FINALTURN_API AZack : public ACharacter, public IPickupInterface
{
    GENERATED_BODY()

public:
    AZack();

    // --- Unreal Overrides ---
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;

    // --- Pickup Interface ---
    virtual void OnPickedUp(EPickupType PickupType, int32 Amount) override;
    
    /** Is character currently moving? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move")
    bool IsMoving = false;

    /** Can click nodes? */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    bool CanClickNode = true;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Life")
    bool IsAlive = true;

    // --- Inline Setters for Pickup Items ---
    FORCEINLINE void SetPickupItem(APickup* Pickup)      { PickupItem = Pickup; }
    FORCEINLINE void SetEquippedItem(APickup* Equipped) { EquippedItem = Equipped; }

    // --- Blueprint Event for Shooting ---
    UFUNCTION(BlueprintImplementableEvent)
    void DoShootAt(const FVector& Dest);

protected:
    // --- Input Handling ---
    UFUNCTION(BlueprintCallable) void OnInteract();
    UFUNCTION(BlueprintCallable) void EquipWeapon();
    UFUNCTION(BlueprintCallable) void EquipStone();
    UFUNCTION(BlueprintCallable) void EquipGranade();

    // --- Movement & Actions ---
    void DoMoveTo(const FVector& Dest);
    void DoThrowStoneAt(const FVector& Dest);
    void DoThrowGrenadeAt(const FVector& Dest);

    // --- Animation Notifications ---
    UFUNCTION() void HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& Payload);

    // --- Debug/Utility ---
    void PrintOutData();

    // --- Internal State ---
    EEquipState EquipState = EEquipState::None;

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
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> IA_EquipWeapon;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> IA_EquipStone;
    UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> IA_EquipGranade;

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
    UPROPERTY(EditAnywhere) UClass* ThrowableStoneClass;
    UPROPERTY(EditAnywhere) UClass* StoneClass;
    UPROPERTY(EditAnywhere) UClass* BulletClass;

    // --- Animation Montages ---
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* ThrowMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* DrawGunMontage;
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* EquipStoneMontage;

    UFUNCTION(BlueprintCallable)
    bool CanClickOnNode(const FVector &Dest);
private:
    UFUNCTION()
    void PlayAnimMontages(UAnimMontage* MontageToPlay);
    UFUNCTION()
    virtual void OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted);
    UFUNCTION()
    void PlayAnimMontageInReverse(UAnimMontage* MontageToPlay);

    TArray<AActor*> OverlappingActorsOnNode;

};
