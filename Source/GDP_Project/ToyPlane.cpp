// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyPlane.h"
#include "Blueprint/UserWidget.h"
#include "GDP_ProjectGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Macros.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "CustomMovementComponent.h"
#include "PossessableActorComponent.h"

// Sets default values
AToyPlane::AToyPlane()
	:MinSpeed(400.0f), MaxSpeed(600.0f), CamShakeSpeed(500.0f), SpeedIncrement(100.0f), BoostSpeedIncrement(200.0f), PitchAmount(90.0f), YawAmount(90.0f), RollAmount(90.0f), PropRotateSpeed(3.0f),
	MaximumBoost(100.0f), CurrentBoost(0.0f), MovementInput(0.0f),
	RotationInterpolation(0.1f),
	AutoFocus(true), AutoFocusDelay(1.0f), fLastUnFocusTime(-AutoFocusDelay),
	bAlreadyRestarted(false)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//	PlaneBodyMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AToyPlane::OnToyPlaneOverlap);
	//	PlaneBodyMeshComponent->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);

	RotationInterpolation *= 60.0f;

	//Create our components

	//Mesh
	PlaneBodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneBodyMeshComponent"));
	PlaneBodyMeshComponent->SetRelativeRotation(FRotator(13.5f, 0.0f, 0.0f));
	possComponent = CreateDefaultSubobject<UPossessableActorComponent>(TEXT("PossessableComponent"));
	PlaneBodyMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	//PlaneBodyMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AToyPlane::OnToyPlaneOverlap);
	//PlaneBodyMeshComponent->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);
	//static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialPlane(TEXT("MaterialInstanceDynamic'/Game/Plane/Texture/Plane_Material.Plane_Material'"));
	//if (MaterialPlane.Object)
	//	PlaneBodyMeshComponent->SetMaterial(0, MaterialPlane.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBody(TEXT("StaticMesh'/Game/Plane/Plane_Plane.Plane_Plane'"));
	if (MeshAssetBody.Object)
		PlaneBodyMeshComponent->SetStaticMesh(MeshAssetBody.Object);

	RootComponent = PlaneBodyMeshComponent;

	//Mesh
	PlanePropMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlanePropMeshComponent"));
	PlanePropMeshComponent->AttachToComponent(PlaneBodyMeshComponent, FAttachmentTransformRules::KeepWorldTransform);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAssetBProp(TEXT("StaticMesh'/Game/Plane/Plane_Prop.Plane_Prop'"));
	if (MeshAssetBProp.Object)
		PlanePropMeshComponent->SetStaticMesh(MeshAssetBProp.Object);
	PlanePropMeshComponent->SetRelativeLocation(FVector(45.0f, 0.0f, 2.0f));

	OurCameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	OurCameraSpringArm->SetupAttachment(RootComponent);
	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(-20.0f, 0.0f, 0.0f));
	OurCameraSpringArm->TargetArmLength = 100.f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 20.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

	eCameraType = THIRD_PERSON;

	fSpeed = 0.0f;
	IsBoosting = false;
	bIsActive = true;
	fPropRotation = 0.0f;

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create an instance of our movement component, and tell it to update our root component.
	CustomMovementComponent = CreateDefaultSubobject<UCustomMovementComponent>(TEXT("CustomMovementComponent"));
	CustomMovementComponent->UpdatedComponent = RootComponent;
}

void AToyPlane::Restart()
{
	Super::Restart();

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->ChangeHUD("ToyPlane");

	if (bAlreadyRestarted)
		bIsActive = true;
	else
		bAlreadyRestarted = true;

}

