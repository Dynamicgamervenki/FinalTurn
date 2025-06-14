// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Characters/Player/Zack.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(RootComponent);
	
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&APickup::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this,&APickup::OnSphereEndOverlap);
	
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->Implements<UPickupInterface	>())
	{
		PlayPickUpSound(GetActorLocation());
		IPickupInterface* Pickup = Cast<IPickupInterface>(OtherActor);
		if (Pickup && OtherActor)
		{
			Pickup->OnPickedUp(PickupType,PickupAmount);
			Pickup->AddToPickupArray(this);
			this->SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
			Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void APickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


