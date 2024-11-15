#include "Obstacle.h"

AObstacle::AObstacle()
{	
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;
}

void AObstacle::SetObstacleMesh(UStaticMesh* Mesh)
{
	MeshComponent->SetStaticMesh(Mesh);
}

