// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Weapon.h"
#include "Components/SphereComponent.h"
#include "Interfaces/PickupInterface.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AWeapon::Equip(USceneComponent* InParent, FName InSoketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget,true);
	ItemMesh->AttachToComponent(InParent, TransformRules,InSoketName);
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (OtherActor->Implements<UPickupInterface	>())
	{
		IPickupInterface* Pickup = Cast<IPickupInterface>(OtherActor);
		if (Pickup && OtherActor)
		{
			Pickup->OnPickedUp(EPickupType::Bullet,BulletCount);
			Pickup->AddToPickupArray(this);
			this->SetActorLocation(FVector(0.0f, 0.0f, 0.0f));
			Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

