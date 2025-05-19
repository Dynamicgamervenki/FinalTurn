// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/Player/Zack.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interfaces/InteractInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Pickups/Stone.h"
#include "Pickups/ThrowableStone.h"
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

		// Let the controller generate click/hover events on actors
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
			EnhancedInputComponent->BindAction(IA_EquipWeapon,ETriggerEvent::Started,this,&AZack::EquipWeapon);
		    EnhancedInputComponent->BindAction(IA_EquipStone,ETriggerEvent::Started,this,&AZack::EquipStone);
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
	//GEngine->AddOnScreenDebugMessage(123,3.0f,FColor::Magenta,Hit.GetActor()->GetName());
	if (Hit.bBlockingHit && CanClickNode)
	{
		//bool bHitIsCharacter = Hit.GetActor() && Hit.GetActor()->IsA(ACharacter::StaticClass());
		 if( Hit.GetActor()->Implements<UInteractInterface>() /*|| bHitIsCharacter*/ )
		 {
		 	if (IInteractInterface* Interact = Cast<IInteractInterface>(Hit.GetActor()))
		 	{
		 		FVector MoveLocation  = Interact->InteractPosition();
		 		OverlappingActorsOnNode = Interact->GetOverlappingActorsOnNode();
		 		switch (EquipState)
		 		{
		 		case EEquipState::None:
		 			DoMoveTo(MoveLocation);
		 			break;

		 		case EEquipState::Stone:
		 			DoThrowStoneAt(MoveLocation);
		 			break;

		 		case EEquipState::Grenade:
		 			DoThrowGrenadeAt(MoveLocation);
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
		if (EquipState == EEquipState::Stone)
		{
			PlayAnimMontageInReverse(EquipStoneMontage);
			if (EquippedItem != nullptr)
			{
				EquippedItem->Destroy();
			}
		}
		PickupItem->SetActorHiddenInGame(false);
		PlayAnimMontages(DrawGunMontage);
		EquipState = EEquipState::Gun;
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld,true);
		PickupItem->AttachToComponent(GetMesh(), TransformRules,"GunE_Socket");
	}
	else
	{
		// EquipState = EEquipState::None;
		// PlayAnimMontageInReverse(DrawGunMontage);
		// if (PickupItem)
		// PickupItem->SetActorHiddenInGame(true);
	}
}


void AZack::EquipStone()
{
	if (StoneCount > 0 && EquipState != EEquipState::Stone)
	{
		if (EquipState == EEquipState::Gun)
		{
		   PlayAnimMontageInReverse(DrawGunMontage);
			PickupItem->SetActorHiddenInGame(true);
		}
		FVector SocketLocation = GetMesh()->GetSocketLocation("Stone_Socket");
		FRotator SocketRotation = GetMesh()->GetSocketRotation("Stone_Socket");
		AStone* Stone = GetWorld()->SpawnActor<AStone>(StoneClass,SocketLocation,SocketRotation);
		SetEquippedItem(Stone);
		FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::KeepWorld,true);
		PlayAnimMontages(EquipStoneMontage);
		Stone->AttachToComponent(GetMesh(), TransformRules,"Stone_Socket");
		EquipState = EEquipState::Stone;
	}
	else
	{
		// GEngine->AddOnScreenDebugMessage(12,2,FColor::Green,"UnEquipped Stone");
		// EquipState = EEquipState::None;
		// UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		// if (EquippedItem != nullptr && AnimInstance != nullptr)
		// {
		// 	PlayAnimMontageInReverse(EquipStoneMontage);
		// 	AnimInstance->OnMontageEnded.AddDynamic(this,&AZack::OnAnimMontageEnded);();
		// }
	}
}

void AZack::EquipGranade()
{
	
}

void AZack::DoMoveTo(const FVector& Dest)
{
	 bool bHasOverlapCharacter;
	if (!OverlappingActorsOnNode.IsEmpty())
	{
		bHasOverlapCharacter = OverlappingActorsOnNode.ContainsByPredicate(
		[](AActor* Actor)
		{
			GEngine->AddOnScreenDebugMessage(1,2,FColor::Green,Actor->GetName());
			return Actor && Actor->IsA(ACharacter::StaticClass());
		});
	}
	
	double distance = UKismetMathLibrary::Vector_Distance(Dest,GetActorLocation());
	if (distance <= moveDistance && distance > 100.0f /*&& !bHasOverlapCharacter*/) 
	{
		FVector AdjustedDest = Dest + FVector(50.0f, 0.0f, 0.0f);
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), Dest);
		IsMoving = true;
		CanClickNode = false;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(16,2,FColor::Red,"Distance > 200 units");
	}
}

void AZack::DoThrowStoneAt(const FVector& Dest)
{
	if (StoneCount > 0 && EquipState == EEquipState::Stone && CanClickOnNode(Dest))
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
			FVector ScaledForward = ForwardVector * 600.0f;
			float ZValue = ForwardVector.Z;
			float MappedZ = FMath::GetMappedRangeValueClamped(FVector2D(0.0f, 1.0f), FVector2D(3.0f, 10.0f), ZValue);
			float UpwardImpulse = MappedZ * 100.0f;


			FVector FinalImpulse = ScaledForward + FVector(0.0f, 0.0f, UpwardImpulse);
			SpawnedStone->SM_Stone->AddImpulse(FinalImpulse);
			StoneCount--;
			CanClickNode = true;
			EquipState = EEquipState::None;
		}
		else if (NotifyName == "canWalk")
		{
			//soon
		}
	}
}

void AZack::DoThrowGrenadeAt(const FVector& Dest)
{
	GEngine->AddOnScreenDebugMessage(15,1,FColor::Red,"Throwing Grenade");
}


// void AZack::DoShootAt(const FVector& Dest)
// {
// 	GEngine->AddOnScreenDebugMessage(15,1,FColor::Red,"Shooting");
// 	FRotator lookRotator = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),Dest);
// 	SetActorRotation(lookRotator);
//
// 	FVector SocketLocation = GetMesh()->GetSocketLocation("Stone_Socket");
// 	FRotator SocketRotation = GetMesh()->GetSocketRotation("Stone_Socket");
// 	
// }


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

// Zack.cpp


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
	}
	else
	{
		const FString Msg = FString::Printf(
		TEXT("%s: MontageToPlay is null! Did you assign it in the editor?"),
		*GetName()
	);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Msg);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				/*Key=*/ -1, /*Time=*/ 5.f, /*Color=*/ FColor::Orange,
				Msg
			);
		}
	}
}

void AZack::OnAnimMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
		if (Montage == EquipStoneMontage && EquippedItem)
		{
			EquippedItem->Destroy();
			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				AnimInstance->OnMontageEnded.RemoveDynamic(this, &AZack::OnAnimMontageEnded);
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
