// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyCar.h"
#include "ToyCarWheelFront.h"
#include "ToyCarWheelRear.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "UObject/ConstructorHelpers.h"
#include "PossessableActorComponent.h"
#include "EngineUtils.h"
#include "RespawnPoint.h"
#include "Runtime/Engine/Classes/Engine/SpotLight.h"
#include "Runtime/Engine/Classes/Components/LightComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BackgroundMusicManager.h"

AToyCar::AToyCar()
	:// Reset
	RespawnDelay(1.5f), m_fLastRespawn(-RespawnDelay),
	// Anti flip rotation
	MaxAngle(85.0f), RotateSpeed(2.0f), LimitRotation(true),
	// Camera
	CameraRotation(-20.0f, 0.0f, 0.0f), AutoFocus(true), AutoFocusDelay(1.0f), m_fLastUnFocusTime(-AutoFocusDelay),
	m_fTurnAmount(0.0f), m_fTimeOnGround(0.0f),
	CanSeeHints(true), TimerPuzzleCompleted(false), FrictionPuzzleCompleted(false), TrainPuzzleCompleted(false)
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("SkeletalMesh'/Game/Car/TOYCAR.TOYCAR'"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Car/TOYCAR_Skeleton_AnimBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	// Setup friction materials
	//static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> SlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/Slippery.Slippery"));
	//SlipperyMaterial = SlipperyMat.Object;
	//
	//static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> NonSlipperyMat(TEXT("/Game/VehicleAdv/PhysicsMaterials/NonSlippery.NonSlippery"));
	//NonSlipperyMaterial = NonSlipperyMat.Object;

	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	check(Vehicle4W->WheelSetups.Num() == 4);


	float RearForwardDistance(24.0f);
	float FrontForwardDistance(-18.0f);

	float FrontSideDistance(8.0f);
	float RearSideDistance(10.0f);

	float FrontWheelHeight(5.75f);
	float RearWheelHeight(4.0f);

	// Wheels/Tyres
	// Setup the wheels
	Vehicle4W->WheelSetups[0].WheelClass = UToyCarWheelFront::StaticClass();
	Vehicle4W->WheelSetups[0].BoneName = FName("WheelFL");
	Vehicle4W->WheelSetups[0].AdditionalOffset = FVector(FrontForwardDistance, -FrontSideDistance, FrontWheelHeight);

	Vehicle4W->WheelSetups[1].WheelClass = UToyCarWheelFront::StaticClass();
	Vehicle4W->WheelSetups[1].BoneName = FName("WheelFR");
	Vehicle4W->WheelSetups[1].AdditionalOffset = FVector(FrontForwardDistance, FrontSideDistance, FrontWheelHeight);

	Vehicle4W->WheelSetups[2].WheelClass = UToyCarWheelRear::StaticClass();
	Vehicle4W->WheelSetups[2].BoneName = FName("WheelRL");
	Vehicle4W->WheelSetups[2].AdditionalOffset = FVector(RearForwardDistance, RearSideDistance, RearWheelHeight);

	Vehicle4W->WheelSetups[3].WheelClass = UToyCarWheelRear::StaticClass();
	Vehicle4W->WheelSetups[3].BoneName = FName("WheelRR");
	Vehicle4W->WheelSetups[3].AdditionalOffset = FVector(RearForwardDistance, -RearSideDistance, RearWheelHeight);

	// Adjust the tire loading
	Vehicle4W->MinNormalizedTireLoad = 0.0f;
	Vehicle4W->MinNormalizedTireLoadFiltered = 0.2f;
	Vehicle4W->MaxNormalizedTireLoad = 2.0f;
	Vehicle4W->MaxNormalizedTireLoadFiltered = 2.0f;

	// Engine 
	// Torque setup
	Vehicle4W->EngineSetup.MaxRPM = 2000.0f;
	//Vehicle4W->MaxEngineRPM = 2000.0f;
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->Reset();
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(0.0f, 400.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(1890.0f, 420.0f);
	Vehicle4W->EngineSetup.TorqueCurve.GetRichCurve()->AddKey(5730.0f, 400.0f);

	// Adjust the steering 
	Vehicle4W->SteeringCurve.GetRichCurve()->Reset();
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 0.6f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(43.0f, 0.6f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);

	// Transmission	
	// We want 4wd
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.85f;

	// Drive the front wheels a little more than the rear
	//Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

	// Automatic gearbox
	Vehicle4W->TransmissionSetup.bUseGearAutoBox = true;
	Vehicle4W->TransmissionSetup.GearSwitchTime = 0.15f;
	Vehicle4W->TransmissionSetup.GearAutoBoxLatency = 1.0f;

	// THIS HAS TO BE TRUE!!!! Stops the wheels from being ridiculously suspended
	Vehicle4W->bDeprecatedSpringOffsetMode = true;

	// Physics settings
	// Adjust the center of mass - the buggy is quite low
	UPrimitiveComponent* UpdatedPrimitive = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	if (UpdatedPrimitive)
	{
		UpdatedPrimitive->BodyInstance.COMNudge = FVector(8.0f, 0.0f, 0.0f);
	}

	// Set the inertia scale. This controls how the mass of the vehicle is distributed.
	Vehicle4W->InertiaTensorScale = FVector(1.0f, 1.333f, 1.2f);

	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->SetupAttachment(RootComponent);
	SpringArm->TargetArmLength = 200.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 20.0f;

	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.0f;

	//ChangeVehicleWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	//ChangeVehicleWidget->SetupAttachment(CameraParent);
	//static ConstructorHelpers::FClassFinder<UUserWidget> Widget(TEXT("/Game/HUD/VehicleWidget"));
	//ChangeVehicleWidget->SetWidgetClass(Widget.Class);
	//ChangeVehicleWidget->SetRelativeLocation(FVector(0, 0, 150.0f));

	PossessableComponent = CreateDefaultSubobject<UPossessableActorComponent>(TEXT("PossessableComponent"));

	m_bIsLowFriction = false;
	m_bInReverseGear = false;

	m_bIsBraking = false;
	m_bCanMove = true;
	m_fStickyFriction = 1.0f;

	// Load our Sound Cue for the propeller sound we created in the editor... 
	static ConstructorHelpers::FObjectFinder<USoundCue> engineSound(TEXT("/Game/Sounds/Car_Engine_Cue"));
	static ConstructorHelpers::FObjectFinder<USoundCue> breakSound(TEXT("/Game/Sounds/Car_Break_Cue"));

	AudioCues.Add(engineSound.Object);
	AudioCues.Add(breakSound.Object);

	_CurrentRoom = RoomName::WARP_ROOM;

	// Create an audio component, the audio component wraps the Cue
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));

	// I don't want the sound playing the moment it's created.
	AudioComponent->bAutoActivate = false;
	// I want the sound to follow the pawn around, so I attach it to the Pawns root.
	AudioComponent->SetupAttachment(RootComponent);

	//Take control of the default Player
	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