// Called when the game starts or when spawned
void AToyPlane::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AToyPlane::Tick(float DeltaTime)
{
	if (!bIsActive)
		return;

	Super::Tick(DeltaTime);

	if (CurrentBoost <= 0)
		IsBoosting = false;
	else if (CurrentBoost > MaximumBoost)
		CurrentBoost = MaximumBoost;

	if (IsBoosting) 
	{
		fSpeed += DeltaTime * BoostSpeedIncrement;
		UpdateCurrentBoost(-DeltaTime * 0.3f * MaximumBoost);

	}
	else if (fSpeed <= MinSpeed)
	{
		fSpeed += DeltaTime * SpeedIncrement;
		//fSpeed -= DeltaTime * SpeedIncrement;
		//UpdateCurrentBoost(DeltaTime * 0.2f * MaximumBoost);
	}
	else
	{
		fSpeed += MovementInput.W * SpeedIncrement * DeltaTime;
		fSpeed = FMath::Clamp(fSpeed, MinSpeed, MaxSpeed);
	}

	if (fSpeed >= CamShakeSpeed) 
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		PC->ClientPlayCameraShake(CameraShake, 1);
	}

	////Scale our movement input axis values by 100 units per second
	//MovementInput = MovementInput.GetSafeNormal();// *100.0f;
	FRotator NewRotation(GetActorRotation());

	float fRotateMod(fSpeed / MaxSpeed);
	if (fRotateMod > 1.0f)
		fRotateMod = 1.0f;

	FRotator rot1(0.0f);
	FRotator rot2(0.0f);
	FRotator rot3(0.0f);

	InterpolateMovementInput(DeltaTime);

	rot1.Pitch = MovementInput.X * PitchAmount * DeltaTime;
	rot2.Yaw = MovementInput.Y * YawAmount * DeltaTime;
	rot3.Roll = MovementInput.Z * RollAmount * DeltaTime;

	NewRotation = UKismetMathLibrary::ComposeRotators(rot3, NewRotation);
	NewRotation = UKismetMathLibrary::ComposeRotators(rot1, NewRotation);
	NewRotation = UKismetMathLibrary::ComposeRotators(rot2, NewRotation);

	SetActorRotation(NewRotation);
	//SetActorLocation(NewLocation);

	CustomMovementComponent->AddInputVector(GetActorForwardVector() * fSpeed);

	fPropRotation += DeltaTime * fSpeed * PropRotateSpeed;
	PlanePropMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, fPropRotation));

	UpdateCamera(DeltaTime);
}

void AToyPlane::UpdateCamera(float DeltaTime)
{
	if (AutoFocus && (GetWorld()->GetTimeSeconds() - fLastUnFocusTime >= AutoFocusDelay))
		CameraRotationOffset = CameraRotationOffset + (FRotator(0.0f, 0.0f, 0.0f) - CameraRotationOffset) * DeltaTime;
	if (eCameraType == THIRD_PERSON)
	{
		if (AutoFocus && (GetWorld()->GetTimeSeconds() - fLastUnFocusTime >= AutoFocusDelay))
			CameraRotationOffset = CameraRotationOffset + (FRotator(0.0f, 0.0f, 0.0f) - CameraRotationOffset) * DeltaTime;

		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(-20.0f + CameraRotationOffset.Pitch, CameraRotationOffset.Yaw, 0.0f));
	}
	else
	{
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(10.0f, 0.0f, 17.0f), FRotator(CameraRotationOffset.Pitch, CameraRotationOffset.Yaw, 0.0f));
	}
	CameraRotationOffset.Yaw += CameraInput.X;
	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch + CameraInput.Y, -70.0f, 15.0f);
}

UPawnMovementComponent* AToyPlane::GetMovementComponent() const
{
	return CustomMovementComponent;
}

void AToyPlane::OnToyPlaneOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{

}

