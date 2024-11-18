#include "CarDashComponent.h"

UCarDashComponent::UCarDashComponent()
{
}

void UCarDashComponent::Interact()
{
	OnDashInteracted.Broadcast();
}

void UCarDashComponent::BeginPlay()
{
	Super::BeginPlay();	
}

