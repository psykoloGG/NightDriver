#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "CarPawn.generated.h"

class UBoxComponent;
class UCrosshairWidget;
class UMainHUD;
class UCarDashComponent;
class UQuickTimeEvent;
class USpotLightComponent;
class AObstacle;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class ABeatController;
class AObstacleSpawner;
class UObstacleSpawner;
class ASplineActor;
class UQuickTimeEventWidget;
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

	void Interact();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
protected:
	UFUNCTION()
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

	UFUNCTION()
	void LightsQuicktimeEvent(UQuickTimeEvent* QuickTimeEvent);

	UFUNCTION()
	void QuicktimeEventFailed(UQuickTimeEvent* QuickTimeEvent);

	UFUNCTION()
	void ToggleLights();

	UFUNCTION()
	void OnObstacleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnDeletionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void TakeDamage();

	UFUNCTION()
	void ToggleWipers();
	
	UPROPERTY(EditAnywhere)
	UQuickTimeEventWidget* QuickTimeEventWidget;

	UPROPERTY(EditAnywhere)
	UMainHUD* MainHUD;

	UPROPERTY(EditAnywhere)
	UCrosshairWidget* CrosshairWidget;

	/***  Lane following stuff ***/
	float CurrentDistance;

	// From right to left (europe) according to forward moving direction
	int32 CurrentLaneIndex;
	
	bool bJustChangedLanes = false;

	// Starting speed of the car
	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	float CurrentSpeed = 2000.0f;

	UPROPERTY(VisibleAnywhere, Category="Spline Follow Settings")
	TArray<ASplineActor*> SplineActors;

	/*** Inputs ***/
	UPROPERTY(EditAnywhere, Category="Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* MoveLeftRightAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* InteractAction;
	
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

	UPROPERTY(VisibleAnywhere)
	bool bAreLightsOn = false;
	
	UPROPERTY(VisibleAnywhere)
	bool bAreWipersOn = false;
	
	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* LeftSpotLight;

	UPROPERTY(VisibleAnywhere)
	USpotLightComponent* RightSpotLight;

	UPROPERTY(VisibleAnywhere)
	UCarDashComponent* HeadlightsButton;

	UPROPERTY(VisibleAnywhere)
	UCarDashComponent* WiperLever;

	/*** Obstacle Spawning ***/
	UPROPERTY(VisibleAnywhere)
	ABeatController* BeatController;
	
	UPROPERTY(EditAnywhere, Category="Obstacle Spawn Settings")
	TArray<TSubclassOf<AObstacle>> Obstacles;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ObstacleDetectionVolume;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* ObstacleDeletionVolume;

	UPROPERTY()
	bool bShouldSpawnObstacles = true;

	/*** Player Information ***/
	
	// Start with 4 hearts
	UPROPERTY(VisibleAnywhere)
	int32 Health = 4;
};
