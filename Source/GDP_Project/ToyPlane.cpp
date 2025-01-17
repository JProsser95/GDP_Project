// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyPlane.h"
#include "Blueprint/UserWidget.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
//#include "Macros.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "CustomMovementComponent.h"
#include "PossessableActorComponent.h"
#include "AchievementManager.h"
#include "EngineUtils.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Runtime/Engine/Classes/Engine/StaticMesh.h"


// Sets default values
AToyPlane::AToyPlane()
	:MinSpeed(400.0f), MaxSpeed(600.0f), MaxBoostSpeed(800.0f), SpeedIncrement(100.0f), BoostSpeedIncrement(200.0f), PitchAmount(90.0f), YawAmount(90.0f), RollAmount(90.0f), PropRotateSpeed(3.0f),
	MaximumBoost(100.0f), CurrentBoost(0.0f), MovementInput(0.0f), FrameHack(0),
	RotationInterpolation(0.03f),
	AutoFocus(true), AutoFocusDelay(1.0f), fLastUnFocusTime(-AutoFocusDelay),
	bAlreadyRestarted(false), SWControlPitch(false), SWControlPrevious(SWControlPitch), SwapYawAndRoll(false), PitchInverted(false),
	m_eControlType(Controls::ASDW_Simple), m_ePreviousControlType(m_eControlType),
	m_bCrashed(false), m_fCrashTime(0.0f), m_fRespawnTime(3.0f)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RotationInterpolation *= 60.0f;

	//Create our components

	//Mesh
	PlaneBodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlaneBodyMeshComponent"));
	PlaneBodyMeshComponent->SetRelativeRotation(FRotator(13.5f, 0.0f, 0.0f));
	possComponent = CreateDefaultSubobject<UPossessableActorComponent>(TEXT("PossessableComponent"));
	PlaneBodyMeshComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);

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
	OurCameraSpringArm->TargetArmLength = 150.0f;
	OurCameraSpringArm->bEnableCameraLag = true;
	OurCameraSpringArm->CameraLagSpeed = 20.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);
	OurCamera->SetFieldOfView(90.0f);

	eCameraType = THIRD_PERSON;

	fSpeed = 0.0f;
	IsBoosting = false;
	bIsActive = false;
	fPropRotation = 0.0f;

	//Take control of the default Player
	//AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create an instance of our movement component, and tell it to update our root component.
	CustomMovementComponent = CreateDefaultSubobject<UCustomMovementComponent>(TEXT("CustomMovementComponent"));
	CustomMovementComponent->UpdatedComponent = RootComponent;

	// Create an audio component, the audio component wraps the Cue
	PlaneAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("PlaneAudioComponent"));
	PlaneAudioComponent->VolumeMultiplier = 1.0f;

	PlaneAudioComponent->bAutoActivate = false;

	// I want the sound to follow the pawn around, so I attach it to the Pawns root.
	PlaneAudioComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USoundCue> planeSound(TEXT("/Game/Sounds/Plane_Propeller_Cue"));

	PlaneAudioComponent->SetSound(planeSound.Object);

}

void AToyPlane::Restart()
{
	Super::Restart();

	if (bAlreadyRestarted)
	{
		bIsActive = true;
		PlaneAudioComponent->Play();
	}
	else
		bAlreadyRestarted = true;

}

// Called when the game starts or when spawned
void AToyPlane::BeginPlay()
{
	Super::BeginPlay();
	startTransform = GetTransform();
}