// Called when the game starts or when spawned
void AToyCar::BeginPlay()
{
	Super::BeginPlay();

	AudioComponent->SetSound(AudioCues[ENGINE]);
	currentSoundCue = ENGINE;
	if (GetWorld() != nullptr)
	{
		lightLeft = GetWorld()->SpawnActor<ASpotLight>(ASpotLight::StaticClass());
		lightLeft->SetEnabled(false);
		lightLeft->SetBrightness(50000.0f);
		lightLeft->SetMobility(EComponentMobility::Movable);
		lightLeft->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		lightLeft->SetActorRelativeLocation(FVector(41.0f, -4.0f, 15.0f));
		lightLeft->SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));

		lightRight = GetWorld()->SpawnActor<ASpotLight>(ASpotLight::StaticClass());
		lightRight->SetEnabled(false);
		lightRight->SetBrightness(50000.0f);
		lightRight->SetMobility(EComponentMobility::Movable);
		lightRight->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		lightRight->SetActorRelativeLocation(FVector(41.0f, 16.0f, 15.0f));
		lightRight->SetActorRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	}

	GetWorld()->GetFirstPlayerController()->Possess(this);
}

// Called every frame
void AToyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Setup the flag to say we are in reverse gear
	m_bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	if (m_pHeadlightArea->IsOverlappingActor(this))
	{
		lightLeft->SetEnabled(true);
		lightRight->SetEnabled(true);
	}
	else
	{
		lightLeft->SetEnabled(false);
		lightRight->SetEnabled(false);
	}
	// Update physics material
	//UpdatePhysicsMaterial();

	UpdateCamera(DeltaTime);

	LimitCarRotation(DeltaTime);
}

