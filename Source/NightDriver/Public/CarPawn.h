#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "CarPawn.generated.h"

class AObstacle;
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

	UFUNCTION()
	void LookAround(const FInputActionValue& InputActionValue);

	void FollowSpline(float DeltaTime);

	void ControlCameraRotation(float DeltaTime);

	void SpinSteeringWheel(float DeltaTime);

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

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;
	
	// Default camera rotation to return to
	UPROPERTY(EditAnywhere, Category="Input")
	FRotator DefaultCameraRotation = FRotator(0.0f, 0.0f, 0.0f);
	
	float TimeSinceLastMove = 0.0f;

	UPROPERTY(EditAnywhere, Category="Input")
	float TimeUntilCameraReset = 3.0f;

	/*** Car Visuals ***/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMeshComponent;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* SteeringWheelMeshComponent;

	bool bIsTurningRight = false;

	/*** Obstacle Spawning ***/
	UPROPERTY(VisibleAnywhere)
	ABeatController* BeatController;
	
	UPROPERTY(EditAnywhere, Category="Obstacle Spawn Settings")
	TArray<TSubclassOf<AObstacle>> Obstacles;
};
