// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyTrain.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "GDP_ProjectGameModeBase.h"
#include "Macros.h"
#include "Engine/StaticMesh.h"

const int HEIGHT = 0;//height of player above spline

#define CARRIAGESPACING 38
#define TIMETOUPDATETRAIN 0.017f

// Sets default values
AToyTrain::AToyTrain()
	: splinePointer(0), Rotating(false), CarriageAttached(false), MovementDirection(0), TrainState(RunawayTrain)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Game/Train/TrainScaleRef2.TrainScaleRef2'"));
	if (MeshAssetBody.Object)
		MeshComponent->SetStaticMesh(MeshAssetBody.Object);

	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 100.0f), FRotator(-30.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 700.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	//Take control of the default Player
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	for (int i = 0; i < NUMBEROFTRACKSWITCHERS; ++i)
	{
		TrackSwitched[i] = false;
	}
}

// Called when the game starts or when spawned
void AToyTrain::BeginPlay()
{
	Super::BeginPlay();


	for(int i = 0; i < SplineBPs.Num(); ++i)
	{
		USplineComponent* SplineComponent = SplineBPs[i]->FindComponentByClass<USplineComponent>();

		if (!SplineComponent)
		{
			UE_LOG(LogTemp, Error, TEXT("No spline component find in actor %d"), i);
			continue;
		}

		int numberOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();
		float totalLength = SplineComponent->GetSplineLength();

		float currentLength = 0;

		int sampleLength = 5; //we will sample the spline every "sampleLength" units

		if (numberOfSplinePoints > 1) {//you can also use GetName() to select the spline component you want to process

			int splinePointCount = 0;

			pathPointLocation.Push({});
			pathPointRotation.Push({});

			while (currentLength < totalLength) 
			{
				//The next line samples the spline at "currentLength" units from the starting point
				FTransform splinePointTransform = SplineComponent->GetTransformAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World);

				currentLength += sampleLength;//increase "currentLength" for the next sample

				pathPointRotation[i].Add(splinePointTransform.GetRotation());

				FVector up = HEIGHT * pathPointRotation[i][splinePointCount].GetAxisZ(); //this vector is above the spline by 300 units

				pathPointLocation[i].Add(splinePointTransform.GetLocation() + up); //this will be used to place the player (i.e. the space ship if you don't change the model)

				splinePointCount += 1;
			}
		}
	}

	// Uncomment the two lines below to test the train puzzle from near the end
	//TrainState = PossessableTrain4;
	//splinePointer = 1500;
}

void AToyTrain::Restart()
{
	Super::Restart();

	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->ChangeHUD("ToyTrain");
}

// Called every frame
void AToyTrain::Tick(float DeltaTime)
{
	if (!PrimaryActorTick.bCanEverTick)
		return;

	Super::Tick(DeltaTime);

	UpdateState(); // Check train state and update if neccessary

	// Handle the updating of the spline pointer
	static float SplineTimer = 0.0f;

	if (MovementDirection)
	{
		SplineTimer += DeltaTime;
		if (SplineTimer >= TIMETOUPDATETRAIN)
		{
			int iUpdates = (int)(SplineTimer / TIMETOUPDATETRAIN);

			for (int i = 0; i < iUpdates; ++i)
			{
				SplineTimer = SplineTimer - TIMETOUPDATETRAIN;
				UpdateSplinePointer();
			}
		}
	}
	else
		SplineTimer = 0.0f;


	// Move the train and its carriage
	if (!Rotating)
	{
		UpdateTrainOnSpline();
	}
	else
	{
		RootComponent->SetWorldRotation(FRotator(0.0f, RootComponent->GetComponentRotation().Yaw - (30.0f * DeltaTime), 0.0f));
		MoveForward(0.0f); // Make sure the train can't move
		if (FMath::Abs(RootComponent->GetComponentRotation().Yaw - pathPointRotation[TrainState][splinePointer].Rotator().Yaw) < 1.0f)
		{
			Rotating = false;
			splinePointer = 0;
		}
	}

	TrackSwappingManager->UpdateInteractionUI(this); // Update the train's interaction UI

	if (PossessionChangerManager->PuzzleSolutionPadIsOverlapped())
	{
		BridgePieces[0]->SetActorLocation(FVector(1453.0f, 1854.0f, 150.5f));
		BridgePieces[0]->SetActorRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));

		BridgePieces[1]->SetActorLocation(FVector(1703.0f, 1854.0f, 150.5f));
		BridgePieces[1]->SetActorRotation(FQuat(FRotator(0.0f, 0.0f, 0.0f)));
	}

	//End movement at end of Spline
	if (MeshComponent->IsOverlappingActor(TrainHouse) && CarriageAttached)
	{
		CompleteTrainPuzzle();
	}
}

