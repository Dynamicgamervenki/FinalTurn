// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Zack.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InteractInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/Stone.h"
#include "Pickups/ThrowableItem.h"
#include "Components/PawnNoiseEmitterComponent.h"

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

void AZack::OnInteract()
{
	FHitResult Hit;
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	PlayerController->GetHitResultUnderCursorForObjects(ObjectTypes,true,Hit);
	if (Hit.bBlockingHit && CanClickNode)
	{
		PlayInteractionSound(Hit.ImpactPoint);
		if( Hit.GetActor()->Implements<UInteractInterface>())
		{
			GEngine->AddOnScreenDebugMessage(123,2.0f,FColor::Yellow,FString::Printf(TEXT("HitActor Implements InteractInterface")));
			MoveLocation = IInteractInterface::Execute_InteractPosition(Hit.GetActor());
			PerformEquipStateAction(EquipState,MoveLocation,Hit.GetActor());
		}
		else
		{
			MoveLocation = Hit.GetActor()->GetActorLocation();
			DoMoveTo(MoveLocation);
		}
	}
}

void AZack::EquipWeapon()
{
	if (PickupItem != nullptr && EquipState != EEquipState::Gun)
	{
		PickupItem->SetActorHiddenInGame(false);
		PlayAnimMontages(DrawGunMontage);
		EquipState = EEquipState::Gun;
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld,true);
		PickupItem->AttachToComponent(GetMesh(), TransformRules,"GunE_Socket");
		IsGunAiming = true;
	}
}

void AZack::EquipPickUp(TSoftClassPtr<APickup> InPickUpClass, FName SocketName, EEquipState InEquipState)
{
	if (EquipState == InEquipState || !InPickUpClass.IsValid())
	{
		GEngine->AddOnScreenDebugMessage(50, 2.0f, FColor::Green, TEXT("EquipState "));
		if (EquippedItem)
		{
			EquippedItem->Destroy();
			EquippedItem = nullptr;
		}
		EquipState = EEquipState::None;
		return;
	}

	if (!HasAmmoForEquipState(InEquipState))
	{
		GEngine->AddOnScreenDebugMessage(51, 2.0f, FColor::Green, TEXT("HasNoAmmo"));
		EquipState = EEquipState::None;
		return;
	}

	if (EquipState == EEquipState::Gun)
	{
		PlayAnimMontageInReverse(DrawGunMontage);
		if (PickupItem)
		{
			PickupItem->SetActorHiddenInGame(true);
		}
	}

	GEngine->AddOnScreenDebugMessage(53, 2.0f, FColor::Green, TEXT("Aync Abt To Start"));

	//  Async load the pickup class
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		InPickUpClass.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(
			this, &AZack::OnPickupClassLoaded, InPickUpClass, SocketName, InEquipState
		)
	);

	//  Preload the corresponding throwable class too
	TSoftClassPtr<AThrowableItem> SoftThrowableClass;
	switch (InEquipState)
	{
	case EEquipState::Stone:
		SoftThrowableClass = ThrowableStoneClass;
		break;
	case EEquipState::Grenade:
		SoftThrowableClass = ThrowableGrenadeClass;
		break;
	case EEquipState::Dynamite:
		SoftThrowableClass = ThrowableDynamiteClass;
	default:
		break;
	}

	if (!SoftThrowableClass.IsValid())
	{
		Streamable.RequestAsyncLoad(
			SoftThrowableClass.ToSoftObjectPath(),
			FStreamableDelegate(),
			0,
			false,
			false,
			FString::Printf(TEXT("Preloading Throwable for %s"), *UEnum::GetValueAsString(InEquipState))
		);
	}
}


void AZack::OnPickupClassLoaded(TSoftClassPtr<APickup> LoadedClass, FName SocketName, EEquipState InEquipState)
{
	if (!LoadedClass.IsValid()) return;

	UClass* ActualClass = LoadedClass.Get();

	FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
	FRotator SocketRotation = GetMesh()->GetSocketRotation(SocketName);

	APickup* Pickup = GetWorld()->SpawnActor<APickup>(ActualClass, SocketLocation, SocketRotation);
	if (!Pickup) return;

	Pickup->Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	SetEquippedItem(Pickup);

	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
	PlayAnimMontages(EquipStoneMontage);
	Pickup->AttachToComponent(GetMesh(), TransformRules, SocketName);

	EquipState = InEquipState;
}


bool AZack::HasAmmoForEquipState(EEquipState State)
{
	switch (State)
	{
	case EEquipState::Stone:
		return StoneCount > 0;
	case EEquipState::Grenade:
		return GranadeCount > 0;
	case EEquipState::Dynamite:
		return GranadeCount > 0;
	default:
		return false;
	}
}

