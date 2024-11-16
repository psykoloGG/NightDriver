#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GeometryCollection/GeometryCollectionActor.h"
#include "Obstacle.generated.h"

UCLASS()
class NIGHTDRIVER_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacle();

private:
	UPROPERTY(EditAnywhere)
	UGeometryCollectionComponent* GeometryCollectionComponent;
};
