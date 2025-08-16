// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Zack.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Pickups/Pickup.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InteractInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "DataAssets/PickupVariantAsset.h"

AZack::AZack()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	moveDistance = 500.0f;

	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Pawm Noise Emitter"));
}


void AZack::BeginPlay()
{
	Super::BeginPlay();
	
	check(IMC_FinalTurn);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(IMC_FinalTurn,0);
		}

		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableMouseOverEvents = true;
		PlayerController->bEnableClickEvents     = true;

		// Mix game input (camera, movement) with UI/clicks
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		PlayerController->SetInputMode(InputMode);
	}
	
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
	{
		AnimInstance->OnPlayMontageNotifyBegin.AddUniqueDynamic(this, &AZack::HandleThrowMontageNotifyBegin);
	}
}

void AZack::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
			EnhancedInputComponent->BindAction(IA_Move,ETriggerEvent::Started,this,&AZack::OnInteract);
	}
}

void AZack::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PrintOutData();
}

void AZack::AddPickUpItem(APickup* Pickup)
{
	PickupActors.Add(Pickup);
}

void AZack::OnInteract()
{
	FHitResult Hit;
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	PlayerController->GetHitResultUnderCursorForObjects(InteractableObjectTypes,true,Hit);
	if (Hit.bBlockingHit && CanClickNode)
	{
		PlayInteractionSound(Hit.ImpactPoint);
		if( Hit.GetActor()->Implements<UInteractInterface>())
		{
			GEngine->AddOnScreenDebugMessage(123,2.0f,FColor::Yellow,FString::Printf(TEXT("HitActor Implements InteractInterface")));
			MoveLocation = IInteractInterface::Execute_InteractPosition(Hit.GetActor());
			HitImpactLocation = Hit.Location;
			PerformEquipStateAction(CurrentEquipState,MoveLocation,Hit.GetActor());
		}
		else if(ACharacter* Enemy = Cast<ACharacter>(Hit.GetActor()))
		{
			MoveLocation = Hit.GetActor()->GetActorLocation();
			if (CurrentEquipState == EEquipState::None)
				DoMoveTo(MoveLocation);
			else if (CurrentEquipState == EEquipState::Gun)
					ShootGun(MoveLocation,Hit.GetActor());
		}
	}
}

void AZack::PerformEquipStateAction(EEquipState State, const FVector& InteractLocation, AActor* HitActor)
{
	switch (State)
	{
	case EEquipState::None:
		IInteractInterface::Execute_Interact(HitActor,this);
		break;
	case EEquipState::Stone:
		ThrowEquippedItem(InteractLocation,HitActor);
		break;
	case EEquipState::Granade:
		ThrowEquippedItem(InteractLocation,HitActor);
		break;
	case EEquipState::Dynamite:
		ThrowEquippedItem(HitImpactLocation,HitActor);
		break;
	case EEquipState::HeavyDynamite: 
		IInteractInterface::Execute_Interact(HitActor,this);
		break;
	case EEquipState::LavaCrystal: 
		ThrowEquippedItem(InteractLocation,HitActor);
		break;
	case EEquipState::LavaOrb: 
		ThrowEquippedItem(InteractLocation,HitActor,true);
		break;
	case EEquipState::Gun:
		ShootGun(InteractLocation,HitActor);
	}
}

void AZack::Equip(EPickupType Pickup)
{
	UPickupVariantAsset* VariantAsset = PickupVariantMap.FindRef(Pickup);
	if (!VariantAsset)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, TEXT("Invalid Variant Asset or PickupClass"));
		return;
	}

	FPickupVariantData Data = VariantAsset->PickupData;
	EquipPickupFromInventory(Data);
}

