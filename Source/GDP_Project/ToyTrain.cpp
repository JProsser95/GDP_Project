// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyTrain.h"
#include "ToyCar.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "Engine/StaticMesh.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"

const int HEIGHT = 0;//height of player above spline

#define CARRIAGESPACING 10
#define TIMETOUPDATETRAIN 0.012f

// Sets default values
AToyTrain::AToyTrain()
	: splinePointer(0), MovementDirection(-1), TrainState(RunawayTrain), m_fStationWaitTime(0.0f), m_bCarriageAttached(false), m_bRotating(false), m_bPlanePartAttached(true)
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

	//OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	//OurCameraSpringArm->SetupAttachment(RootComponent);
	//OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 100.0f), FRotator(-30.0f, 0.0f, 0.0f));
	//OurCameraSpringArm->TargetArmLength = 700.f;
	//OurCameraSpringArm->bEnableCameraLag = true;
	//OurCameraSpringArm->CameraLagSpeed = 3.0f;
	//
	//OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	//OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	StartRaceTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StartRaceTriggerBox"));
	StartRaceTriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	StartRaceTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));
	StartRaceTriggerBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

	StopWaitingAtStationTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("StopWaitingAtStationTriggerBox"));
	StopWaitingAtStationTriggerBox->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	StopWaitingAtStationTriggerBox->SetBoxExtent(FVector(100.0f, 100.0f, 10.0f));
	StopWaitingAtStationTriggerBox->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));

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
			UE_LOG(LogTemp, Error, TEXT("No spline component found in actor %d"), i);
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

	PlanePart->SetActorEnableCollision(false);
	UpdatePlanePartLocation();

	// Get the ToyCar that is now in the scene
	for (TActorIterator<AToyCar> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		m_pToyCar = *ActorItr;
	}

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

	if (MovementDirection != -1)
	{
		UpdateState(); // Check train state and update if neccessary

		// Handle the updating of the spline pointer
		static float SplineTimer = 0.0f;

		if (m_fStationWaitTime == 0.0f)
		{
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
		}
		else
		{
			m_fStationWaitTime -= DeltaTime;
			if (m_fStationWaitTime < 0.0f)
				m_fStationWaitTime = 0.0f;
			else
			{
				TArray<UPrimitiveComponent*> components;

				GetOverlappingComponents(components);

				for (UPrimitiveComponent* pComponent : components)
				{
					if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
					{
						m_fStationWaitTime = 0.0f;
					}
				}
			}
		}

		// Move the train and its carriage
		if (m_bPlanePartAttached)
		{
			if (!m_bRotating)
			{
				UpdateTrainOnSpline();
			}
			else
			{
				RootComponent->SetWorldRotation(FRotator(0.0f, RootComponent->GetComponentRotation().Yaw - (45.0f * DeltaTime), 0.0f));
				RotatingTrack->SetActorRotation(FRotator(0.0f, RootComponent->GetComponentRotation().Yaw - (45.0f * DeltaTime), 0.0f));
				MoveForward(0.0f); // Make sure the train can't move
				splinePointer = 0;
				if (FMath::Abs(RootComponent->GetComponentRotation().Yaw - pathPointRotation[TrainState][splinePointer].Rotator().Yaw) < 1.0f)
				{
					m_bRotating = false;
					SplineTimer = -0.5f;
				}
			}
		}
		UpdatePlanePartLocation();

		//End movement at end of Spline
		if (TrainPuzzleCompleted())
		{
			CompleteTrainPuzzle();
		}
	}
	else
	{
		UpdateTrainOnSpline(); // Incase train hasn't movement previously

		TArray<UPrimitiveComponent*> components;

		GetOverlappingComponents(components);

		for (UPrimitiveComponent* pComponent : components)
		{
			if (pComponent->GetCollisionObjectType() == ECollisionChannel::ECC_Vehicle)
			{
				MovementDirection = 1;
				return;
			}
		}
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
			ChangeToState(RunawayTrain2_Station);
			m_fStationWaitTime = STATIONWAITTIME;
		}
		break;

	case TRAIN_STATES::RunawayTrain2_Station:
		if (!AutomatedMovement())
		{
			ChangeToState(TrackSwitched[1] ? RunawayTrain3 : RunawayTrain2_Failed);
		}
		break;

	case TRAIN_STATES::RunawayTrain3:
		if (!AutomatedMovement() && TrackSwitched[2])
		{
			ChangeToState(RunawayTrain4);
			m_bRotating = true;
		}
		else if(TrackSwitched[2])
		{
			m_pToyCar->SetCanMove(false);
		}
		break;

	case TRAIN_STATES::RunawayTrain4:
		m_bCarriageAttached = true;
		AutomatedMovement();
		if (splinePointer >= 350)
		{
			m_pToyCar->SetCanMove(true);
		}
		break;

	case TRAIN_STATES::PlanePartState:
		AutomatedMovement();
		m_pToyCar->SetPuzzleCompleted(PuzzleName::TRAIN);
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
	if (MovementDirection == 1)
	{
		if (++splinePointer >= pathPointLocation[TrainState].Num() - 1)
			splinePointer = pathPointLocation[TrainState].Num() - 1;
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
	if (!m_bCarriageAttached)
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
			Carriages[i]->SetActorLocation(RootComponent->RelativeLocation - (RootComponent->GetForwardVector() * 50.0f));
			Carriages[i]->SetActorRotation(pathPointRotation[TrainState][0]);
		}
	}

}

void AToyTrain::UpdatePlanePartLocation()
{
	if (m_bPlanePartAttached)
	{
		PlanePart->SetActorLocation(Carriages[0]->GetActorLocation() + FVector(45.0f, 0.0f, 60.0f));
		PlanePart->SetActorRotation(Carriages[0]->GetActorRotation() + FRotator(90.0f, 0.0f, 90.0f));
	}
	else
	{
		PlanePart->SetActorLocation(pathPointLocation[TrainState][splinePointer]);
		PlanePart->SetActorRotation(pathPointRotation[TrainState][splinePointer]);
	}
}

bool AToyTrain::OnFailureTrainLine()
{
	return TrainState == TRAIN_STATES::RunawayTrain_Failed || TrainState == TRAIN_STATES::RunawayTrain2_Failed;
}

bool AToyTrain::OnCompletionTrainLine()
{
	return TrainState == TRAIN_STATES::RunawayTrain4;
}

bool AToyTrain::TrainPuzzleFailed()
{
	return OnFailureTrainLine() && EndOfCurrentLine();
}

bool AToyTrain::TrainPuzzleCompleted()
{
	return OnCompletionTrainLine() && EndOfCurrentLine();
}

void AToyTrain::CompleteTrainPuzzle()
{
	TrainState = PlanePartState;
	splinePointer = 0;
	PlanePart->SetActorEnableCollision(true);
	m_bPlanePartAttached = false;
}

bool AToyTrain::StartOfCurrentLine()
{
	return splinePointer <= 0;
}

bool AToyTrain::EndOfCurrentLine()
{
	return splinePointer >= pathPointLocation[TrainState].Num() - 1;
}

// Called to bind functionality to input
void AToyTrain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("CarMoveForward", this, &AToyTrain::MoveForward);
}

void AToyTrain::MoveForward(float fValue)
{
	MovementDirection = (int)fValue;
}

void AToyTrain::TrackSwitcherHit(int TrackSwitchNumber)
{
	TrackSwitched[TrackSwitchNumber] = true;
}
