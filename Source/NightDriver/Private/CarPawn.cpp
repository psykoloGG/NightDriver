#include "CarPawn.h"

#include "BeatController.h"
#include "CarDashComponent.h"
#include "CrosshairWidget.h"
#include "EngineUtils.h"
#include "Obstacle.h"
#include "SplineActor.h"
#include "Components/SplineComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "MainHUD.h"
#include "QuickTimeEventWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/BoxComponent.h"
#include "Components/SpotLightComponent.h"
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

	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeadlightButtonAsset(TEXT("/Script/Engine.StaticMesh'/Game/MainLevel/3D/Car/CarInteract/LightButton.LightButton'"));
	if (HeadlightButtonAsset.Succeeded())
	{
		HeadlightsButton = CreateDefaultSubobject<UCarDashComponent>(TEXT("HeadlightsButton"));
		HeadlightsButton->SetStaticMesh(HeadlightButtonAsset.Object);
		HeadlightsButton->SetupAttachment(CarMeshComponent);
		HeadlightsButton->OnDashInteracted.AddDynamic(this, &ACarPawn::ToggleLights);
	}

	static ConstructorHelpers::FObjectFinder<UStaticMesh> WiperLeverAsset(TEXT("/Script/Engine.StaticMesh'/Game/MainLevel/3D/Car/CarInteract/WindShieldWiperStalk.WindShieldWiperStalk'"));
	if (WiperLeverAsset.Succeeded())
	{
		WiperLever = CreateDefaultSubobject<UCarDashComponent>(TEXT("WiperLever"));
		WiperLever->SetStaticMesh(WiperLeverAsset.Object);
		WiperLever->SetupAttachment(CarMeshComponent);
		WiperLever->OnDashInteracted.AddDynamic(this, &ACarPawn::ToggleWipers);
	}

	// Set up car spotlights
	LeftSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("LeftSpotLight"));
	RightSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("RightSpotLight"));
	LeftSpotLight->SetupAttachment(CarMeshComponent);
	RightSpotLight->SetupAttachment(CarMeshComponent);

	LeftSpotLight->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	LeftSpotLight->SetRelativeLocation(FVector(-330.0f, -50.0f, 60.0f));
	RightSpotLight->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f));
	RightSpotLight->SetRelativeLocation(FVector(-330.0f, 50.0f, 60.0f));

	LeftSpotLight->SetIntensityUnits(ELightUnits::Lumens);
	RightSpotLight->SetIntensityUnits(ELightUnits::Lumens);
	LeftSpotLight->SetIntensity(50.0f);
	RightSpotLight->SetIntensity(50.0f);
	LeftSpotLight->SetAttenuationRadius(1000.0f);
	RightSpotLight->SetAttenuationRadius(1000.0f);

	// Start with lights off
	LeftSpotLight->SetVisibility(false);
	RightSpotLight->SetVisibility(false);
	
	// Set up the dash
	static ConstructorHelpers::FObjectFinder<UStaticMesh> HeadlightsButtonAsset(TEXT(""));
	if (HeadlightsButtonAsset.Succeeded())
	{
		HeadlightsButton = CreateDefaultSubobject<UCarDashComponent>(TEXT("HeadlightsButton"));
		HeadlightsButton->SetStaticMesh(HeadlightsButtonAsset.Object);
		HeadlightsButton->SetupAttachment(CarMeshComponent);
		HeadlightsButton->OnDashInteracted.AddDynamic(this, &ACarPawn::ToggleLights);
	}

	// Collision detection settings
	ObstacleDetectionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ObstacleDetectionVolume"));
	ObstacleDetectionVolume->SetupAttachment(CarMeshComponent);
	ObstacleDetectionVolume->SetBoxExtent(FVector(10.0f, 50.0f, 10.0f));
	ObstacleDetectionVolume->SetRelativeLocation(FVector(-340.0f, 0.0f, 70.0f));
	ObstacleDetectionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ObstacleDetectionVolume->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	ObstacleDetectionVolume->SetCollisionProfileName(TEXT("OverlapAll"));
	ObstacleDetectionVolume->OnComponentBeginOverlap.AddDynamic(this, &ACarPawn::OnObstacleOverlap);

	// Obstacle Deletion once passed
	ObstacleDeletionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ObstacleDeletionVolume"));
	ObstacleDeletionVolume->SetupAttachment(CarMeshComponent);
	ObstacleDeletionVolume->SetBoxExtent(FVector(10.0f, 500.0f, 10.0f));
	ObstacleDeletionVolume->SetRelativeLocation(FVector(-150.0f, 0.0f, 70.0f));
	ObstacleDeletionVolume->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	ObstacleDeletionVolume->SetCollisionObjectType(ECollisionChannel::ECC_Pawn);
	ObstacleDeletionVolume->SetCollisionProfileName(TEXT("OverlapAll"));
	ObstacleDeletionVolume->OnComponentBeginOverlap.AddDynamic(this, &ACarPawn::OnDeletionOverlap);
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
	// TO DO: Reference it from editor instead of looping
	for (TActorIterator<ABeatController> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		BeatController = *ActorItr;
		
		// Add quicktime events
		UQuickTimeEvent* QuickTimeLights = NewObject<UQuickTimeEvent>();
		QuickTimeLights->StartTime = 17.0f;
		QuickTimeLights->EndTime = 20.0f;
		QuickTimeLights->OnQuickTimeEventHappening.AddDynamic(this, &ACarPawn::LightsQuicktimeEvent);
		QuickTimeLights->OnQuickTimeEventFailed.AddDynamic(this, &ACarPawn::QuicktimeEventFailed);
		BeatController->AddQuickTimeEvent(QuickTimeLights);
		
		BeatController->PlayMusic();
		BeatController->OnBeat.AddDynamic(this, &ACarPawn::OnBeat);
	}

	if (MainHUD)
	{
		MainHUD->AddToViewport();
	}
}

