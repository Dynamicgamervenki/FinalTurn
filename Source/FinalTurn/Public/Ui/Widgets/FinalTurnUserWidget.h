// DynamicV

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FinalTurnUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class FINALTURN_API UFinalTurnUserWidget : public UUserWidget
{
	GENERATED_BODY()
public :
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)	
	TObjectPtr<UObject> WidgetController;
	protected :
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
