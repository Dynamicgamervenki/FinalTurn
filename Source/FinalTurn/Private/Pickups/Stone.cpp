// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Stone.h"

#include "Characters/Player/Zack.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"

AStone::AStone()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStone::BeginPlay()
{
	Super::BeginPlay();
}

void AStone::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AStone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPickupInterface	>())
	{
		IPickupInterface* Pickup = Cast<IPickupInterface>(OtherActor);
		if (Pickup && OtherActor)
		{
			Pickup->OnPickedUp(EPickupType::Stone,StoneCount);
			Pickup->AddToPickupArray(this);
			this->SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
			Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AStone::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}
