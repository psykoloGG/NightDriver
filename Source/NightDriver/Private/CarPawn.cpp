#include "CarPawn.h"

#include "SplineActor.h"
#include "Components/SplineComponent.h"

// Sets default values
ACarPawn::ACarPawn()
{
	CarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	RootComponent = CarMeshComponent;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CarMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/Prototyping/_Meshes/Vehicles/Car/Car_Car_Base.Car_Car_Base'"));
	if (CarMeshAsset.Succeeded())
	{
		CarMeshComponent->SetStaticMesh(CarMeshAsset.Object);
	}
}

// Called when the game starts or when spawned
void ACarPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CurrentDistance += CurrentSpeed * DeltaTime;
	if (SplineActor)
	{
		SetActorLocation(SplineActor->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World));
		SetActorRotation(SplineActor->SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World));
	}
}

// Called to bind functionality to input
void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

