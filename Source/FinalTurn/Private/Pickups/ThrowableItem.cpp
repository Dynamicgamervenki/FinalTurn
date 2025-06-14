// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/ThrowableItem.h"

AThrowableItem::AThrowableItem()
{
 
	PrimaryActorTick.bCanEverTick = true;
	SM_Throwable = CreateDefaultSubobject<UStaticMeshComponent>("SM_Stone");
	SM_Throwable->SetCanEverAffectNavigation(false);
	SM_Throwable->SetSimulatePhysics(true);
	SM_Throwable->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent = SM_Throwable;
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AThrowableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