// Called to bind functionality to input
void AToyPlane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction("PlaneBoost", IE_Pressed, this, &AToyPlane::StartBoost);
	//PlayerInputComponent->BindAction("PlaneBoost", IE_Released, this, &AToyPlane::EndBoost);
	PlayerInputComponent->BindAction("PlaneCameraZoom", IE_Released, this, &AToyPlane::CameraZoom);

	//Hook up every-frame handling for our four axes
	//PlayerInputComponent->BindAxis("PlaneMoveUp", this, &AToyPlane::MoveUp);
	//PlayerInputComponent->BindAxis("PlaneMoveRight", this, &AToyPlane::MoveRight);
	PlayerInputComponent->BindAxis("PlaneCameraPitch", this, &AToyPlane::PitchCamera);
	PlayerInputComponent->BindAxis("PlaneCameraYaw", this, &AToyPlane::YawCamera);
	PlayerInputComponent->BindAxis("PlanePitch", this, &AToyPlane::Pitch);
	PlayerInputComponent->BindAxis("PlaneYaw", this, &AToyPlane::Yaw);
	PlayerInputComponent->BindAxis("PlaneRoll", this, &AToyPlane::Roll);
	PlayerInputComponent->BindAxis("PlaneThrottle", this, &AToyPlane::Throttle);

}

void AToyPlane::RotateDown(float DeltaTime)
{
	FRotator oldRotation(GetActorRotation());
	FRotator targetRotation(-90.0f, oldRotation.Yaw, oldRotation.Roll);
	SetActorRotation(FQuat::FastLerp(oldRotation.Quaternion(), targetRotation.Quaternion(), DeltaTime));
}

void AToyPlane::InterpolateMovementInput(float DeltaTime)
{
	MovementInput.X = FMath::Lerp(MovementInput.X, TargetInput.X, RotationInterpolation * DeltaTime);
	MovementInput.Y = FMath::Lerp(MovementInput.Y, TargetInput.Y, RotationInterpolation * DeltaTime);
	MovementInput.Z = FMath::Lerp(MovementInput.Z, TargetInput.Z, RotationInterpolation * DeltaTime);
}

//Input functions
void AToyPlane::Pitch(float AxisValue)
{
	TargetInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AToyPlane::Yaw(float AxisValue)
{
	TargetInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AToyPlane::Roll(float AxisValue)
{
	TargetInput.Z = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

void AToyPlane::Throttle(float AxisValue)
{
	//TargetInput.W = FMath::Clamp<float>(MovementInput.X, -1.0f, 1.0f);
	MovementInput.W = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

//void AToyPlane::MoveUp(float AxisValue)
//{
//	MovementInput.X = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
//}
//
//void AToyPlane::MoveRight(float AxisValue)
//{
//	MovementInput.Y = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
//}
//
void AToyPlane::PitchCamera(float AxisValue)
{
	if (AxisValue != 0.0f)
		fLastUnFocusTime = GetWorld()->GetTimeSeconds();
	CameraInput.Y = AxisValue;
}

void AToyPlane::YawCamera(float AxisValue)
{
	if (AxisValue != 0.0f)
		fLastUnFocusTime = GetWorld()->GetTimeSeconds();
	CameraInput.X = AxisValue;
}

void AToyPlane::StartBoost() 
{
	if (CurrentBoost > 0.0f) {
		IsBoosting = true;
	}
}

void AToyPlane::EndBoost()
{
	IsBoosting = false;
}

void AToyPlane::CameraZoom()
{
	if (eCameraType == FIRST_PERSON) {
		eCameraType = THIRD_PERSON;
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 30.0f), FRotator(-20.0f, 0.0f, 0.0f));
		OurCameraSpringArm->TargetArmLength = 100.f;
		OurCameraSpringArm->bEnableCameraLag = true;
	}
	else {
		eCameraType = FIRST_PERSON;
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(-10.0f, 0.0f, 15.0f), FRotator(0.0f, 0.0f, 0.0f));
		OurCameraSpringArm->TargetArmLength = 0.0f;
		OurCameraSpringArm->bEnableCameraLag = false;
	}
}

void AToyPlane::UpdateCurrentBoost(float boostIncrement)
{
	CurrentBoost += boostIncrement;
}

void AToyPlane::SetIsActive(bool Value)
{
	bIsActive = Value;
}
	
