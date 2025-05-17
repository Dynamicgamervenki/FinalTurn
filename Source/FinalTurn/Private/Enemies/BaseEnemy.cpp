// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/BaseEnemy.h"

// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}
void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector ABaseEnemy::InteractPosition()
{
	return GetActorLocation();
}

TArray<AActor*> ABaseEnemy::GetOverlappingActorsOnNode()
{
	TArray<AActor*> OverlappingActors;
	GetMesh()->GetOverlappingActors(OverlappingActors);
	return OverlappingActors;
}
