#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CarPawn.generated.h"

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

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	float CurrentDistance;

	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	float CurrentSpeed;

	UPROPERTY(EditAnywhere, Category="Spline Follow Settings")
	class ASplineActor* SplineActor;

	// Car Mesh
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CarMeshComponent;

};
