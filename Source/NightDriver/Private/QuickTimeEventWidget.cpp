#include "QuickTimeEventWidget.h"

#include "Components/TextBlock.h"

void UQuickTimeEventWidget::SetQuickTimeEventText(FText Text)
{
	if (QuickTimeEventText)
	{
		QuickTimeEventText->SetText(Text);
		UE_LOG(LogTemp, Warning, TEXT("Set quick time event text: %s"), *Text.ToString());
	}
}

void UQuickTimeEventWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
