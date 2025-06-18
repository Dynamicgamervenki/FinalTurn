// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Node.h"
#include "Save/PlayerSave.h"
#include "Characters/Player/Zack.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


ANode::ANode()
{
	PrimaryActorTick.bCanEverTick = false;
	SM_Node = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	SM_Node->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	SM_Node->SetCollisionObjectType(ECC_EngineTraceChannel1);
	SetRootComponent(SM_Node);
	
	Box = CreateDefaultSubobject<UBoxComponent>("BoxComponent");
	Box->SetupAttachment(GetRootComponent());
	Box->OnComponentBeginOverlap.AddDynamic(this,&ANode::OnBoxOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this,&ANode::OnBoxEndOverlap);
}


void ANode::BeginPlay()
{
	Super::BeginPlay();


}

void ANode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANode::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AZack* Zack = Cast<AZack>(OtherActor))
	{
		if (Is_EndNode)
		{
			UGameplayStatics::OpenLevel(this,LevelName);
			GetCompletedLevel();
		}
		Zack->IsMoving = false;
		Zack->CanClickNode = true;
	}
}

void ANode::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

FVector ANode::InteractPosition()
{
	return Box->GetComponentLocation();
}

TArray<AActor*> ANode::GetOverlappingActorsOnNode()
{
	TArray<AActor*> OverlappingActors;
	Box->GetOverlappingActors(OverlappingActors);
	return OverlappingActors;
}