void ACarPawn::OnBeat()
{
	SpawnObstacle();
}

void ACarPawn::SpawnObstacle()
{
	if (!bShouldSpawnObstacles)
	{
		return;
	}
	int32 SplineIndex = FMath::RandRange(0, SplineActors.Num() - 1);
	if (Obstacles.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No obstacles to spawn"));
		return;
	}
	
	FVector Location = SplineActors[SplineIndex]->SplineComponent->GetLocationAtDistanceAlongSpline(CurrentDistance + 2000.0f, ESplineCoordinateSpace::World);
	FRotator Rotation = SplineActors[SplineIndex]->SplineComponent->GetRotationAtDistanceAlongSpline(CurrentDistance + 2000.0f, ESplineCoordinateSpace::World);

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

	if (!CrosshairWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("No crosshair widget set"));
		return;
	}
	if (!CrosshairWidget->IsInViewport())
	{
		CrosshairWidget->AddToViewport();
	}
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
		if (FVector::Dist(CurrentLocation, TargetLocation) < 100.0f)
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

	MainHUD->SetProgressBarPercent(CurrentDistance / SplineActors[CurrentLaneIndex]->SplineComponent->GetSplineLength());
}

void ACarPawn::ControlCameraRotation(float DeltaTime)
{
	TimeSinceLastMove += DeltaTime;

	// Rotate camera gently do default rotation
	if (TimeSinceLastMove > TimeUntilCameraReset && GetController()->GetControlRotation() != DefaultCameraRotation)
	{
		if (CrosshairWidget && CrosshairWidget->IsInViewport())
		{
			CrosshairWidget->RemoveFromParent();
		}
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

void ACarPawn::LightsQuicktimeEvent(UQuickTimeEvent* QuickTimeEvent)
{
	if (!QuickTimeEventWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("No quicktime event widget set"));
		return;
	}
	
	if (!QuickTimeEventWidget->IsInViewport())
	{
		QuickTimeEventWidget->AddToViewport();
	}
	bShouldSpawnObstacles = false;
	
	QuickTimeEventWidget->SetQuickTimeEventText(FText::FromString("Turn on your lights! Time left: " + FString::SanitizeFloat(QuickTimeEvent->EndTime - BeatController->GetCurrentPlaybackTime())));

	if (bAreLightsOn)
	{
		bShouldSpawnObstacles = true;
		QuickTimeEvent->Complete();
		QuickTimeEventWidget->RemoveFromParent();
	}
}

void ACarPawn::QuicktimeEventFailed(UQuickTimeEvent* QuickTimeEvent)
{
	UE_LOG(LogTemp, Warning, TEXT("Quicktime event failed"));
	if (QuickTimeEventWidget)
	{
		bShouldSpawnObstacles = true;
		QuickTimeEventWidget->RemoveFromParent();
	}
}

void ACarPawn::ToggleLights()
{

	if (bAreLightsOn)
	{
		bAreLightsOn = false;
		LeftSpotLight->SetVisibility(false);
		RightSpotLight->SetVisibility(false);
	}
	else
	{
		bAreLightsOn = true;
		LeftSpotLight->SetVisibility(true);
		RightSpotLight->SetVisibility(true);
	}
}

void ACarPawn::OnObstacleOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Overlapped component: %s"), *OtherActor->GetName());
	if (OtherActor->IsA(AObstacle::StaticClass()))
	{
		TakeDamage();
		OtherComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ACarPawn::OnDeletionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->IsA(AObstacle::StaticClass()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Deleting obstacle"));
		OtherActor->Destroy();
	}
}

void ACarPawn::TakeDamage()
{
	Health--;
	MainHUD->SetHeartImage(Health, false);
	if (Health <= 0)
	{
		// Return to main menu for now
		UGameplayStatics::OpenLevel(GetWorld(), "MenuWorld");
	}
}

void ACarPawn::ToggleWipers()
{

	if (bAreWipersOn)
	{
		bAreWipersOn = false;
		WiperLever->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}
	else
	{
		bAreWipersOn = true;
		WiperLever->SetRelativeRotation(FRotator(0.0f, 0.0f, -31.0f));
	}
}

void ACarPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (SplineActors.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No spline actors found"));
		return;
	}
	FollowSpline(DeltaTime);
	ControlCameraRotation(DeltaTime);
	SpinSteeringWheel(DeltaTime);
}

void ACarPawn::Interact()
{
	// Raycast from camera to see if we hit the dash
	FHitResult HitResult;
	FVector Start = CameraComponent->GetComponentLocation();
	FVector End = Start + CameraComponent->GetForwardVector() * 100.0f;

	UE_LOG(LogTemp, Warning, TEXT("Interacting"));
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_Visibility))
	{
		DrawDebugPoint(GetWorld(), HitResult.ImpactPoint, 10.0f, FColor::Red, false, 3.0f);
		if (HitResult.GetComponent()->GetClass() == UCarDashComponent::StaticClass())
		{
			UCarDashComponent* DashComponent = Cast<UCarDashComponent>(HitResult.GetComponent());
			DashComponent->Interact();
		}
	}
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

	// Bind left mouse click to interact with the car
	EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &ACarPawn::Interact);
}

