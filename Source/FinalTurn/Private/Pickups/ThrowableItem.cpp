// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/ThrowableItem.h"
#include "Actors/Node.h"
#include "Breakable/BreakableActor.h"
#include "Components/SphereComponent.h"


AThrowableItem::AThrowableItem()
{
 
	PrimaryActorTick.bCanEverTick = true;
	SM_Throwable = CreateDefaultSubobject<UStaticMeshComponent>("SM_Stone");
	SM_Throwable->SetCanEverAffectNavigation(false);
	SM_Throwable->SetSimulatePhysics(true);
	SM_Throwable->SetCollisionProfileName(TEXT("PhysicsActor"));
	RootComponent = SM_Throwable;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(SM_Throwable);
	Sphere->SetSphereRadius(20.0f);
	Sphere->SetLineThickness(0.5f);
	Sphere->ShapeColor = FColor::Blue;
	Sphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);

	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AThrowableItem::OnSphereOverlap);
}

void AThrowableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AThrowableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void AThrowableItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (ANode* Node = Cast<ANode>(OtherActor))
	{
		OnThrowableImpact.Broadcast(Node);
	}
	else if (ABreakableActor* BreakableActor = Cast<ABreakableActor>(OtherActor))
	{
		OnThrowableImpact.Broadcast(BreakableActor);
	}
}