// Called every frame
void AToyPlane::Tick(float DeltaTime)
{
	if (!bIsActive)
		return;

	//OUTPUT_INT(int(PlaneBodyMeshComponent->IsSimulatingPhysics()));
	//OUTPUT_INT(int(RootComponent->IsSimulatingPhysics()));

	UpdateCamera(DeltaTime);

	if (!m_bCrashed && CustomMovementComponent->HitObject())
	{
		//if (PlaneBodyMeshComponent->IsSimulatingPhysics())
		//	return;
		if (fSpeed > 0.0f)
		{
			PlaneBodyMeshComponent->SetSimulatePhysics(true);
			m_bCrashed = true;
			m_fCrashTime = 0.0f;

			OurCameraSpringArm->bInheritPitch = false;
			OurCameraSpringArm->bInheritRoll = false;
			OurCameraSpringArm->bInheritYaw = true;
			return;
		}
	}
	else if (m_bCrashed)
	{
		m_fCrashTime += DeltaTime;

		if (m_fCrashTime >= m_fRespawnTime)
			ResetPlane();
		return;
	}
	else if (CustomMovementComponent->HitWindow())
	{
		FrameHack = 3;
		PlaneBodyMeshComponent->SetSimulatePhysics(true);
	}
	else if (FrameHack > 0)
		--FrameHack;
	else
		PlaneBodyMeshComponent->SetSimulatePhysics(false);

	Super::Tick(DeltaTime);

	if (m_eControlType != m_ePreviousControlType || SWControlPrevious != SWControlPitch)
	{
		m_ePreviousControlType = m_eControlType;
		SWControlPrevious = SWControlPitch;
		SetupInput();
	}

	if (CurrentBoost <= 0)
		IsBoosting = false;
	else if (CurrentBoost > MaximumBoost)
		CurrentBoost = MaximumBoost;


	/*if (IsBoosting)
	{
		fSpeed += DeltaTime * BoostSpeedIncrement;
		UpdateCurrentBoost(-DeltaTime * 0.3f * MaximumBoost);
		//FlyTowards(FVector(7000.0f, -12000.0f, 1540.0f), DeltaTime);
	}
	else*/ if (fSpeed <= MinSpeed)
	{
		fSpeed += DeltaTime * SpeedIncrement;
		//fSpeed -= DeltaTime * SpeedIncrement;
		//UpdateCurrentBoost(DeltaTime * 0.2f * MaximumBoost);
	}
	else
	{
		fSpeed += MovementInput.W * SpeedIncrement * DeltaTime;
		if (!IsBoosting)
			fSpeed = FMath::Clamp(fSpeed, MinSpeed, MaxSpeed);
		else
			fSpeed = FMath::Clamp(fSpeed, MinSpeed, MaxBoostSpeed);

	}

	////Scale our movement input axis values by 100 units per second
	//MovementInput = MovementInput.GetSafeNormal();// *100.0f;
	FRotator NewRotation(GetActorRotation());

	FRotator rot1(0.0f);
	FRotator rot2(0.0f);
	FRotator rot3(0.0f);

	InterpolateMovementInput(DeltaTime);

	rot1.Pitch = MovementInput.X * PitchAmount * DeltaTime;
	if (m_eControlType != Controls::ASDW_Simple)
	{
		rot2.Yaw = MovementInput.Y * YawAmount * DeltaTime;
		rot3.Roll = MovementInput.Z * RollAmount * DeltaTime;
		NewRotation = UKismetMathLibrary::ComposeRotators(rot3, NewRotation);
		NewRotation = UKismetMathLibrary::ComposeRotators(rot1, NewRotation);
		NewRotation = UKismetMathLibrary::ComposeRotators(rot2, NewRotation);
	}
	else
	{
		//rot2.Yaw = MovementInput.Z * YawAmount * DeltaTime;
		//rot3.Roll = MovementInput.Z * RollAmount * DeltaTime;
		NewRotation = UKismetMathLibrary::ComposeRotators(rot1, NewRotation); // += MovementInput.X * -PitchAmount * DeltaTime;
		NewRotation.Yaw += MovementInput.Z * YawAmount * DeltaTime;
		NewRotation.Roll = NewRotation.Roll + ((MovementInput.Z * RollAmount*0.5f) - NewRotation.Roll) * DeltaTime * 2.2f;

		//if (NewRotation.Roll && !MovementInput.Z)
		//{
		//	NewRotation.Roll *= 0.975f;
		//}
	}

	SetActorRotation(NewRotation);
	//SetActorLocation(NewLocation);

	CustomMovementComponent->AddInputVector(GetActorForwardVector() * fSpeed);

	fPropRotation += DeltaTime * fSpeed * PropRotateSpeed;
	PlanePropMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, fPropRotation));

}

void AToyPlane::UpdateCamera(float DeltaTime)
{
	if (eCameraType == THIRD_PERSON)
	{
		if (AutoFocus && (GetWorld()->GetTimeSeconds() - fLastUnFocusTime >= AutoFocusDelay))
		{
			if (CameraRotationOffset.Yaw <= 180.0f)
				CameraRotationOffset = CameraRotationOffset + (FRotator(0.0f, 0.0f, 0.0f) - CameraRotationOffset) * DeltaTime;
			else
				CameraRotationOffset = CameraRotationOffset + (FRotator(0.0f, 360.0f, 0.0f) - CameraRotationOffset) * DeltaTime;
		}

		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(-10.0f + CameraRotationOffset.Pitch, CameraRotationOffset.Yaw, 0.0f));
	}
	else
	{
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(10.0f, 0.0f, 17.0f), FRotator(CameraRotationOffset.Pitch, CameraRotationOffset.Yaw, 0.0f));
	}

	CameraRotationOffset.Yaw += CameraInput.X;

	if (CameraRotationOffset.Yaw >= 360.0f)
		CameraRotationOffset.Yaw -= 360.0f;
	else if (CameraRotationOffset.Yaw < 0.0f)
		CameraRotationOffset.Yaw += 360.0f;

	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch + CameraInput.Y, -70.0f, 15.0f);
}

UPawnMovementComponent* AToyPlane::GetMovementComponent() const
{
	return CustomMovementComponent;
}

// Called to bind functionality to input
void AToyPlane::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	m_PlayerInput = PlayerInputComponent;

	SetupInput();

}

