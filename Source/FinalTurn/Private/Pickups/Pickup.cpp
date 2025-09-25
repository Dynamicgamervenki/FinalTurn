// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Pickup.h"

#include "Actors/Node.h"
#include "Components/SphereComponent.h"
#include "Characters/Player/Zack.h"
#include "Field/FieldSystemObjects.h"
#include "GameFramework/RotatingMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Perception/AISense_Hearing.h"

APickup::APickup()
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>("ItemMesh");
	ItemMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(RootComponent);

	PulseEffect = CreateDefaultSubobject<UParticleSystemComponent>("PulseRingEffect");
	PulseEffect->SetWorldScale3D(FVector(0.02));
	PulseEffect->SetupAttachment(RootComponent);
	
	RadialFalloff = CreateDefaultSubobject<URadialFalloff>(TEXT("RadialFalloff"));
	FieldSystemMetaDataFilter = CreateDefaultSubobject<UFieldSystemMetaData>(TEXT("FieldSystemMetaDataFilter"));
	RadialVector = CreateDefaultSubobject<URadialVector>(TEXT("RadialVector"));

	RotatingMovement = CreateDefaultSubobject<URotatingMovementComponent>(TEXT("RotatingMovementComponent"));
	RotatingMovement->RotationRate = FRotator(0.f, 180.f, 0.f);
	RotatingMovement->SetAutoActivate(true);

	Tags.Add(FName("Pickup"));
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
		PulseEffect->SetVisibility(false);
		RotatingMovement->DestroyComponent();
		PlayPickUpSound(ItemMesh->GetComponentLocation());
		this->SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		Zack->OnPickedUp(PickupType,PickupAmount);
		Zack->AddPickUpItem(this);
#if WITH_EDITOR
		int32 RandomValue = FMath::RandRange(1, 10); 
		FString NewLabel = FString::Printf(TEXT("Inventory_Pickup_%d"), RandomValue);
		this->SetActorLabel(*NewLabel);
#endif
	}
	else if (ANode* Node = Cast<ANode>(OtherActor))
	{
		UAISense_Hearing::ReportNoiseEvent(GetWorld(),GetActorLocation(),1,this,0,"Distraction");
		SetActorHiddenInGame(true);
		PlayPickupImpactSound(SweepResult.ImpactPoint);
		Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
		if (PickupType == EPickupType::LavaCrystal && Node->isGasNode && Node->isGasNode && Node->GasVfx)
		{
			Node->SM_Node->SetVisibility(false);
			Node->GasVfx->SetActorHiddenInGame(true);
		}
		
		SetLifeSpan(0.5f);
	}
}

void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APickup::ActivateField(FVector Location)
{
	Field(Location);
}

void APickup::BroadcastThrowableImpact(AActor* HitActor)
{
	OnThrowableImpact.Broadcast(HitActor);
}