void AZack::DoMoveTo(const FVector& Dest,float OffsetValue,bool IgnoreDistance)
{
	double distance = UKismetMathLibrary::Vector_Distance(Dest,GetActorLocation());
	if (distance <= moveDistance && distance > 100.0f || IgnoreDistance && distance > 50.0f || IsHiding && distance < 1000.0f) 
	{
		GEngine->AddOnScreenDebugMessage(122, 2.0f, FColor::Black, FString::Printf(TEXT("Distance: %.2f"), distance));

		FVector Direction = (Dest - GetActorLocation()).GetSafeNormal();
		FVector OffSet = Dest + Direction * OffsetValue;
		
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), OffSet);
		IsMoving = true;
		CanClickNode = false;
	}
}

void AZack::DoThrowEquipItem(const FVector& Dest, AActor* HitActor)
{
	if (!CanClickOnNode(Dest))
	{
		GEngine->AddOnScreenDebugMessage(16, 2, FColor::Red, "TRYING TO THROW AT DISTANCE > 500");
		return;
	}

	if (!HasAmmoForEquipState(EquipState))
	{
		GEngine->AddOnScreenDebugMessage(16, 3, FColor::Green, "No ammo, switching state");
		EquipState = EEquipState::None;
		DoMoveTo(Dest);

		if (EquippedItem)
		{
			EquippedItem->Destroy();
		}
		return;
	}

	// Proceed with throw
	CanClickNode = false;
	FRotator LookRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Dest);
	SetActorRotation(LookRotator);
	PlayAnimMontages(ThrowMontage);
}

void AZack::PerformEquipStateAction(EEquipState State, const FVector& InteractLocation, AActor* HitActor)
{
	switch (State)
	{
	case EEquipState::None:
		IInteractInterface::Execute_Interact(HitActor,this);
		break;
	case EEquipState::Stone:
		DoThrowEquipItem(InteractLocation,HitActor);
		break;
	case EEquipState::Grenade:
		DoThrowEquipItem(InteractLocation,HitActor);
		break;
	case EEquipState::Dynamite:
		DoThrowEquipItem(InteractLocation,HitActor);
		break;
	case EEquipState::Gun:
		DoShootAt(InteractLocation);
		break;
	}
}

void AZack::ReportNoise(AActor* NoiseMaker, float Loudness, const FVector& NoiseLocation)
{
	PawnNoiseEmitter->MakeNoise(NoiseMaker,Loudness,NoiseLocation);
}

void AZack::PlayHideMontage()
{
	PlayAnimMontages(HideMontage);
}


