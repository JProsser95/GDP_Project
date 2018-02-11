// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyTrain.h"
#include "ToyCar.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "GDP_ProjectGameModeBase.h"
#include "Macros.h"

const int HEIGHT = 0;//height of player above spline

#define CARRIAGESPACING 30
#define FIRSTLINELENGTH 300

// Sets default values
AToyTrain::AToyTrain()
	: splinePointer(0), Rotating(false), LineSwapped(false), MovementDirection(0)
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

	ToyCar = nullptr;

	isActive = true;
}

// Called when the game starts or when spawned
void AToyTrain::BeginPlay()
{
	Super::BeginPlay();

	for (TObjectIterator<USplineComponent> SplineComponent; SplineComponent; ++SplineComponent)
	{
		int numberOfSplinePoints = SplineComponent->GetNumberOfSplinePoints();
		float totalLength = SplineComponent->GetSplineLength();

		float currentLength = 0;
		int sampleLength = 5; //we will sample the spline every "sampleLength" units

		FRotator splinePointRotation = FRotator(0, 0, 0);

		if (numberOfSplinePoints > 5) {//you can also use GetName() to select the spline component you want to process

			int splinePointCount = 0;

			while (currentLength < totalLength) {

				//The next line samples the spline at "currentLength" units from the starting point
				FTransform splinePointTransform = SplineComponent->GetTransformAtDistanceAlongSpline(currentLength, ESplineCoordinateSpace::World);

				currentLength += sampleLength;//increase "currentLength" for the next sample

				pathPointRotation[splinePointCount] = splinePointTransform.GetRotation();

				FVector up = HEIGHT * pathPointRotation[splinePointCount].GetAxisZ();//this vector is above the spline by 300 units

				pathPointLocation[splinePointCount] = splinePointTransform.GetLocation() + up;//this will be used to place the player (i.e. the space ship if you don't change the model)

				splinePointRotation = FRotator(pathPointRotation[splinePointCount]);

				splinePointCount += 1;
			}
			totalSplinePoints = splinePointCount;
		}
	}

	FirstLine = StartingPosition->GetActorLocation() - pathPointLocation[0];
	
	float angle = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(FirstLine.GetSafeNormal(), FVector::ForwardVector)));
	
	RootComponent->SetWorldLocation(StartingPosition->GetActorLocation());
	RootComponent->SetWorldRotation(FRotator(0.0f, -angle, 0.0f));
}

void AToyTrain::Restart()
{
	Super::Restart();

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->ChangeHUD("ToyTrain");
}

// Called every frame
void AToyTrain::Tick(float DeltaTime)
{
	if (!isActive)
		return;

	Super::Tick(DeltaTime);

	static float SplineTimer = 0.0f;

	if (MovementDirection)
	{
		SplineTimer += DeltaTime;
		if (SplineTimer >= 0.01f)
		{
			SplineTimer = 0.0f;
			UpdateSplinePointer();
		}
	}
	else
		SplineTimer = 0.0f;

	if (!Rotating)
	{
		if (LineSwapped)
		{
			UpdateTrainOnSpline();
		}
		else
		{
			UpdateTrainOnVector();

			if (splinePointer == FIRSTLINELENGTH)
			{
				Rotating = true;
			}
		}
	}
	else
	{
		RootComponent->SetWorldRotation(FRotator(0.0f, RootComponent->GetComponentRotation().Yaw + (30.0f * DeltaTime), 0.0f));
		if (FMath::Abs(RootComponent->GetComponentRotation().Yaw - pathPointRotation[0].Rotator().Yaw) < 1.0f)
		{
			Rotating = false;
			LineSwapped = true;
			splinePointer = 0;
		}
	}

	//End movement at end of Spline
	if (MeshComponent->IsOverlappingActor(TrainHouse))
	{
		CompleteTrainPuzzle();
	}

	//UE_LOG(LogTemp, Warning, TEXT("%s"), *pathPointLocation[splinePointer].ToString());
}

void AToyTrain::UpdateSplinePointer()
{
	if (MovementDirection == 1)
	{
		if (LineSwapped)
		{
			if (!MeshComponent->IsOverlappingActor(Obstacle))
			{
				if (++splinePointer >= totalSplinePoints)
					splinePointer = totalSplinePoints;
			}
		}
		else
		{
			if (--splinePointer < 0)
				splinePointer = 0;
		}
	}
	else
	{
		if (LineSwapped)
		{
			if (--splinePointer < 0)
				splinePointer = 0;
		}
		else
		{
			if (++splinePointer > FIRSTLINELENGTH)
				splinePointer = FIRSTLINELENGTH;
		}
	}
}

void AToyTrain::UpdateTrainOnVector()
{
	RootComponent->SetWorldLocation(StartingPosition->GetActorLocation() - ((FirstLine / FIRSTLINELENGTH) * splinePointer)); // The movement is a minus because the vector is backwards to make the train move in reverse.
}

void AToyTrain::UpdateTrainOnSpline()
{
	RootComponent->SetWorldLocation(pathPointLocation[splinePointer]);//just move the player to the next sampled point on the spline
	RootComponent->SetWorldRotation(pathPointRotation[splinePointer]);//and give the player the same rotation as the sampled point

	UpdateCarriages();
}

void AToyTrain::UpdateCarriages()
{
	int carriageSplinePointer = 0;
	for (int i = 0; i < Carriages.Num(); ++i)
	{
		carriageSplinePointer = splinePointer - (CARRIAGESPACING * (i + 1));
		if (carriageSplinePointer >= 0)
		{
			Carriages[i]->SetActorLocation(pathPointLocation[carriageSplinePointer]);
			Carriages[i]->SetActorRotation(pathPointRotation[carriageSplinePointer]);
		}
		else
		{
			Carriages[i]->SetActorLocation(RootComponent->RelativeLocation - FVector(150.0f, 0.0f, 0.0f));
			Carriages[i]->SetActorRotation(pathPointRotation[0]);
		}
	}
}

void AToyTrain::CompleteTrainPuzzle()
{
	OUTPUT_STRING("END");
	ChangePossesion();
}

// Called to bind functionality to input
void AToyTrain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("CarMoveForward", this, &AToyTrain::MoveForward);

	PlayerInputComponent->BindAction("Posses", IE_Released, this, &AToyTrain::ChangePossesion);
}

void AToyTrain::SetIsActive(bool Value)
{
	isActive = Value;
}

void AToyTrain::SetToyCar(APawn* TC)
{
	ToyCar = TC;
}

void AToyTrain::MoveForward(float fValue)
{
	MovementDirection = (int)fValue;
}

void AToyTrain::ChangePossesion()
{
	isActive = false;
	if (ToyCar != nullptr)
	{
		GetWorld()->GetFirstPlayerController()->Possess(ToyCar);
		AToyCar* TC = Cast<AToyCar>(ToyCar);
		if (TC != nullptr)
		{
			TC->SetIsActive(true);
		}
	}
}