void AToyCar::Restart()
{
	Super::Restart();

	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->ChangeHUD("ToyCar");
}

// Called to bind functionality to input
void AToyCar::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("CarMoveForward", this, &AToyCar::MoveForward);
	PlayerInputComponent->BindAxis("CarMoveRight", this, &AToyCar::MoveRight);
	PlayerInputComponent->BindAxis("CarCameraPitch", this, &AToyCar::PitchCamera);
	PlayerInputComponent->BindAxis("CarCameraYaw", this, &AToyCar::YawCamera);

	PlayerInputComponent->BindAction("CarHandbrake", IE_Pressed, this, &AToyCar::OnHandbrakePressed);
	PlayerInputComponent->BindAction("CarHandbrake", IE_Released, this, &AToyCar::OnHandbrakeReleased);
	PlayerInputComponent->BindAction("Posses", IE_Released, this, &AToyCar::ChangePossesion);
	//PlayerInputComponent->BindAction("DisplayHUD", IE_Released, this, &AToyCar::ChangeHUD);
	PlayerInputComponent->BindAction("ResetCar", IE_Released, this, &AToyCar::ResetPositionAndRotation);
	PlayerInputComponent->BindAction("RespawnCar", IE_Released, this, &AToyCar::Respawn);
}

void AToyCar::MoveForward(float AxisValue)
{
	if (!m_bIsBraking && m_bCanMove)
	{
		if (AxisValue > 0) {
			if (currentSoundCue != ENGINE) {
				AudioComponent->SetSound(AudioCues[ENGINE]);
				currentSoundCue = ENGINE;
			}
			if (!AudioComponent->IsPlaying())
				AudioComponent->FadeIn(1.0f, 0.6f);
		}
		else {
			AudioComponent->FadeOut(1.5f, 0.0f);
		}
	}
	GetVehicleMovementComponent()->SetThrottleInput(AxisValue * m_fStickyFriction * (int)m_bCanMove);
}

void AToyCar::MoveRight(float AxisValue)
{
	m_fTurnAmount = AxisValue;
}

void AToyCar::PitchCamera(float AxisValue)
{
	if (AxisValue != 0.0f)
		m_fLastUnFocusTime = GetWorld()->GetTimeSeconds();
	CameraInput.Y = AxisValue;
}

void AToyCar::YawCamera(float AxisValue)
{
	if (AxisValue != 0.0f)
		m_fLastUnFocusTime = GetWorld()->GetTimeSeconds();
	CameraInput.X = AxisValue;
}

void AToyCar::OnHandbrakePressed()
{
	if (m_bCanMove)
	{
		if (currentSoundCue != BRAKE)
		{
			AudioComponent->SetSound(AudioCues[BRAKE]);
			currentSoundCue = BRAKE;
		}
		if (!AudioComponent->IsPlaying())
		{
			AudioComponent->Play();
		}
	}

	GetVehicleMovementComponent()->SetHandbrakeInput(true);
	m_bIsBraking = true;
	SetLatStiff(50.0f);
}

void AToyCar::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
	m_bIsBraking = false;
	SetLatStiff(100.0f);
}

