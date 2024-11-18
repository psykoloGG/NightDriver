#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuickTimeEventWidget.generated.h"

/**
 * 
 */
UCLASS()
class NIGHTDRIVER_API UQuickTimeEventWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetQuickTimeEventText(FText Text);
	
protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* QuickTimeEventText;
	
	virtual void NativeConstruct() override;
};