void AZack::EquipPickupFromInventory(FPickupVariantData PickupData)
{
	FName SocketName = PickupData.SocketName;
	EEquipState InEquipState = PickupData.EquipState;
	CurrentPickupType = PickupData.PickupType;
	
	if (CurrentEquipState == InEquipState)//when player already in equipstate and clicked on button again without using equiiped item ,unequipping that item
	{
		FString InEquipStateMsg = FString::Printf(TEXT("InEquipState : %s"), *UEnum::GetValueAsString(InEquipState));
		FString CurrentEquipStateMsg = FString::Printf(TEXT("InEquipState : %s"), *UEnum::GetValueAsString(InEquipState));
		GEngine->AddOnScreenDebugMessage(52, 5.0f, FColor::Green, InEquipStateMsg);
		GEngine->AddOnScreenDebugMessage(53, 5.0f, FColor::Green, CurrentEquipStateMsg);
		
		if (EquippedItem)
		{
			GEngine->AddOnScreenDebugMessage(558, 5.0f, FColor::Green, TEXT("Unequipped"));
			EquippedItem->SetActorLocation(FVector(0, 0, 0));
			EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedItem = nullptr;
			DisableHighlightEffect();
		}
		CurrentEquipState = EEquipState::None;
		CurrentPickupType = EPickupType::None;
		return;
	}

	//DisableHighlightEffect();
	if (CurrentEquipState != InEquipState)
	{
		if (CurrentEquipState == EEquipState::Gun && !EquippedItem)
		{
			OnGunUnequip.AddDynamic(this,&AZack::Equip);
			UnEquipGun(CurrentPickupType);
			return;
		}
		if (EquippedItem)
		{
			EquippedItem->SetActorLocation(FVector(0, 0, 0));
			EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedItem = nullptr;
		}
	}

	if (!HasAmmoForEquipState(InEquipState))
	{
		GEngine->AddOnScreenDebugMessage(51, 2.0f, FColor::Green, TEXT("HasNoAmmo"));
		CurrentEquipState = EEquipState::None;
		CurrentPickupType = EPickupType::None;
		return;
	}
	
	APickup* Pickup = nullptr;
	for (APickup* pickup : PickupActors)
	{
		if (pickup && pickup->PickupType == PickupData.PickupType)
		{
			Pickup = pickup;
			break;
		}
	}
	if (!Pickup)
	{
		GEngine->AddOnScreenDebugMessage(58, 2.0f, FColor::Green, TEXT("Cannot Find The Equip Pickup in PickupArray"));
		return;
	}
	HighlightNearByNodes();
	HandlePickupEquipped(Pickup,SocketName, InEquipState);
}

void AZack::HandlePickupEquipped(APickup* Pickup,FName SocketName, EEquipState InEquipState)
{
	if (CurrentEquipState != EEquipState::Gun)	//Gun-State Beign Handled in Blueprints
	{
		Pickup->Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Pickup->ItemMesh->SetRenderCustomDepth(true);
		Pickup->ItemMesh->SetCustomDepthStencilValue(1);
		SetEquippedItem(Pickup);
		PlayAnimMontages(EquipStoneMontage);
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		Pickup->AttachToComponent(GetMesh(), TransformRules, SocketName);
		CurrentEquipState = InEquipState;
	}
}


bool AZack::HasAmmoForEquipState(EEquipState State)
{ 
	EPickupType PickupType = (EPickupType)State;
	const int32* Count = PickupCounts.Find(PickupType);
	return (Count && *Count > 0);
}

void AZack::InvokeDisableHiglightEffectThroughBp()
{
	DisableHighlightEffect();
}

void AZack::PickupAsyncLoad(TSoftClassPtr<APickup> pickupClass)
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		pickupClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(
			this,&AZack::PickupAsyncLoaded,pickupClass
		)
	);
}


void AZack::PickupAsyncLoaded(TSoftClassPtr<APickup> loadedPickup)
{
	UClass* LoadedUClass = loadedPickup.Get(); 
	if (!LoadedUClass) 
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to get UClass for pickup!"));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = this;

	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;

	APickup* pickup = GetWorld()->SpawnActor<APickup>(
		LoadedUClass,
		SpawnLocation,
		SpawnRotation,
		SpawnParams
	);
	if (pickup)
	{
		pickup->SetActorScale3D(FVector(2, 2, 2));
		pickup->Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AddPickUpItem(pickup);
	}
}