void AToyCar::UpdateCamera(float DeltaTime)
{

	FRotator CameraRot = GetActorRotation() + CameraRotation;//SpringArm->GetComponentRotation();
															 //CameraRotation = cameraRot - CameraRotationOffset;

	if (AutoFocus && (GetWorld()->GetTimeSeconds() - m_fLastUnFocusTime >= AutoFocusDelay))
	{
		if (CameraRotationOffset.Yaw <= 180.0f)
			CameraRotationOffset = CameraRotationOffset + (FRotator(0.0f, 0.0f, 0.0f) - CameraRotationOffset) * DeltaTime;
		else if (CameraRotationOffset.Yaw)
			CameraRotationOffset = CameraRotationOffset + (FRotator(0.0f, 360.0f, 0.0f) - CameraRotationOffset) * DeltaTime;
	}

	CameraRotationOffset.Yaw += CameraInput.X;
	CameraRotationOffset.Pitch = FMath::Clamp(CameraRotationOffset.Pitch + CameraInput.Y, -70.0f, 15.0f);

	if (CameraRotationOffset.Yaw >= 360.0f)
		CameraRotationOffset.Yaw -= 360.0f;
	else if (CameraRotationOffset.Yaw < 0.0f)
		CameraRotationOffset.Yaw += 360.0f;

	FRotator finalRotation(CameraRot + CameraRotationOffset);
	finalRotation.Pitch = FMath::Clamp(finalRotation.Pitch, -70.0f, 0.0f);
	finalRotation.Roll = 0.0f;
	SpringArm->SetWorldRotation(finalRotation);
}

bool AToyCar::InAir()
{
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	TArray<UToyCarWheelFront*> frontWheels;
	TArray<UToyCarWheelRear*> rearWheels;

	frontWheels.Add(Cast<UToyCarWheelFront>(Vehicle4W->Wheels[0]));
	frontWheels.Add(Cast<UToyCarWheelFront>(Vehicle4W->Wheels[1]));
	rearWheels.Add(Cast<UToyCarWheelRear>(Vehicle4W->Wheels[2]));
	rearWheels.Add(Cast<UToyCarWheelRear>(Vehicle4W->Wheels[3]));

	//UVehicleWheel* pWheel = &Vehicle4W->WheelSetups[0].WheelClass;

	if (frontWheels[0] && frontWheels[0]->InAir() &&
		frontWheels[1] && frontWheels[1]->InAir() &&
		rearWheels[0] && rearWheels[0]->InAir() &&
		rearWheels[1] && rearWheels[1]->InAir())
	{
		return true;
	}
	return false;
}

void AToyCar::SetLatStiff(float fNewLatStiff)
{
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());

	TArray<UToyCarWheelFront*> frontWheels;
	TArray<UToyCarWheelRear*> rearWheels;

	frontWheels.Add(Cast<UToyCarWheelFront>(Vehicle4W->Wheels[0]));
	frontWheels.Add(Cast<UToyCarWheelFront>(Vehicle4W->Wheels[1]));
	rearWheels.Add(Cast<UToyCarWheelRear>(Vehicle4W->Wheels[2]));
	rearWheels.Add(Cast<UToyCarWheelRear>(Vehicle4W->Wheels[3]));

	frontWheels[0]->LatStiffValue = fNewLatStiff;
	frontWheels[1]->LatStiffValue = fNewLatStiff;
	rearWheels[0]->LatStiffValue = fNewLatStiff;
	rearWheels[1]->LatStiffValue = fNewLatStiff;
}

