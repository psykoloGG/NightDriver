#include "MainHUD.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

void UMainHUD::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMainHUD::SetProgressBarPercent(float Percent)
{
	ProgressBar->SetPercent(Percent);
}

void UMainHUD::SetHeartImage(int32 Index, bool bIsVisible)
{
	switch (Index)
	{
		case 0:
            Heart0->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            break;
		case 1:
            Heart1->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            break;
		case 2:
            Heart2->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            break;
	    case 3:
            Heart3->SetVisibility(bIsVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
            break;
	}
}