void AToyTrain::UpdateState()
{
	switch (TrainState)
	{
	case TRAIN_STATES::RunawayTrain:
		if (!AutomatedMovement())
		{
			ChangeToState(TrackSwitched[0] ? RunawayTrain2 : RunawayTrain_Failed);
		}
		break;

	case TRAIN_STATES::RunawayTrain_Failed:
	case TRAIN_STATES::RunawayTrain2_Failed:
		AutomatedMovement();
		break;

	case TRAIN_STATES::RunawayTrain2:
		if (!AutomatedMovement())
		{
			ChangeToState(TrackSwitched[1] ? RunawayTrain3 : RunawayTrain2_Failed);
		}
		break;

	case TRAIN_STATES::RunawayTrain3:
		if (!AutomatedMovement())
		{
			ChangeToState(PossessableTrain);
			Rotating = true; // The train will rotate BEFORE the player can move on the new line
		}
		break;

	case TRAIN_STATES::PossessableTrain:
	case TRAIN_STATES::PossessableTrain2:
	case TRAIN_STATES::PossessableTrain3:
	case TRAIN_STATES::PossessableTrain5:
		break;
	case TRAIN_STATES::PossessableTrain6:
		if(!CarriageAttached)
			CarriageAttached = true;
		break;
	
	case TRAIN_STATES::PossessableTrain4:
		// Rotate the train then attach the carriage
		if (EndOfCurrentLine())
		{
			Rotating = true;
			ChangeToState(PossessableTrain6);
			TrackSwappingManager->ForceSwitch(2, false);
		}
		break;


	case TRAIN_STATES::TRAIN_STATES_MAX:
	default:
		UE_LOG(LogTemp, Warning, TEXT("AToyTrain::UpdateState has attempted to use an invalid TRAIN_STATE"));
		break;
	}
}

void AToyTrain::ChangeToState(TRAIN_STATES newState)
{
	splinePointer = 0;
	TrainState = newState;
	MoveForward(0.0f);
}

bool AToyTrain::AutomatedMovement()
{
	if (!EndOfCurrentLine())
	{
		MoveForward(1.0f);
		return true;
	}
	return false;
}

void AToyTrain::UpdateSplinePointer()
{
	if (MeshComponent->IsOverlappingActor(RotatingTrack))
	{
		--splinePointer; // Attempt to revert the spline pointer to remove the collision
		UpdateTrainOnSpline();
		if (MeshComponent->IsOverlappingActor(RotatingTrack)) // If we're still colliding then we must for gone the wrong way
		{
			splinePointer += 2;
			UpdateTrainOnSpline();
		}
	}
		if (MovementDirection == 1)
		{
			if (!MeshComponent->IsOverlappingActor(Obstacle))
			{
				if (++splinePointer >= pathPointLocation[TrainState].Num() - 1)
					splinePointer = pathPointLocation[TrainState].Num() - 1;
			}
		}
		else
		{
			if (--splinePointer < 0)
				splinePointer = 0;
		}
	
}

void AToyTrain::UpdateTrainOnSpline()
{
	RootComponent->SetWorldLocation(pathPointLocation[TrainState][splinePointer]);//just move the player to the next sampled point on the spline
	RootComponent->SetWorldRotation(pathPointRotation[TrainState][splinePointer]);//and give the player the same rotation as the sampled point

	UpdateCarriages();
}

void AToyTrain::UpdateCarriages()
{
	if (!CarriageAttached) // The carriage is not attached, so don't update its location
		return;

	int carriageSplinePointer = 0;
	for (int i = 0; i < Carriages.Num(); ++i)
	{
		carriageSplinePointer = splinePointer - (CARRIAGESPACING * (i + 1));
		if (carriageSplinePointer >= 0)
		{
			Carriages[i]->SetActorLocation(pathPointLocation[TrainState][carriageSplinePointer]);
			Carriages[i]->SetActorRotation(pathPointRotation[TrainState][carriageSplinePointer]);
		}
		else
		{
			Carriages[i]->SetActorLocation(RootComponent->RelativeLocation - (RootComponent->GetForwardVector() * 175.0f));
			Carriages[i]->SetActorRotation(pathPointRotation[TrainState][0]);
		}
	}
}