void AToyCar::LimitCarRotation(float DeltaTime)
{
	if (LimitRotation)
	{
		UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
		UPrimitiveComponent* pPrimComponent = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);

		if (InAir())
		{
			pPrimComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.0f));
			FRotator currentRotation(GetActorRotation());
			currentRotation = currentRotation + (FRotator(0.0f, currentRotation.Yaw, 0.0f) - currentRotation) * DeltaTime;
			SetActorRotation(currentRotation, ETeleportType::TeleportPhysics);
			GetVehicleMovementComponent()->SetSteeringInput(m_fTurnAmount*0.14f);
			m_fTimeOnGround = 0.0f;
		}
		else
		{
			m_fTimeOnGround += DeltaTime;
			if (m_fTimeOnGround >= 0.15f)
				GetVehicleMovementComponent()->SetSteeringInput(m_fTurnAmount);

			FRotator currentRotation(GetActorRotation());
			FRotator targetRotation(0.0f, currentRotation.Yaw, 0.0f);

			bool rotated(false);

			// Cap pitch
			if (currentRotation.Pitch > MaxAngle)
			{
				rotated = true;
				//currentRotation.Pitch = rotationCap;
			}
			else if (currentRotation.Pitch < -MaxAngle)
			{
				rotated = true;
				//currentRotation.Pitch = -rotationCap;
			}

			// Cap roll
			if (currentRotation.Roll > MaxAngle)
			{
				rotated = true;
				//currentRotation.Roll = rotationCap;
			}
			else if (currentRotation.Roll < -MaxAngle)
			{
				rotated = true;
				//currentRotation.Roll = -rotationCap;
			}

			if (rotated)
			{
				FRotator rotateAmount = (targetRotation - currentRotation).GetNormalized();
				pPrimComponent->SetPhysicsAngularVelocityInDegrees(FVector(rotateAmount.Roll, rotateAmount.Pitch, rotateAmount.Yaw)*RotateSpeed);
			}
		}
	}
}

void AToyCar::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (m_bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			m_bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			m_bIsLowFriction = true;
		}
	}
}

void AToyCar::ChangePossesion()
{
	if (PossessionChangerManager)
		PossessionChangerManager->CheckPossessionPads();
}

void AToyCar::ChangeHUD()
{
	//AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	//GameMode->DisplayPlanePartsCollected(true);
}

void AToyCar::ResetPositionAndRotation()
{
	if (GetWorld()->GetTimeSeconds() - m_fLastRespawn >= RespawnDelay)
	{
		m_fLastRespawn = GetWorld()->GetTimeSeconds();

		FVector currentLocation = this->GetActorLocation();
		FRotator currentRotation = this->GetActorRotation();

		this->SetActorRelativeLocation(FVector(currentLocation.X, currentLocation.Y, currentLocation.Z + 100.0f), false, NULL, ETeleportType::TeleportPhysics);
		this->SetActorRotation(FRotator(0.0f, currentRotation.Yaw, 0.0f), ETeleportType::TeleportPhysics);

		ResetVelocity();
	}
}

void AToyCar::Respawn()
{
	//if (GetWorld()->GetTimeSeconds() - fLastRespawn >= RespawnDelay)
	//{
	m_fLastRespawn = GetWorld()->GetTimeSeconds();

	for (TActorIterator<ARespawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetIsCurrentSpawnPoint())
		{
			FRotator currentRotation = this->GetActorRotation();

			FVector spawnLocation = ActorItr->GetActorLocation();
			spawnLocation.Z += 100;
			this->SetActorLocationAndRotation(spawnLocation, FRotator(0.0f, currentRotation.Yaw, 0.0f), false, NULL, ETeleportType::TeleportPhysics);
			GetWorld()->GetTimeSeconds();
			ResetVelocity();
		}
	}
	//}
}

void AToyCar::ResetVelocity()
{
	UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
	UPrimitiveComponent* pPrimComponent = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
	pPrimComponent->SetPhysicsLinearVelocity(FVector(0.0f, 0.0f, 0.0f));
	pPrimComponent->SetPhysicsAngularVelocityInDegrees(FVector(0.0f, 0.0f, 0.0f));
}

void AToyCar::OnSticky()
{
	m_fStickyFriction = 0.1f;
}

void AToyCar::OffSticky()
{
	m_fStickyFriction = 1.0f;
}

