// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "Actors/Node.h"
#include "Characters/Player/Zack.h"
#include "Components/SphereComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Pickups/Pickup.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetNotifyBreaks(true);
	GeometryCollection->OnChaosBreakEvent.AddDynamic(this,&ABreakableActor::OnGeometryCollectionBreak);
	GeometryCollection->SetCanEverAffectNavigation(false);
	
	SphereCollision = CreateDefaultSubobject<USphereComponent>("BoxComponent");
	SphereCollision->SetupAttachment(GeometryCollection);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this,&ABreakableActor::OnBoxOverlap);
	SphereCollision->ShapeColor = FColor(0, 197, 255, 255);
	SphereCollision->SetSphereRadius(100.0f);

} 

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	if (NodeToMoveAfterDestroyingBreakable)
		NodeToMoveAfterDestroyingBreakable->bCannotMoveToNode = true;
}

void ABreakableActor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AZack* Zack = Cast<AZack>(OtherActor))
	{
		HandleZackOverlap(Zack);
	}
	else if (APickup* Pickup = Cast<APickup>(OtherActor))
	{
		HandlePickupOverlap(Pickup);
	}
}

void ABreakableActor::Interact_Implementation(AActor* Interactor)
{
	if (AZack* Zack = Cast<AZack>(Interactor))
	{
		GEngine->AddOnScreenDebugMessage(-12, 5.f, FColor::Red, TEXT("Interact from BreakableActor"));
		FVector MoveToLocation;
		if (Zack->CurrentEquipState == EEquipState::HeavyDynamite && HeavydynamitePlacingPositionActor)
		{
			if (bStopBeforeUnits)
			{
				FVector BoxLocation = HeavydynamitePlacingPositionActor->GetActorLocation();
				FVector PlayerLocation = Zack->GetActorLocation();
				FVector Direction = (BoxLocation - PlayerLocation).GetSafeNormal();
				MoveToLocation = BoxLocation - Direction * UnitsBeforeStop;
			}
			else
			{
				MoveToLocation = GetActorLocation();
			}
			Zack->DoMoveTo(MoveToLocation,0,true);
		}
	}
}

void ABreakableActor::HandleZackOverlap(AZack* Zack)
{
	if (bPlaceHeavyDynamiteOnClick && Zack->CurrentEquipState == EEquipState::HeavyDynamite)
	{
		Zack->BreakableActor = this;
		Zack->HeavyDynamiteSpawnLocation = HeavydynamitePlacingPositionActor->GetActorLocation();
		Zack->PlayPlacignHeavyDynamiteMontage();
	}
}

void ABreakableActor::HandlePickupOverlap(APickup* Pickup)
{
	bool bIsCorrectPickupType = false;
	for (EPickupType type : DestoroyablePickup)
	{
		if (type == Pickup->PickupType)
		{
			bIsCorrectPickupType = true;
			break;
		}
	}
	if (!bIsCorrectPickupType)
	{
		GEngine->AddOnScreenDebugMessage(212, 5.f, FColor::Red, TEXT("Trying To Destroy With Wrong Pickup"));
		return;
	}
	Hit++;
	if (Hit == AmountToGetDestoryed)
	{
		Pickup->Field(GetActorLocation());
		SetLifeSpan(3.0f);
	}
}

void ABreakableActor::Glow_Implementation()
{
	GeometryCollection->SetRenderCustomDepth(true);
	GeometryCollection->SetCustomDepthStencilValue(1);
}

void ABreakableActor::ResetGlow_Implementation()
{
	GeometryCollection->SetRenderCustomDepth(false);
	GeometryCollection->SetCustomDepthStencilValue(0);
}

void ABreakableActor::OnGeometryCollectionBreak(const FChaosBreakEvent& BreakEvent)
{
	if (NodeToMoveAfterDestroyingBreakable)
		NodeToMoveAfterDestroyingBreakable->bCannotMoveToNode = false;
	GeometryCollection->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	SetLifeSpan(1.0f);
}

FVector ABreakableActor::InteractPosition_Implementation()
{
		return GetActorLocation();
}

TArray<AActor*> ABreakableActor::GetOverlappingActorsOnNode_Implementation()
{
	TArray<AActor*> OverlappingActors;
	return OverlappingActors;
}

void ABreakableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

