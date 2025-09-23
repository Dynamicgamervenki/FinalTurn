// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Node.h"
#include "Characters/Player/Zack.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"
#include "Pickups/Pickup.h"

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
//	Box->OnComponentEndOverlap.AddDynamic(this,&ANode::OnBoxEndOverlap);

	SM_Ring = CreateDefaultSubobject<UStaticMeshComponent>("FinalNodeRing");
	SM_Ring->SetGenerateOverlapEvents(false);
	SM_Ring->SetHiddenInGame(false);
	SM_Ring->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SM_Ring->SetupAttachment(GetRootComponent());

	Tags.Add(FName("Node"));
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
			HandleFinalNodeTransition(Zack);
		}
		else if (isTeleportNode)
		{
			HandleTeleportNode(Zack);
		}
	}
}

 void ANode::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
 	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
 {

 }

void ANode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!SM_Ring) return;
	
	SM_Ring->SetVisibility(Is_EndNode);
	SM_Ring->SetHiddenInGame(!Is_EndNode);


	UMaterialInterface* CurrentMaterial = SM_Ring->GetMaterial(0);
	if (!CurrentMaterial) return;

	UMaterialInstanceDynamic* MaterialInstance = SM_Ring->CreateDynamicMaterialInstance(0, CurrentMaterial);
	if (MaterialInstance)
	{
		MaterialInstance->SetVectorParameterValue("Color", Color);
	}

}

FVector ANode::InteractPosition_Implementation()
{
	return Box->GetComponentLocation();
}

TArray<AActor*> ANode::GetOverlappingActorsOnNode_Implementation()
{
	TArray<AActor*> OverlappingActors;
	Box->GetOverlappingActors(OverlappingActors);
	OverlappingActors.Remove(this);
	OverlappingActors.Remove(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	return OverlappingActors;
}

void ANode::Interact_Implementation(AActor* Interactor)
{
	GEngine->AddOnScreenDebugMessage(-12, 5.f, FColor::Red, TEXT("node interaction null"));
	AZack* Zack = Cast<AZack>(Interactor);
	if (Zack && !bCannotMoveToNode)
	{
		FVector MoveToLocation;
	
		if (bStopBeforeUnits)
		{
			FVector BoxLocation = Box->GetComponentLocation();
			FVector PlayerLocation = Zack->GetActorLocation();
			FVector Direction = (BoxLocation - PlayerLocation).GetSafeNormal();
			MoveToLocation = BoxLocation - Direction * UnitsBeforeStop;
			Zack->DoMoveTo(MoveToLocation);
		}
		else if (HiddenNode)
		{
			MoveToLocation = HidingLocationActor->GetActorLocation();
			Zack->DoMoveTo(MoveToLocation,20,true);
		}
		else
		{
			MoveToLocation =  Box->GetComponentLocation();
			Zack->DoMoveTo(MoveToLocation);
		}
	}
}

void ANode::Glow_Implementation()
{
	GlowNode();
}

void ANode::ResetGlow_Implementation()
{
	SM_Node->SetRenderCustomDepth(false);
	SM_Node->SetCustomDepthStencilValue(0);
}

void ANode::GlowNode()
{
	SM_Node->SetRenderCustomDepth(true);
	SM_Node->SetCustomDepthStencilValue(1);
}

void ANode::HandleTeleportNode(AZack* Zack)
{
	FTimerHandle DelayTimerHandle;
	TeleportNode->Box->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Zack->SetActorLocation(TeleportNode->GetActorLocation() + FVector(0,0,50.0f),false);
	Zack->GetCharacterMovement()->StopMovementImmediately();
	Zack->DoMoveTo(PostTeleportNode->GetActorLocation(),20,true);
	GetWorldTimerManager().SetTimer(
	DelayTimerHandle,
	FTimerDelegate::CreateLambda([this]()
	{
		if (TeleportNode)
			TeleportNode->Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}),
	1.0f,
	false);
}

void ANode::HandleFinalNodeTransition(AZack* Zack)
{
	Zack->bOnFinalNode = true;
	Zack->CanClickNode = false;
	FTimerHandle DelayTimerHandle;
	GetWorldTimerManager().SetTimer(
	DelayTimerHandle,
	FTimerDelegate::CreateLambda([this,Zack]()
	{
		Zack->ShowGameCompletedWidget();
		Zack->CanClickNode = true;
		Zack->bOnFinalNode = false;
	}),
	0.5f,
	false);
}

