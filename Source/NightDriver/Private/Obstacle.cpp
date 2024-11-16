#include "Obstacle.h"

#include "GeometryCollection/GeometryCollectionComponent.h"

AObstacle::AObstacle()
{
	GeometryCollectionComponent = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollectionComponent"));
	RootComponent = GeometryCollectionComponent;
}
