#include "CarPawn.h"

#include "BeatController.h"
#include "EngineUtils.h"
#include "Obstacle.h"
#include "SplineActor.h"
#include "Components/SplineComponent.h"

ACarPawn::ACarPawn()
{
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	CarMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	RootComponent = CarMeshComponent;

	// Set up camera
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(CarMeshComponent);
	CameraComponent->FieldOfView = 120.0f;
	CameraComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	CameraComponent->SetRelativeLocation(FVector(-120.0f, 30.0f, 110.0f));

	// Set up car meshes
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CarMeshAsset(TEXT("/Script/Engine.StaticMesh'/Game/MainLevel/3D/Car/Textures/Car-UE_Bonnet_low.Car-UE_Bonnet_low'"));
	if (CarMeshAsset.Succeeded())
	{
		CarMeshComponent->SetStaticMesh(CarMeshAsset.Object);
		CarMeshComponent->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	}
}

void ACarPawn::BeginPlay()
{
	Super::BeginPlay();

	// Find all the lane splines
	for (TActorIterator<ASplineActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		SplineActors.Add(*ActorItr);
	}

	// Sort the splines by index
	SplineActors.Sort([](const ASplineActor& A, const ASplineActor& B) {
		return A.Index < B.Index;
	});

	// Find BeatMapper and start playing music (Ideally done elsewhere)
	for (TActorIterator<ABeatController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		BeatController = *ActorItr;
		BeatController->PlayMusic();
		BeatController->OnBeat.AddDynamic(this, &ACarPawn::OnBeat);
	}
}

void ACarPawn::OnBeat()
{
	SpawnObstacle();	
}

void ACarPawn::SpawnObstacle()
{
	int32 SplineIndex = FMath::RandRange(0, SplineActors.Num() - 1);
	if (ObstacleMeshes.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No obstacles to spawn"));
		return;
	}
	
	FVector Location = SplineActors[SplineIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance + 1000.0f, ESplineCoordinateSpace::World);
	FRotator Rotation = SplineActors[SplineIndex]->SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance + 1000.0f, ESplineCoordinateSpace::World);

	int32 RandIndex = FMath::RandRange(0, ObstacleMeshes.Num() - 1);
	UStaticMesh* Mesh = ObstacleMeshes[RandIndex];
	GetWorld()->SpawnActor<AObstacle>(AObstacle::StaticClass(), Location, Rotation)->SetObstacleMesh(Mesh);
}

void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Start game in the middle lane
	CurrentDistance += CurrentSpeed * DeltaTime;
	if (SplineActors.Num() > 0)
	{
		int32 MidLaneIndex = SplineActors.Num() / 2.0f;
		SetActorLocation(SplineActors[MidLaneIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World));
		SetActorRotation(SplineActors[MidLaneIndex]->SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World));
		AddActorWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("No spline actors found"));
	}
}

void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

