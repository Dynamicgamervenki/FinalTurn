// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Zack.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/Node.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Breakable/BreakableActor.h"
#include "Characters/Enemies/EnemyBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InteractInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/Stone.h"
#include "Pickups/ThrowableItem.h"
AZack::AZack()
{
	GetCharacterMovement()->bOrientRotationToMovement = true;
	moveDistance = 500.0f;
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
		PlayerController->bEnableClickEvents     = true;
		PlayerController->bEnableMouseOverEvents = true;

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
			//EnhancedInputComponent->BindAction(IA_EquipWeapon,ETriggerEvent::Started,this,&AZack::EquipWeapon);
		    //EnhancedInputComponent->BindAction(IA_EquipStone,ETriggerEvent::Started,this,&AZack::EquipPickUp);
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
		// if (AEnemyBase* Enemy = Cast<AEnemyBase>(Hit.GetActor()))
		// {
		// 	GEngine->AddOnScreenDebugMessage(345,2.0f,FColor::Green,"ClickedOnCharacter");
		// 	OverlappingActorsOnNode.Add(Enemy);
		// 	 AttackEnemy(Enemy);
		// }
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Hit Actor: %s"), Hit.GetActor() ? *Hit.GetActor()->GetName() : TEXT("None")));
		if( Hit.GetActor()->Implements<UInteractInterface>())
		{
		 	if (IInteractInterface* Interact = Cast<IInteractInterface>(Hit.GetActor()))
		 	{
		 		MoveLocation  = Interact->InteractPosition();
		 		OverlappingActorsOnNode = Interact->GetOverlappingActorsOnNode(); 		
		 		switch (EquipState)
		 		{
		 		case EEquipState::None:
		 			if (ABreakableActor* breakableActor = Cast<ABreakableActor>(Hit.GetActor()))
		 			{
		 				GEngine->AddOnScreenDebugMessage(123,2.0f,FColor::Red,FString::Printf(TEXT("Clicking On BreakableActor without equping granade")));
		 			}
				    else
				    {
				    	if (ANode* node = Cast<ANode>(Hit.GetActor()))
				    	{
				    		bool temp = node->DontGoToNode();
				    		if (!temp)
				    			DoMoveTo(MoveLocation);
				    	}

				    }
		 			break;
		 		case EEquipState::Stone:
		 			DoThrowEquipItem(MoveLocation,Hit.GetActor());
		 			break;

		 		case EEquipState::Grenade:
		 			DoThrowEquipItem(MoveLocation,Hit.GetActor());
		 			break;

		 		case EEquipState::Gun:
		 			DoShootAt(MoveLocation);
		 				break;
		 		}
		 	}
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


/*void AZack::EquipPickUp(TSubclassOf<APickup> InPickUpClass , FName SocketName , EEquipState InEquipState)
{
	if (EquipState != EEquipState::Stone)
	{
		if (EquipState == EEquipState::Gun)
		{
		   PlayAnimMontageInReverse(DrawGunMontage);
			PickupItem->SetActorHiddenInGame(true);
		}
		FVector SocketLocation = GetMesh()->GetSocketLocation("Stone_Socket");
		FRotator SocketRotation = GetMesh()->GetSocketRotation("Stone_Socket");
		APickup* Stone = GetWorld()->SpawnActor<APickup>(InPickUpClass,SocketLocation,SocketRotation);
		Stone->Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
		SetEquippedItem(Stone);
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld,true);
		PlayAnimMontages(EquipStoneMontage);
		Stone->AttachToComponent(GetMesh(), TransformRules,"Stone_Socket");
		EquipState = InEquipState;
	}
	else
	{
		EquipState = EEquipState::None;
	}
}*/

void AZack::EquipPickUp(TSubclassOf<APickup> InPickUpClass, FName SocketName, EEquipState InEquipState)
{
	if (EquipState == InEquipState || !InPickUpClass)
	{
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
		EquipState = EEquipState::None;
		return;
	}

	if (EquipState != InEquipState)
	{
		if (EquipState == EEquipState::Gun)
		{
			PlayAnimMontageInReverse(DrawGunMontage);
			PickupItem->SetActorHiddenInGame(true);
		}

		FVector SocketLocation = GetMesh()->GetSocketLocation(SocketName);
		FRotator SocketRotation = GetMesh()->GetSocketRotation(SocketName);
		APickup* Pickup = GetWorld()->SpawnActor<APickup>(InPickUpClass, SocketLocation, SocketRotation);
		
		if (!Pickup) return;

		Pickup->Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		SetEquippedItem(Pickup);

		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true);
		PlayAnimMontages(EquipStoneMontage);
		Pickup->AttachToComponent(GetMesh(), TransformRules, SocketName);

		EquipState = InEquipState;
	}
}

bool AZack::HasAmmoForEquipState(EEquipState State)
{
	switch (State)
	{
	case EEquipState::Stone:
		return StoneCount > 0;
	case EEquipState::Grenade:
		return GranadeCount > 0;
	default:
		return false;
	}
}



void AZack::EquipGranade()
{
	
}

void AZack::DoMoveTo(const FVector& Dest)
{
	double distance = UKismetMathLibrary::Vector_Distance(Dest,GetActorLocation());
	if (distance <= moveDistance && distance > 100.0f /*&& OverlappingActorsOnNode.IsEmpty()*/) 
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Dest);
		IsMoving = true;
		CanClickNode = false;
	}
}

