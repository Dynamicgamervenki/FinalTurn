// DynamicV

#pragma once

#include "CoreMinimal.h"
#include "Actors/Node.h"
#include "TempNode.generated.h"

/**
 * 
 */
UCLASS()
class FINALTURN_API ATempNode : public ANode
{
	GENERATED_BODY()
public:

protected:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IsFrozen = true;
};
