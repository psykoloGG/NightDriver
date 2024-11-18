#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainHUD.generated.h"

class UImage;

/**
 * 
 */
UCLASS()
class NIGHTDRIVER_API UMainHUD : public UUserWidget
{
	GENERATED_BODY()

public:
	void SetProgressBarPercent(float Percent);

	void SetHeartImage(int32 Index, bool bIsVisible);
	
protected:
	virtual void NativeConstruct() override;

private:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ProgressBar;
	
	UPROPERTY(meta = (BindWidget))
	UImage* Heart0;

	UPROPERTY(meta = (BindWidget))
	UImage* Heart1;

	UPROPERTY(meta = (BindWidget))
	UImage* Heart2;

	UPROPERTY(meta = (BindWidget))
	UImage* Heart3;
};