void AZack::HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{
	if (NotifyName != "Throw") return;
	TSoftClassPtr<AThrowableItem> SoftThrowableClass;
	
	switch (EquipState)
	{
	case EEquipState::Stone:
		SoftThrowableClass = ThrowableStoneClass;
		break;
	case EEquipState::Grenade:
		SoftThrowableClass = ThrowableGrenadeClass;
		break;
	case EEquipState::Dynamite:
		SoftThrowableClass = ThrowableDynamiteClass;
		break;
	default:
		return;
	}
	
	if (SoftThrowableClass.IsValid())
	{
		OnThrowableLoaded(SoftThrowableClass);
		GEngine->AddOnScreenDebugMessage(15,1,FColor::Red,SoftThrowableClass.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Throwable class was not loaded at throw time — preload may have failed."));
	}
}


void AZack::OnThrowableLoaded(TSoftClassPtr<AThrowableItem> LoadedClass)
{
	if (!LoadedClass.IsValid()) return;

	UClass* ActualClass = LoadedClass.Get();
	if (!ActualClass || !GetWorld()) return;

	if (EquippedItem)
	{
		EquippedItem->Destroy();
	}

	FVector SocketLocation = GetMesh()->GetSocketLocation("Stone_Socket");
	FRotator SocketRotation = GetMesh()->GetSocketRotation("Stone_Socket");

	AThrowableItem* SpawnedItem = GetWorld()->SpawnActor<AThrowableItem>(ActualClass, SocketLocation, SocketRotation);
	if (!SpawnedItem || !SpawnedItem->SM_Throwable) return;

	FVector ForwardVector = GetCapsuleComponent()->GetForwardVector();
	FVector ScaledForward = ForwardVector * 500.0f;
	float ZValue = ForwardVector.Z;
	float MappedZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(3.0f, 10.0f), ZValue);
	float UpwardImpulse = MappedZ * 100.0f;
	FVector FinalImpulse = ScaledForward + FVector(0.0f, 0.0f, UpwardImpulse);

	SpawnedItem->SM_Throwable->SetMassOverrideInKg(NAME_None, 1.0f, true);
	SpawnedItem->SM_Throwable->AddImpulse(FinalImpulse);

	SpawnedItem->OnThrowableImpact.AddDynamic(this,&AZack::HandleThrowableImpact);
		
	
	// Deduct ammo
	switch (EquipState)
	{
	case EEquipState::Stone:   StoneCount--; OnPickupUpdated.Broadcast(EPickupType::Stone, StoneCount); break;
	case EEquipState::Grenade: GranadeCount--; OnPickupUpdated.Broadcast(EPickupType::Granade, GranadeCount); break;
	case EEquipState::Dynamite: GranadeCount--; OnPickupUpdated.Broadcast(EPickupType::Granade, GranadeCount); break;
	default: break;
	}

	CanClickNode = true;
	EquipState = EEquipState::None;
}

 void AZack::DoShootAt(const FVector& Dest)
 {
	FVector Direction = (Dest - GetActorLocation()).GetSafeNormal();
	FRotator LookRotator = Direction.Rotation();
	
 	SetActorRotation(LookRotator);
	if (BulletCount > 0 && EquipState == EEquipState::Gun && CanClickOnNode(Dest))
	{
 		GEngine->AddOnScreenDebugMessage(15,1,FColor::Red,"Shooting");
		FVector SocketLocation = GetMesh()->GetSocketLocation("socket_BulletSpawn");
		//AActor* Bullet = GetWorld()->SpawnActor<AActor>(BulletClass,SocketLocation,LookRotator);
		BulletCount--;
	}
	else
	{
		IsGunAiming = false;
		GEngine->AddOnScreenDebugMessage(234,2.0f,FColor::Red,"No Bullets Left");
		EquipState = EEquipState::None;
		DoMoveTo(Dest);
	}
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
	int32 NewAmount = 0;
	
	switch (PickupType)
	{
	case EPickupType::Bullet:
		BulletCount += Amount;
		NewAmount = BulletCount;
		break;
	case EPickupType::Stone:
		StoneCount += Amount;
		NewAmount = StoneCount;
		break;
	case EPickupType::Granade:
		GranadeCount += Amount;
		NewAmount = GranadeCount;
		break;
	};
	OnPickupUpdated.Broadcast(PickupType,NewAmount);
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
		GEngine->AddOnScreenDebugMessage(8, 2.0f, FColor::Yellow, 
			FString::Printf(TEXT("Stones: %d"), StoneCount));
            
		GEngine->AddOnScreenDebugMessage(9, 2.0f, FColor::Yellow, 
			FString::Printf(TEXT("Bullets: %d"), BulletCount));
            
		GEngine->AddOnScreenDebugMessage(10, 2.0f, FColor::Yellow, 
			FString::Printf(TEXT("Grenades: %d"), GranadeCount));

		GEngine->AddOnScreenDebugMessage(13, 2.0f, FColor::Yellow,
	FString::Printf(TEXT("CanClickNode: %s"), CanClickNode ? TEXT("true") : TEXT("false")));
		
		GEngine->AddOnScreenDebugMessage(
			-3, 2.0f, FColor::Yellow,
			FString::Printf(TEXT("EquipState (int): %d"), static_cast<int32>(EquipState))
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

		AnimInstance->Montage_Play(MontageToPlay);
		AnimInstance->Montage_SetEndDelegate(EndDelegate, MontageToPlay);
	}
	else
	{
		const FString Msg = FString::Printf(
		TEXT("%s: MontageToPlay is null! Did you assign it in the editor?"),
		MontageToPlay);
	}
}

void AZack::OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
		 /*if (Montage == StealthMontage)
		{
			GEngine->AddOnScreenDebugMessage(190, 2.0f, FColor::Yellow,"StealthAnimMontageEnded");
			CanClickNode = true;
			if (!OverlappingActorsOnNode.IsEmpty())
			{
				if ( AEnemyBase* enemy = Cast<AEnemyBase>(OverlappingActorsOnNode[0]))
				{
	                enemy->GetCharacterMovement()->StopMovementImmediately();
					enemy->IsDead = true;
				}
			}
		}*/
}

void AZack::PlayAnimMontageInReverse(UAnimMontage* MontageToPlay)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && MontageToPlay)
	{
		AnimInstance->Montage_Play(MontageToPlay, -1.f);
		float MontageLength = MontageToPlay->GetPlayLength();
		AnimInstance->Montage_SetPosition(MontageToPlay, MontageLength);
	}
}

bool AZack::CanClickOnNode(const FVector& Dest)
{
	double distance = UKismetMathLibrary::Vector_Distance(Dest,GetActorLocation());
	return (distance <= moveDistance && distance > 100.0f);
}
