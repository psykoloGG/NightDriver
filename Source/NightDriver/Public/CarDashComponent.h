#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CarDashComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashInteracted);

UCLASS()
class NIGHTDRIVER_API UCarDashComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:	
	UCarDashComponent();

	UFUNCTION()
	void Interact();

	FOnDashInteracted OnDashInteracted;
protected:
	virtual void BeginPlay() override;
};
