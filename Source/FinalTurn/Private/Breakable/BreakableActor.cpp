// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
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

	SphereCollision = CreateDefaultSubobject<USphereComponent>("BoxComponent");
	SphereCollision->SetupAttachment(GeometryCollection);
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this,&ABreakableActor::OnBoxOverlap);
	SphereCollision->ShapeColor = FColor(0, 197, 255, 255);
	SphereCollision->SetSphereRadius(100.0f);
} 

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AZack* Zack = Cast<AZack>(OtherActor))
	{
		if (bPlaceHeavyDynamiteOnClick && Zack->EquipState == EEquipState::HeavyDynamite)
		{
			Zack->BreakableActor = this;
			Zack->HeavyDynamiteSpawnLocation = HeavydynamitePlacingPositionActor->GetActorLocation();
			Zack->PlayPlacignHeavyDynamiteMontage();
		}
	}
	else if (APickup* Pickup = Cast<APickup>(OtherActor))
	{
		Hit++;
		if (Hit == AmountToGetDestoryed)
		{
			Pickup->Field(GetActorLocation());	
			SetLifeSpan(2.0f);
		}
	}
}

void ABreakableActor::Interact_Implementation(AActor* Interactor)
{
	if (AZack* Zack = Cast<AZack>(Interactor))
	{
		GEngine->AddOnScreenDebugMessage(-12, 5.f, FColor::Red, TEXT("Interact from BreakableActor"));
		FVector MoveToLocation;
		if (Zack->EquipState == EEquipState::HeavyDynamite && HeavydynamitePlacingPositionActor)
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