void AToyPlane::SetupInput()
{
	m_PlayerInput->ClearActionBindings();

	//PlayerInputComponent->BindAction("PlaneBoost", IE_Pressed, this, &AToyPlane::StartBoost);
	//PlayerInputComponent->BindAction("PlaneBoost", IE_Released, this, &AToyPlane::EndBoost);
	m_PlayerInput->BindAction("PlaneCameraZoom", IE_Released, this, &AToyPlane::CameraZoom);

	m_PlayerInput->BindAxis("PlaneCameraPitch", this, &AToyPlane::PitchCamera);
	m_PlayerInput->BindAxis("PlaneCameraYaw", this, &AToyPlane::YawCamera);
	m_PlayerInput->BindAxis("PlanePitch", this, &AToyPlane::Pitch);
	//m_PlayerInput->BindAxis("PlanePitchAltArrows", this, &AToyPlane::Pitch);
	m_PlayerInput->BindAxis("PlaneYaw", this, &AToyPlane::Yaw);
}

void AToyPlane::RotateDown(float DeltaTime)
{
	FRotator oldRotation(GetActorRotation());
	FRotator targetRotation(-90.0f, oldRotation.Yaw, oldRotation.Roll);
	SetActorRotation(FQuat::FastLerp(oldRotation.Quaternion(), targetRotation.Quaternion(), DeltaTime));
}

void AToyPlane::FlyTowards(FVector targetPosition, float DeltaTime)
{
	FRotator targetRotation = (targetPosition - GetActorLocation()).Rotation();
	SetActorRotation(FQuat::FastLerp(GetActorRotation().Quaternion(), targetRotation.Quaternion(), DeltaTime*4.0f));
}

void AToyPlane::InterpolateMovementInput(float DeltaTime)
{
	MovementInput.Y = FMath::Lerp(MovementInput.Y, TargetInput.Z, RotationInterpolation * DeltaTime);
	MovementInput.Z = FMath::Lerp(MovementInput.Z, TargetInput.Y, RotationInterpolation * DeltaTime);
	
	MovementInput.X = FMath::Lerp(MovementInput.X, TargetInput.X, RotationInterpolation * DeltaTime);
	MovementInput.W = FMath::Lerp(MovementInput.W, TargetInput.W, RotationInterpolation * DeltaTime);
}

void AToyPlane::RegisterInput(float AxisValue, float& input)
{
	input = FMath::Clamp<float>(AxisValue, -1.0f, 1.0f);
}

//Input functions
void AToyPlane::Pitch(float AxisValue)
{
	if (PitchInverted)
		AxisValue *= -1.0f;

	RegisterInput(AxisValue, TargetInput.X);
}

void AToyPlane::Yaw(float AxisValue)
{
	RegisterInput(AxisValue, TargetInput.Y);
}

void AToyPlane::Roll(float AxisValue)
{
	RegisterInput(AxisValue, TargetInput.Z);
}

void AToyPlane::Throttle(float AxisValue)
{
	// modify speed or plane pitch
	RegisterInput(AxisValue, TargetInput.W);
}

void AToyPlane::PitchCamera(float AxisValue)
{
	AxisValue = -AxisValue;
	if (AxisValue != 0.0f)
		fLastUnFocusTime = GetWorld()->GetTimeSeconds();

	// Pitch camera or pitch plane
	RegisterInput(AxisValue, CameraInput.Y);

}

void AToyPlane::YawCamera(float AxisValue)
{
	if (AxisValue != 0.0f)
		fLastUnFocusTime = GetWorld()->GetTimeSeconds();

	// Rotate camera or pitch plane
	RegisterInput(AxisValue, CameraInput.X);

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
		OurCameraSpringArm->TargetArmLength = 100.0f;
		OurCameraSpringArm->bEnableCameraLag = true;
	}
	else {
		eCameraType = FIRST_PERSON;
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(-10.0f, 0.0f, 15.0f), FRotator(0.0f, 0.0f, 0.0f));
		OurCameraSpringArm->TargetArmLength = 0.0f;
		OurCameraSpringArm->bEnableCameraLag = false;
	}
}

void AToyPlane::ResetPlane()
{
	for (TActorIterator<AAchievementManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		ActorItr->EarnAchievement(AchievementName::PLANE_CRASH);
	}
	m_bCrashed = false;
	PlaneBodyMeshComponent->SetSimulatePhysics(false);
	MovementInput = FVector4(0.0f);
	fSpeed = 0.0f;

	OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(10.0f, 0.0f, 17.0f), FRotator(CameraRotationOffset.Pitch, CameraRotationOffset.Yaw, 0.0f));
	OurCameraSpringArm->bInheritPitch = true;
	OurCameraSpringArm->bInheritRoll = true;
	OurCameraSpringArm->bInheritYaw = true;

	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch + CameraInput.Y, -70.0f, 15.0f);
	SetActorTransform(startTransform, false, nullptr, ETeleportType::TeleportPhysics);
	OurCameraSpringArm->CameraLagSpeed = 20.0f;
	
}

void AToyPlane::UpdateCurrentBoost(float boostIncrement)
{
	CurrentBoost += boostIncrement;
	MinSpeed += 5.0f * boostIncrement;
	MaxSpeed += 5.0f * boostIncrement;
	if (CurrentBoost >= 100.0f)
		if (WindowCollision != nullptr)
			WindowCollision->Destroy();
}

void AToyPlane::SetIsActive(bool Value)
{
	bIsActive = Value;
}

