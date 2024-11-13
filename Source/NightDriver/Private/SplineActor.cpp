#include "SplineActor.h"

#include "Components/SplineComponent.h"

ASplineActor::ASplineActor()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	RootComponent = SplineComponent;
}

