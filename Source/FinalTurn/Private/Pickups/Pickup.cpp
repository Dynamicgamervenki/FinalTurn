// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Pickup.h"
#include "Components/SphereComponent.h"
#include "Characters/Player/Zack.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(RootComponent);
	
	RadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("RadialFalloff"));
	FieldSystemMetaDataFilter = CreateDefaultSubobject<UFieldSystemMetaData>(TEXT("FieldSystemMetaDataFilter"));
	RadialVector = CreateDefaultSubobject<URadialVector>(TEXT("RadialVector"));
}

void APickup::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&APickup::OnSphereOverlap);
	//Sphere->OnComponentEndOverlap.AddDynamic(this,&APickup::OnSphereEndOverlap);
	
}

void APickup::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AZack* Zack = Cast<AZack>(OtherActor))
	{
		PlayPickUpSound(GetActorLocation());
		this->SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Zack->OnPickedUp(PickupType,PickupAmount);
		Zack->AddPickUpItem(this);
	}
	else
	{
		//SetActorHiddenInGame(true);
		SetLifeSpan(1.5f);
	}
	
}

// void APickup::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
// 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
// {
// 	
// }

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickup::ActivateField(FVector Location)
{
	Field(Location);
}


