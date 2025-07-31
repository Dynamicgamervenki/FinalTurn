// DynamicV


#include "Actors/TempNode.h"
#include "Characters/Player/Zack.h"
#include "Components/BoxComponent.h"
#include "Pickups/ThrowableItem.h"

void ATempNode::Interact_Implementation(AActor* Interactor)
{
	//Super::Interact_Implementation(Interactor);
	GEngine->AddOnScreenDebugMessage(123, 5, FColor::Red, "Interact from tempNode");
	if (AZack* Zack = Cast<AZack>(Interactor))
	{
		if (!IsFrozen)
		{
			Zack->DoMoveTo(Box->GetComponentLocation());
		}
	}
}

void ATempNode::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OnBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
	if (AThrowableItem* throwable = Cast<AThrowableItem>(OtherActor))
	{
		IsFrozen = false;
	}
}
