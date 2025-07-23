// Fill out your copyright notice in the Description page of Project Settings.


#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;
	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>("GeometryCollection");
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
} 

void ABreakableActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABreakableActor::Interact_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-12, 5.f, FColor::Red, TEXT("Interact from Breakable null"));
	
}

FVector ABreakableActor::InteractPosition_Implementation()
{
	if (!bPlaceHeavyDynamiteOnClick)
		return GetActorLocation();

	return FVector(0, 0, 0);
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

