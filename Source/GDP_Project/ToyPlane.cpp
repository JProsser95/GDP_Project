// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyPlane.h"


// Sets default values
AToyPlane::AToyPlane()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create our components
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-60.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 700.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 3.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	fPlaneSpeed = 200.0f;

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AToyPlane::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AToyPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Scale our movement input axis values by 100 units per second
	MovementInput = MovementInput.GetSafeNormal() * 100.0f;
	FVector NewLocation = GetActorLocation();
	FRotator NewRotation = GetActorRotation();

	NewLocation += GetActorForwardVector() * DeltaTime * fPlaneSpeed;

	if (!MovementInput.IsZero()) {

		//A or D pressed
		NewLocation += GetActorRightVector() * MovementInput.Y * DeltaTime;
		NewRotation.Roll += MovementInput.Y * DeltaTime;
		NewRotation.Roll = FMath::Clamp(NewRotation.Roll, -30.0f, 30.0f);
		NewRotation.Yaw += MovementInput.Y * DeltaTime;

		//W or S press
		NewRotation.Pitch += MovementInput.X * DeltaTime;
		NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -30.0f, 30.0f);

	}
	else {

		NewRotation = FMath::Lerp(NewRotation, FRotator(0, NewRotation.Yaw, 0), 2.0f * DeltaTime);

	}

	SetActorRotation(NewRotation);
	SetActorLocation(NewLocation);
}

// Called to bind functionality to input
void AToyPlane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Hook up every-frame handling for our four axes
	PlayerInputComponent->BindAxis("PlaneMoveUp", this, &AToyPlane::MoveUp);
	PlayerInputComponent->BindAxis("PlaneMoveRight", this, &AToyPlane::MoveRight);
	PlayerInputComponent->BindAxis("PlaneCameraPitch", this, &AToyPlane::PitchCamera);
	PlayerInputComponent->BindAxis("PlaneCameraYaw", this, &AToyPlane::YawCamera);
}

//Input functions
void AToyPlane::MoveUp(float AxisValue)
{
	MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AToyPlane::MoveRight(float AxisValue)
{
	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AToyPlane::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}

void AToyPlane::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}