void AZack::DoMoveTo(const FVector& Dest,float OffsetValue,bool IgnoreDistance)
{
	double distance = UKismetMathLibrary::Vector_Distance(Dest,GetActorLocation());
	if (distance <= moveDistance && distance > 100.0f || IgnoreDistance && distance > 50.0f && distance < 750.0f || IsHiding && distance < 1000.0f) 
	{
		GEngine->AddOnScreenDebugMessage(122, 2.0f, FColor::Black, FString::Printf(TEXT("Distance: %.2f"), distance));

		FVector Direction = (Dest - GetActorLocation()).GetSafeNormal();
		FVector OffSet = Dest + Direction * OffsetValue;
		
		PreviousNodeLocation = GetActorLocation();
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), OffSet);
		IsMoving = true;
	}
}

void AZack::ThrowEquippedItem(const FVector& Dest, AActor* HitActor,bool IgnoreDistance)
{
	if (Dest.ContainsNaN())
		return;
	
	if (!CanClickOnNode(Dest,IgnoreDistance))
	{
		GEngine->AddOnScreenDebugMessage(16, 2, FColor::Red, "TRYING TO THROW AT DISTANCE > 500");
		return;
	}

	if (!HasAmmoForEquipState(CurrentEquipState))
	{
		GEngine->AddOnScreenDebugMessage(16, 3, FColor::Green, "No ammo, switching state");
		CurrentEquipState = EEquipState::None;
		DoMoveTo(Dest);

		if (EquippedItem)
		{
			EquippedItem->SetActorLocation(FVector(0, 0, 0));
			EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			EquippedItem = nullptr;
		}
		return;
	}
	
	CanClickNode = false;
	FRotator LookRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Dest);
	SetActorRotation(LookRotator);
	PlayAnimMontages(ThrowMontage);
}



void AZack::ReportNoise(AActor* NoiseMaker, float Loudness, const FVector& NoiseLocation)
{
	PawnNoiseEmitter->MakeNoise(NoiseMaker,Loudness,NoiseLocation);
}

void AZack::AmmoUpdateBroadCast(EPickupType type, int Ammo)
{
	OnPickupUpdated.Broadcast(type,Ammo);
}


void AZack::HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{
	if (NotifyName != "Throw") return;

	DisableHighlightEffect();
	OnThrowableLoaded();
}


void AZack::OnThrowableLoaded()
{
	if (!EquippedItem) return;

	double distance = UKismetMathLibrary::Vector_Distance(HitImpactLocation,GetActorLocation());
	FVector ForwardVector = GetCapsuleComponent()->GetForwardVector();
	FVector ScaledForward = ForwardVector * distance;
	float ZValue = ForwardVector.Z;
	float MappedZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(3.0f, 10.0f), ZValue);
	float UpwardImpulse = MappedZ * 100.0f;
	FVector FinalImpulse = ScaledForward + FVector(0.0f, 0.0f, UpwardImpulse);

	EquippedItem->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

	//EquippedItem->SetActorEnableCollision(true);
	
	EquippedItem->ItemMesh->SetCollisionResponseToAllChannels(ECR_Block);
	EquippedItem->ItemMesh->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	EquippedItem->ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
	EquippedItem->Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	EquippedItem->Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	
	EquippedItem->ItemMesh->SetSimulatePhysics(true);
	EquippedItem->ItemMesh->SetMassOverrideInKg(NAME_None, 1.0f, true);
	EquippedItem->ItemMesh->AddImpulse(FinalImpulse);
	
	PickupActors.Remove(EquippedItem);
	EquippedItem->Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	EquippedItem = nullptr;
	
	//EquippedItem->OnThrowableImpact.AddDynamic(this,&AZack::HandleThrowableImpact);
	
	// Deduct ammo
	UpdateInventoryAmmo(CurrentPickupType,-1);
	CanClickNode = true;
	CurrentEquipState = EEquipState::None;
	CurrentPickupType = EPickupType::None;
}


