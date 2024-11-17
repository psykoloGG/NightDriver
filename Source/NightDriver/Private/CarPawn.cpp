#include "CarPawn.h"

#include "BeatController.h"
#include "EngineUtils.h"
#include "Obstacle.h"
#include "SplineActor.h"
#include "Components/SplineComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/GameplayStatics.h"

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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SteeringWheelAsset(TEXT("/Script/Engine.StaticMesh'/Game/MainLevel/3D/Car/Textures/SM_CarWheel.SM_CarWheel'"));
	if (SteeringWheelAsset.Succeeded())
	{
		SteeringWheelMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SteeringWheel"));
		SteeringWheelMeshComponent->SetStaticMesh(SteeringWheelAsset.Object);
		SteeringWheelMeshComponent->SetupAttachment(CarMeshComponent);
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

	if (SplineActors.Num() > 0)
	{
		// At beginning of the game start in middle-est lane
		CurrentLaneIndex = SplineActors.Num() / 2.0f;
	}
	else 
	{
		UE_LOG(LogTemp, Warning, TEXT("No spline actors found"));
	}

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
	if (Obstacles.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No obstacles to spawn"));
		return;
	}
	
	FVector Location = SplineActors[SplineIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance + 1000.0f, ESplineCoordinateSpace::World);
	FRotator Rotation = SplineActors[SplineIndex]->SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance + 1000.0f, ESplineCoordinateSpace::World);

	int32 RandIndex = FMath::RandRange(0, Obstacles.Num() - 1);
	GetWorld()->SpawnActor<AObstacle>(Obstacles[RandIndex], Location, Rotation);
}

void ACarPawn::MoveLeftRight(const FInputActionValue& Value)
{
	const float AxisValue = Value.Get<float>();
	if (AxisValue > 0.0f)
	{
		if (CurrentLaneIndex == SplineActors.Num() - 1)
		{
			return;
		}
		bJustChangedLanes = true;
		bIsTurningRight = true;
		CurrentLaneIndex++;
	}
	
	if (AxisValue < 0.0f)
	{
		if (CurrentLaneIndex == 0)
		{
			return;
		}
		bJustChangedLanes = true;
		bIsTurningRight = false;
		CurrentLaneIndex--;
	}
}

void ACarPawn::LookAround(const FInputActionValue& InputActionValue)
{
	const FVector2D MouseAxisValue = InputActionValue.Get<FVector2D>();

	// Limit horizontal movement
	float NewPitch = GetControlRotation().Pitch + MouseAxisValue.Y;
	float NewYaw = GetControlRotation().Yaw + MouseAxisValue.X;
	NewYaw = FMath::ClampAngle(NewYaw, -60.0f, 60.0f);
	GetController()->SetControlRotation(FRotator(NewPitch, NewYaw, 0.0f));
	TimeSinceLastMove = 0.0f;
}

void ACarPawn::FollowSpline(float DeltaTime)
{
	// Start game in the middle lane
	CurrentDistance += CurrentSpeed * DeltaTime;

	// Smooth transition into another lane if needed
	if (bJustChangedLanes)
	{
		FVector CurrentLocation = GetActorLocation();
		FVector TargetLocation = SplineActors[CurrentLaneIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World);
		// This VInterp never completes for some reason
		SetActorLocation(FMath::VInterpTo(CurrentLocation, TargetLocation, DeltaTime, 20.0f));
		UE_LOG(LogTemp, Warning, TEXT("Just changed lanes: %f"), FVector::Dist(CurrentLocation, TargetLocation));
		if (FVector::Dist(CurrentLocation, TargetLocation) < 25.0f)
		{
			bJustChangedLanes = false;
		}
	}
	else
	{
		SetActorLocation(SplineActors[CurrentLaneIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World));
	}
	
	SetActorRotation(SplineActors[CurrentLaneIndex]->SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance, ESplineCoordinateSpace::World));
	AddActorWorldRotation(FRotator(0.0f, 180.0f, 0.0f));
}

void ACarPawn::ControlCameraRotation(float DeltaTime)
{
	TimeSinceLastMove += DeltaTime;

	// Rotate camera gently do default rotation
	if (TimeSinceLastMove > TimeUntilCameraReset && GetController()->GetControlRotation() != DefaultCameraRotation)
	{
		FRotator NewRotation = FMath::RInterpTo(GetController()->GetControlRotation(), DefaultCameraRotation, DeltaTime, 5.0f);
		GetController()->SetControlRotation(NewRotation);
	}
}

void ACarPawn::SpinSteeringWheel(float DeltaTime)
{
	if (bJustChangedLanes)
	{
		if (bIsTurningRight)
		{
			FRotator NewRotation = FMath::RInterpTo(SteeringWheelMeshComponent->GetRelativeRotation(), FRotator(SteeringWheelMeshComponent->GetRelativeRotation().Pitch, SteeringWheelMeshComponent->GetRelativeRotation().Yaw, 45.0f), DeltaTime, 5.0f);
			SteeringWheelMeshComponent->SetRelativeRotation(NewRotation);
		}
		else
		{
			FRotator NewRotation = FMath::RInterpTo(SteeringWheelMeshComponent->GetRelativeRotation(), FRotator(SteeringWheelMeshComponent->GetRelativeRotation().Pitch, SteeringWheelMeshComponent->GetRelativeRotation().Yaw, -45.0f), DeltaTime, 5.0f);
			SteeringWheelMeshComponent->SetRelativeRotation(NewRotation);
		}
	}
	else
	{
		FRotator NewRotation = FMath::RInterpTo(SteeringWheelMeshComponent->GetRelativeRotation(), FRotator(SteeringWheelMeshComponent->GetRelativeRotation().Pitch, SteeringWheelMeshComponent->GetRelativeRotation().Yaw, 0.0f), DeltaTime, 5.0f);
		SteeringWheelMeshComponent->SetRelativeRotation(NewRotation);
	}
}

void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	FollowSpline(DeltaTime);
	ControlCameraRotation(DeltaTime);
	SpinSteeringWheel(DeltaTime);
}

void ACarPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// Bind input mapping
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AddControllerYawInput(180.0f);
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
	InputSubsystem->AddMappingContext(InputMappingContext, 0);

	// Bind A and D to move car left and right
	EnhancedInputComponent->BindAction(MoveLeftRightAction, ETriggerEvent::Started, this, &ACarPawn::MoveLeftRight);

	// Bind mouse movement to look around
	EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ACarPawn::LookAround);
}