bool AToyTrain::OnFailureTrainLine()
{
	return TrainState == TRAIN_STATES::RunawayTrain_Failed || TrainState == TRAIN_STATES::RunawayTrain2_Failed;
}

bool AToyTrain::TrainPuzzleFailed()
{
	if (OnFailureTrainLine())
	{
		return EndOfCurrentLine();
	}

	return false; // We haven't failed the train puzzle
}

void AToyTrain::CompleteTrainPuzzle()
{
	OUTPUT_STRING("END");
	PrimaryActorTick.bCanEverTick = false;
	
	if (PossessionChangerManager)
		PossessionChangerManager->ForceChangePossession(POSSESSABLE_VEHICLES::Car);
}

bool AToyTrain::StartOfCurrentLine()
{
	return splinePointer <= 0;
}

bool AToyTrain::EndOfCurrentLine()
{
	return splinePointer >= pathPointLocation[TrainState].Num() - 1;
}

void AToyTrain::SetTrainStateToChangeTo(int SwitchActivated)
{
	switch (SwitchActivated)
	{
	case 0:
		if (TrainState == TRAIN_STATES::PossessableTrain)
		{
			if (TrackSwappingManager->IsSwapperActivated(1))
				TrainState = TRAIN_STATES::PossessableTrain3; // Train yard
			else if (TrackSwappingManager->IsSwapperActivated(2))
			{
				if (TrackSwappingManager->IsSwapperActivated(3))
					TrainState = TRAIN_STATES::PossessableTrain5; // Track rotator controls
				else
					TrainState = TRAIN_STATES::PossessableTrain4; // To the carriage

			}
			else
				TrainState = TRAIN_STATES::PossessableTrain2;
		}
		else
			TrainState = TRAIN_STATES::PossessableTrain;
		break;

	case 1:
		TrainState = TRAIN_STATES::PossessableTrain2;
		if (TrackSwappingManager->IsSwapperActivated(1))
			TrainState = TRAIN_STATES::PossessableTrain3; // Train yard
		else if (TrackSwappingManager->IsSwapperActivated(2))
		{
			if (TrackSwappingManager->IsSwapperActivated(3))
				TrainState = TRAIN_STATES::PossessableTrain5; // Track rotator controls
			else
				TrainState = TRAIN_STATES::PossessableTrain4; // To the carriage
		}
		break;

	case 2:
		TrainState = TRAIN_STATES::PossessableTrain2;
		if(TrackSwappingManager->IsSwapperActivated(2))
		{
			if (TrackSwappingManager->IsSwapperActivated(3))
				TrainState = TRAIN_STATES::PossessableTrain5; // Track rotator controls
			else
				TrainState = TRAIN_STATES::PossessableTrain4; // To the carriage
		}
		break;

	case 3:
		if (TrainState == TRAIN_STATES::PossessableTrain4)
			TrainState = TRAIN_STATES::PossessableTrain5;
		else
			TrainState = TRAIN_STATES::PossessableTrain4;
		break;

	case 4:
	{
		FRotator Rotation = RotatingTrack->GetActorRotation();
		Rotation.Yaw += 90.0f;
		RotatingTrack->SetActorRotation(FQuat(Rotation));
		break;
	}
	default:
		UE_LOG(LogTemp, Warning, TEXT("AToyTrain::SwapTrack has attempted to use an invalid Track Swapper"));
		break;
	}
}

// Called to bind functionality to input
void AToyTrain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("CarMoveForward", this, &AToyTrain::MoveForward);

	PlayerInputComponent->BindAction("Posses", IE_Released, this, &AToyTrain::ChangePossesion);

	PlayerInputComponent->BindAction("TrainSwapTrack", IE_Released, this, &AToyTrain::SwapTrack);
}

void AToyTrain::MoveForward(float fValue)
{
	MovementDirection = (int)fValue;
}

void AToyTrain::ChangePossesion()
{
	if (PossessionChangerManager)
		PossessionChangerManager->CheckPossessionPads();
}

void AToyTrain::SwapTrack()
{
	int SplineToSwap = TrackSwappingManager->GetNearestSwapper(this);

	if (SplineToSwap != -1)
	{
		SetTrainStateToChangeTo(SplineToSwap);
	}
}

void AToyTrain::TrackSwitcherHit(int TrackSwitchNumber)
{
	TrackSwitched[TrackSwitchNumber] = true;
}
