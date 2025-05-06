// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Characters/Player/Zack.h"
#include "Pickups/Pickup.h"

#include "Characters/Player/ZackPlayerController.h"

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
	AZack* Zack = Cast<AZack>(OtherActor);
	if (Zack)
	{
		Zack->SetPickupItem(this);
	}
}

void APickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	// AZack* Zack = Cast<AZack>(OtherActor);
	// if (Zack)
	// {
	// 	//Zack->SetPickupItem(nullptr);
	// 	Destroy();
	// }
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