/*
void AZack::DoThrowEquipItem(const FVector& Dest,AActor* HitActor)
{
	if (GranadeCount > 0 && EquipState == EEquipState::Grenade && CanClickOnNode(Dest))
	{
		CanClickNode = false;
		FRotator lookRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),Dest);
		SetActorRotation(lookRotator);
		PlayAnimMontages(ThrowMontage);
	}
	else if (!CanClickOnNode(Dest))
	{
		GEngine->AddOnScreenDebugMessage(16,2,FColor::Red,"TRYING TO THROW AT DISTANCE > 500");
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(16,3,FColor::Green,"Stone count 0 , switching state");
		EquipState = EEquipState::None;
		DoMoveTo(Dest);
		if (EquippedItem != nullptr)
		{
			EquippedItem->Destroy();
		}
	}
}
*/

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

void AZack::HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{
	if (NotifyName != "Throw") return;

	FVector SocketLocation = GetMesh()->GetSocketLocation("Stone_Socket");
	FRotator SocketRotation = GetMesh()->GetSocketRotation("Stone_Socket");

	TSubclassOf<AThrowableItem> ThrowableClass = nullptr;

	switch (EquipState)
	{
	case EEquipState::Stone:
		ThrowableClass = ThrowableStoneClass;
		break;
	case EEquipState::Grenade:
		ThrowableClass = ThrowableGrenadeClass;
		break;
	default:
		break;
	}

	if (ThrowableClass && GetWorld())
	{
		if (EquippedItem)
		{
			EquippedItem->Destroy();
		}

		AThrowableItem* SpawnedItem = GetWorld()->SpawnActor<AThrowableItem>(ThrowableClass, SocketLocation, SocketRotation);
		if (SpawnedItem && SpawnedItem->SM_Throwable)
		{
			FVector ForwardVector = GetCapsuleComponent()->GetForwardVector();
			FVector ScaledForward = ForwardVector * 500.0f;
			float ZValue = ForwardVector.Z;
			float MappedZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(3.0f, 10.0f), ZValue);
			float UpwardImpulse = MappedZ * 100.0f;
			FVector FinalImpulse = ScaledForward + FVector(0.0f, 0.0f, UpwardImpulse);

			SpawnedItem->SM_Throwable->SetMassOverrideInKg(NAME_None, 1.0f, true);
			SpawnedItem->SM_Throwable->AddImpulse(FinalImpulse);
		}

		// Deduct ammo
		switch (EquipState)
		{
		case EEquipState::Stone:   StoneCount--; break;
		case EEquipState::Grenade: GranadeCount--; break;
		default: break;
		}

		CanClickNode = true;
		EquipState = EEquipState::None;
	}
}





/*
void AZack::HandleThrowMontageNotifyBegin(FName NotifyName, const FBranchingPointNotifyPayload& BranchingPayload)
{
	if (NotifyName == "Throw")
	{
		FVector SocketLocation = GetMesh()->GetSocketLocation("Stone_Socket");
		FRotator SocketRotation = GetMesh()->GetSocketRotation("Stone_Socket");

		if (ThrowableStoneClass && GetWorld())
		{
			if (EquippedItem != nullptr)
			{
				EquippedItem->Destroy();
			}
			AThrowableStone* SpawnedStone = GetWorld()->SpawnActor<AThrowableStone>(ThrowableStoneClass, SocketLocation, SocketRotation);
			FVector ForwardVector = GetCapsuleComponent()->GetForwardVector();
			FVector ScaledForward = ForwardVector * 500.0f;
			float ZValue = ForwardVector.Z;
			float MappedZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(3.0f, 10.0f), ZValue);
			float UpwardImpulse = MappedZ * 100.0f;

			FVector FinalImpulse = ScaledForward + FVector(0.0f, 0.0f, UpwardImpulse);
			SpawnedStone->SM_Stone->SetMassOverrideInKg(NAME_None, 1.0f, true);
			SpawnedStone->SM_Stone->AddImpulse(FinalImpulse);
			StoneCount--;
			CanClickNode = true;
			EquipState = EEquipState::None;
		}
	}
}
*/

void AZack::DoThrowGrenadeAt(const FVector& Dest)
{
	GEngine->AddOnScreenDebugMessage(15,1,FColor::Red,"Throwing Grenade");
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
		AActor* Bullet = GetWorld()->SpawnActor<AActor>(BulletClass,SocketLocation,LookRotator);
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


void AZack::OnPickedUp(EPickupType PickupType, int32 Amount)
{
	switch (PickupType)
	{
	case EPickupType::Bullet:
		BulletCount += Amount;
		break;
	case EPickupType::Stone:
		StoneCount += Amount;
		break;
	case EPickupType::Granade:
		GranadeCount += Amount;
		break;
	};
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

		GEngine->AddOnScreenDebugMessage(11, 2.0f, FColor::Green, TEXT("Press E to Equip Gun"));
		GEngine->AddOnScreenDebugMessage(12, 2.0f, FColor::Green, TEXT("Press F to Equip Stone"));

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
		if (Montage == EquipStoneMontage && EquippedItem)
		{
			//EquippedItem->Destroy();
		}
		else if (Montage == StealthMontage)
		{
			GEngine->AddOnScreenDebugMessage(190, 2.0f, FColor::Yellow,"StealthAnimMontageEnded");
			CanClickNode = true;
			if (!OverlappingActorsOnNode.IsEmpty())
			{
				if ( AEnemyBase* enemy = Cast<AEnemyBase>(OverlappingActorsOnNode[0]))
				{
	                enemy->GetCharacterMovement()->StopMovementImmediately();
					enemy->IsDead = true;
					//OverlappingActorsOnNode[0]->Destroy();
				}
			}
		}
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

void AZack::AttackEnemy(AActor* actor)
{
	FVector Dest = actor->GetActorLocation();
	TargetEnemyLocation = Dest;
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Dest);
	IsMoving = true;
	CanClickNode = false;
}