void AZack::HandleThrowableImpact(AActor* HitActor)
{
	if (!HitActor) return;
	
	GEngine->AddOnScreenDebugMessage(55, 10, FColor::Red, 
		FString::Printf(TEXT("Throwable Impact on Actor: %s"), *HitActor->GetName()));
	FVector Location = HitActor->GetActorLocation();
	ReportNoise(HitActor, 1.0f, Location);
}


void AZack::OnPickedUp(EPickupType PickupType, int32 Amount)
{
	int32& CurrentAmount = PickupCounts.FindOrAdd(PickupType);
	CurrentAmount += Amount;

	OnPickupUpdated.Broadcast(PickupType, CurrentAmount);
}

void AZack::UpdateInventoryAmmo(EPickupType PickupType, int32 Amount)
{
	if (PickupCounts.Contains(PickupType))
	{
		int32& CurrentAmount = PickupCounts[PickupType];
		CurrentAmount += Amount;
		CurrentAmount = FMath::Clamp(CurrentAmount, 0.0f, INT_MAX);
		OnPickupUpdated.Broadcast(PickupType, CurrentAmount);
	}
}

int32 AZack::GetAmmoOfState(EPickupType PickupType)
{
	if (PickupCounts.Contains(PickupType))
	{
		int32& AmmoAmount = PickupCounts[PickupType];
		return AmmoAmount;
	}
	return 0;
}



void AZack::BroadCastGunUnequip(EPickupType InPickUpType)
{
	OnGunUnequip.Broadcast(InPickUpType);
}

void AZack::SetDetectedByEnemy_Implementation(bool bDetected)
{
	GotDetectedByEnemy = bDetected;
}

void AZack::SetIsHiding_Implementation(bool isHiding)
{
	IsHiding = isHiding;
}

bool AZack::GetIsHiding_Implementation()
{
	return IsHiding;
}

void AZack::SetCanClickOnNode_Implementation(bool click)
{
	CanClickNode = click;
}

void AZack::PrintOutData()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(15, 2.0f, FColor::Yellow,
	FString::Printf(TEXT("CanClickNode: %s"), CanClickNode ? TEXT("true") : TEXT("false")));
		
		GEngine->AddOnScreenDebugMessage(
			-3, 2.0f, FColor::Yellow,
			FString::Printf(TEXT("EquipState (int): %s"), *UEnum::GetValueAsString(CurrentEquipState))
		);
		GEngine->AddOnScreenDebugMessage(
			-4, 2.0f, FColor::Yellow,
			FString::Printf(TEXT("PickupType (int): %s"), *UEnum::GetValueAsString(CurrentPickupType))
		);
	}
}

void AZack::PlayAnimMontages(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MontageToPlay)
	{
		AnimInstance->Montage_Play(MontageToPlay);
		FOnMontageEnded EndDelegate;
		EndDelegate.BindUObject(this, &AZack::OnAnimMontageEnded);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);
	}
	else
	{
		if (GEngine) GEngine->AddOnScreenDebugMessage(-183, 5.f, FColor::Red, TEXT("MontageToPlay is null! Did you assign it in the editor?"));
	}
}

void AZack::OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	// if (Montage == PlaceHeavyDynamiteMontage)
	// {
	// 	DoMoveTo(PreviousNodeLocation);
	// }
}

void AZack::PlayAnimMontageInReverse(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MontageToPlay)
	{
		float MontageLength = MontageToPlay->GetPlayLength();
		AnimInstance->Montage_Play(MontageToPlay, -1.f);
		AnimInstance->Montage_SetPosition(MontageToPlay, MontageLength);
	}
}

bool AZack::CanClickOnNode(const FVector& Dest,bool IgnoreDistance)
{
	double distance = UKismetMathLibrary::Vector_Distance(Dest,GetActorLocation());
	return (distance <= moveDistance  && distance > 100.0f || IgnoreDistance);
}
