// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyTrain.h"
#include "ToyCar.h"
#include "EngineUtils.h"
#include "UObject/ConstructorHelpers.h"
#include "GDP_ProjectGameModeBase.h"
#include "Macros.h"

const int HEIGHT = 0;//height of player above spline
const int MAXPOINTS = 5000;//stop sampling the spline after MAXPOINTS points
FVector pathPointLocation[MAXPOINTS];//save sampled point locations into an array
FQuat pathPointRotation[MAXPOINTS];//save sampled point rotations into an array
int totalSplinePoints = 0; //After we sampled the spline at intervals, this is the total number of sampled points on the curve
int splinePointer = 1;//this counter is incremented in the Tick() function to move us to the next point on the spline

// Sets default values
AToyTrain::AToyTrain()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Game/Train/TrainSingle_low.TrainSingle_low'"));
	if (MeshAssetBody.Object)
		MeshComponent->SetStaticMesh(MeshAssetBody.Object);

	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-30.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 700.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	//Take control of the default Player
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	ToyCar = nullptr;

	isActive = false;
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

	RootComponent->SetWorldLocation(pathPointLocation[splinePointer]);//just move the player to the next sampled point on the spline
	RootComponent->SetWorldRotation(pathPointRotation[splinePointer]);//and give the player the same rotation as the sampled point

	splinePointer += 1;

	//Loop the train movement
	//if (splinePointer >= totalSplinePoints)
	//{
	//	splinePointer = 1;
	//}

	//End movement at end of Spline
	if (splinePointer >= totalSplinePoints)
	{
		OUTPUT_STRING("END");
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
}

// Called to bind functionality to input
void AToyTrain::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AToyTrain::SetIsActive(bool Value)
{
	isActive = Value;
}

void AToyTrain::SetToyCar(APawn* TC)
{
	ToyCar = TC;
}