#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"

UCLASS()
class NIGHTDRIVER_API AObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	AObstacle();

	UFUNCTION()
	void SetObstacleMesh(UStaticMesh* Mesh);

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComponent;
};
