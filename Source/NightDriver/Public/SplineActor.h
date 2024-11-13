#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

UCLASS()
class NIGHTDRIVER_API ASplineActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineActor();
	
	UPROPERTY(EditAnywhere)
	class USplineComponent* SplineComponent;
};