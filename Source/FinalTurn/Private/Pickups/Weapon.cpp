// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickups/Weapon.h"

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
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

