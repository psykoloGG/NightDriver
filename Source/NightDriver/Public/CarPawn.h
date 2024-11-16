#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "CarPawn.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
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

	UFUNCTION()
	void MoveLeftRight(const FInputActionValue& Value);

	void FollowSpline(float DeltaTime);

	/***  Lane following stuff ***/
	float CurrentDistance;

	// From right to left (europe) according to forward moving direction
	int32 CurrentLaneIndex;
	
	bool bJustChangedLanes = false;

	// Starting speed of the car
	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	float CurrentSpeed = 500.0f;

	UPROPERTY(VisibleAnywhere, Category="Spline Follow Settings")
	TArray<ASplineActor*> SplineActors;
	

	/*** Inputs ***/
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveLeftRightAction;
	

	/*** Car Visuals ***/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	ABeatController* BeatController;
	
	UPROPERTY(EditAnywhere, Category="Obstacle Spawn Settings")
	TArray<UStaticMesh*> ObstacleMeshes;
};