void AToyCar::LookTo(float DeltaTime, USceneComponent* targetActor)
{
	if (targetActor != nullptr)
	{
		FVector from(RootComponent->GetComponentLocation());
		FVector to(targetActor->GetComponentLocation());
		
		FVector targetPosition(to - from);
		FRotator targetRotation((to - (from + FVector(0.0f, 0.0f, 150.0f))).Rotation());

		targetRotation = UKismetMathLibrary::ComposeRotators(targetRotation, RootComponent->GetComponentRotation().GetInverse());

		FVector previousPosition(Camera->RelativeLocation);
		FRotator previousRotation(Camera->RelativeRotation);

		targetPosition = previousPosition + (targetPosition - previousPosition) * DeltaTime;
		targetRotation = previousRotation + (targetRotation - previousRotation) * DeltaTime;

		Camera->SetRelativeRotation(targetRotation);
		Camera->SetRelativeLocation(targetPosition);

		CameraRotation.Pitch = 0.0f;
		CameraInput = FVector2D(0.0f, 0.0f);
		CameraRotationOffset = FRotator(0.0f);
		m_fLastUnFocusTime = 0.0f;
	}
	else
	{
		CameraRotation.Pitch = -20.0f;
		FVector targetPosition(FVector(0.0f, 0.0f, 0.0f));
		FRotator targetRotation(FRotator(10.0f, 0.0f, 0.0f));

		FVector previousPosition(Camera->RelativeLocation);
		FRotator previousRotation(Camera->RelativeRotation);

		targetPosition = previousPosition + (targetPosition - previousPosition) * DeltaTime*2.0f;
		targetRotation = previousRotation + (targetRotation - previousRotation) * DeltaTime*2.0f;

		Camera->SetRelativeRotation(targetRotation);
		Camera->SetRelativeLocation(targetPosition);
	}
}

void AToyCar::LookAtComponent(float DeltaTime, USceneComponent* targetActor)
{
	if (targetActor != nullptr)
	{
		FVector targetPosition(FVector(0.0f, 0.0f, 50.0f));
		FRotator targetRotation((targetActor->GetComponentLocation() - UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)->GetCameraLocation()).Rotation());

		targetRotation = UKismetMathLibrary::ComposeRotators(targetRotation, RootComponent->GetComponentRotation().GetInverse());

		FVector previousPosition(Camera->RelativeLocation);
		FRotator previousRotation(Camera->RelativeRotation);
		
		targetPosition = previousPosition + (targetPosition - previousPosition) * DeltaTime*2.0f;
		targetRotation = previousRotation + (targetRotation - previousRotation) * DeltaTime*2.0f;

		Camera->SetRelativeRotation(targetRotation);
		Camera->SetRelativeLocation(targetPosition);

		CameraRotation.Pitch = 0.0f;
		CameraInput = FVector2D(0.0f, 0.0f);
		CameraRotationOffset = FRotator(0.0f);
		m_fLastUnFocusTime = 0.0f;
	}
	else
	{
		CameraRotation.Pitch = -20.0f;
		FVector targetPosition(FVector(0.0f, 0.0f, 0.0f));
		FRotator targetRotation(FRotator(10.0f, 0.0f, 0.0f));

		FVector previousPosition(Camera->RelativeLocation);
		FRotator previousRotation(Camera->RelativeRotation);

		targetPosition = previousPosition + (targetPosition - previousPosition) * DeltaTime*2.0f;
		targetRotation = previousRotation + (targetRotation - previousRotation) * DeltaTime*2.0f;

		Camera->SetRelativeRotation(targetRotation);
		Camera->SetRelativeLocation(targetPosition);
	}
}

void AToyCar::SetPuzzleCompleted(PuzzleName Name)
{
	switch (Name)
	{

	case TIMER:
		TimerPuzzleCompleted = true;
		break;

	case FRICTION:
		FrictionPuzzleCompleted = true;
		break;

	case TRAIN:
		TrainPuzzleCompleted = true;
		for (TActorIterator<ABackgroundMusicManager> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			ActorItr->SetSound(Sounds::MAIN);
		}
		break;

	}
}

