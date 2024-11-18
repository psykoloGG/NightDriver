#include "Obstacle.h"

#include "Components/BoxComponent.h"
#include "GeometryCollection/GeometryCollectionComponent.h"

AObstacle::AObstacle()
{
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	RootComponent = GeometryCollectionComponent;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(RootComponent);
}
