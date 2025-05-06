// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/ThrowableStone.h"

AThrowableStone::AThrowableStone()
{
 
	PrimaryActorTick.bCanEverTick = true;
	SM_Stone = CreateDefaultSubobject<UStaticMeshComponent>("SM_Stone");
	SM_Stone->SetCanEverAffectNavigation(false);
	SM_Stone->SetSimulatePhysics(true);
	SM_Stone->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent = SM_Stone;
}

void AThrowableStone::BeginPlay()
{
	Super::BeginPlay();
	
}

void AThrowableStone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

