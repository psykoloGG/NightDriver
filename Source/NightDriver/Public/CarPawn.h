#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "CarPawn.generated.h"

class ABeatController;
class AObstacleSpawner;
class UObstacleSpawner;
class ASplineActor;
/**
* Main Character Car class
*/
UCLASS()
class NIGHTDRIVER_API ACarPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ACarPawn();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnBeat();

	// Spawn obstacle on random spawner
	UFUNCTION()
	void SpawnObstacle();
	
	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	float CurrentDistance;

	// Starting speed of the car
	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	float CurrentSpeed = 500.0f;

	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	TArray<ASplineActor*> SplineActors;

	// Car Mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	ABeatController* BeatController;
	
	UPROPERTY(EditAnywhere, Category="Obstacle Spawn Settings")
	TArray<UStaticMesh*> ObstacleMeshes;
};
