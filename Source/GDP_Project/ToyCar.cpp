// Fill out your copyright notice in the Description page of Project Settings.

#include "ToyCar.h"
#include "ToyPlane.h"
#include "ToyCarWheelFront.h"
#include "ToyCarWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "WheeledVehicleMovementComponent4W.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/Engine.h"
#include "GameFramework/Controller.h"
#include "Components/SphereComponent.h"
#include "GDP_ProjectGameModeBase.h"
#include "UObject/ConstructorHelpers.h"
#include "PossessableActorComponent.h"
#include "Components/WidgetComponent.h"
#include "PossessableActorComponent.h"
#include "EngineUtils.h"
#include "RespawnPoint.h"
#include "ToyTrain.h"
#include "Macros.h"
#include "Sound/SoundCue.h"
#include "Runtime/Core/Public/Math/UnrealMathUtility.h"
#include "Components/AudioComponent.h"

const FName AToyCar::LookUpBinding("LookUp");
const FName AToyCar::LookRightBinding("LookRight");

AToyCar::AToyCar()
	:RespawnDelay(1.5f), MaxAngle(90.0f), RotateSpeed(2.0f), LimitRotation(true)
{
	// Car mesh
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> CarMesh(TEXT("SkeletalMesh'/Game/Car/TOYCAR.TOYCAR'"));
	GetMesh()->SetSkeletalMesh(CarMesh.Object);

	static ConstructorHelpers::FClassFinder<UObject> AnimBPClass(TEXT("/Game/Car/TOYCAR_Skeleton_AnimBlueprint"));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetAnimInstanceClass(AnimBPClass.Class);

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));
	SphereCollider->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepWorldTransform);
	SphereCollider->SetRelativeScale3D(FVector(7.0f, 7.0f, 7.0f));
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AToyCar::OnBeginOverlap);
	SphereCollider->OnComponentEndOverlap.AddDynamic(this, &AToyCar::OnEndOverlap);

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
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(0.0f, 1.0f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(43.0f, 0.7f);
	Vehicle4W->SteeringCurve.GetRichCurve()->AddKey(120.0f, 0.6f);

	// Transmission	
	// We want 4wd
	Vehicle4W->DifferentialSetup.DifferentialType = EVehicleDifferential4W::LimitedSlip_4W;

	// Drive the front wheels a little more than the rear
	Vehicle4W->DifferentialSetup.FrontRearSplit = 0.65;

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

	CameraParent = CreateDefaultSubobject<USphereComponent>(TEXT("CameraParent"));
	CameraParent->SetupAttachment(RootComponent);

	// Create a spring arm component for our chase camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetRelativeLocation(FVector(0.0f, 0.0f, 34.0f));
	SpringArm->SetWorldRotation(FRotator(-20.0f, 0.0f, 0.0f));
	SpringArm->SetupAttachment(CameraParent);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bEnableCameraLag = false;
	
	// Create the chase camera component 
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("ChaseCamera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->SetRelativeRotation(FRotator(10.0f, 0.0f, 0.0f));
	Camera->bUsePawnControlRotation = false;
	Camera->FieldOfView = 90.f;

	//ChangeVehicleWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	//ChangeVehicleWidget->SetupAttachment(CameraParent);
	//static ConstructorHelpers::FClassFinder<UUserWidget> Widget(TEXT("/Game/HUD/VehicleWidget"));
	//ChangeVehicleWidget->SetWidgetClass(Widget.Class);
	//ChangeVehicleWidget->SetRelativeLocation(FVector(0, 0, 150.0f));

	PossessableComponent = CreateDefaultSubobject<UPossessableActorComponent>(TEXT("PossessableComponent"));

	bIsLowFriction = false;
	bInReverseGear = false;

	bCanPosses = false;
	isActive = true;
	isBreaking = false;
	bCanMove = true;
	fSitckyFriction = 1.0f;

	// Load our Sound Cue for the propeller sound we created in the editor... 
	static ConstructorHelpers::FObjectFinder<USoundCue> engineSound(TEXT("/Game/Sounds/Car_Engine_Cue"));
	static ConstructorHelpers::FObjectFinder<USoundCue> breakSound(TEXT("/Game/Sounds/Car_Break_Cue"));

	AudioCues.Add(engineSound.Object);
	AudioCues.Add(breakSound.Object);

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
}

// Called every frame
void AToyCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//FVector PlayerLoc = Camera->GetComponentLocation();
	//FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(this->GetActorLocation(), PlayerLoc);
	//ChangeVehicleWidget->SetRelativeRotation(PlayerRot);

	// Setup the flag to say we are in reverse gear
	bInReverseGear = GetVehicleMovement()->GetCurrentGear() < 0;

	FRotator cameraRot = SpringArm->GetComponentRotation();
	cameraRot.Yaw += CameraInput.X;
	cameraRot.Pitch = FMath::Clamp(cameraRot.Pitch + CameraInput.Y, -80.0f, -15.0f);
	cameraRot.Roll = 0;
	SpringArm->SetWorldRotation(cameraRot);

	FRotator cam = Camera->GetComponentRotation();
	cam.Roll = 0;
	Camera->SetWorldRotation(cam);



	// Update physics material
	//UpdatePhysicsMaterial();



	if (LimitRotation)
	{
		UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
		UPrimitiveComponent* pPrimComponent = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);

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
			//FRotator rotateAmount(currentRotation);
			FRotator rotateAmount = (targetRotation - currentRotation).GetNormalized();
			//FRotator rotateAmount(FMath::RInterpTo(currentRotation, targetRotation, DeltaTime, 1.0f));
			//OUTPUT_STRING("Over Rotated... Rotating Back!");
			//SetActorRotation(currentRotation, ETeleportType::TeleportPhysics);
			pPrimComponent->SetPhysicsAngularVelocityInDegrees(FVector(rotateAmount.Roll, rotateAmount.Pitch, rotateAmount.Yaw)*RotateSpeed);
			//pPrimComponent->SetPhysicsAngularVelocityInRadians(FVector(-currentRotation.Roll, -currentRotation.Pitch, 0.0f)*DeltaTime);
		}
	}
}

void AToyCar::Restart()
{
	Super::Restart();

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->ChangeHUD("ToyCar");
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
	PlayerInputComponent->BindAction("DisplayHUD", IE_Released, this, &AToyCar::ChangeHUD);
	PlayerInputComponent->BindAction("ResetCar", IE_Released, this, &AToyCar::ResetPositionAndRotation);
	PlayerInputComponent->BindAction("RespawnCar", IE_Released, this, &AToyCar::Respawn);
	PlayerInputComponent->BindAction("ResetCamera", IE_Released, this, &AToyCar::ResetCamera);

}

void AToyCar::MoveForward(float AxisValue)
{
	if (!isBreaking && bCanMove)
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
			AudioComponent->FadeOut(1.0f, 0.f);
		}
	}

	GetVehicleMovementComponent()->SetThrottleInput(AxisValue * fSitckyFriction * (int)bCanMove);
}

void AToyCar::MoveRight(float AxisValue)
{
	GetVehicleMovementComponent()->SetSteeringInput(AxisValue);
}

void AToyCar::PitchCamera(float AxisValue)
{
	CameraInput.Y = AxisValue;
}

void AToyCar::YawCamera(float AxisValue)
{
	CameraInput.X = AxisValue;
}

void AToyCar::OnHandbrakePressed()
{
	if (currentSoundCue != BREAK) 
	{
		AudioComponent->SetSound(AudioCues[BREAK]);
		currentSoundCue = BREAK;
	}
	if (!AudioComponent->IsPlaying()) 
	{
		AudioComponent->Play();
	}

	GetVehicleMovementComponent()->SetHandbrakeInput(true);
	isBreaking = true;
}

void AToyCar::OnHandbrakeReleased()
{
	GetVehicleMovementComponent()->SetHandbrakeInput(false);
	isBreaking = false;
}

void AToyCar::ResetCamera()
{
	SpringArm->SetWorldRotation(CameraParent->GetComponentRotation());
}

void AToyCar::UpdatePhysicsMaterial()
{
	if (GetActorUpVector().Z < 0)
	{
		if (bIsLowFriction == true)
		{
			GetMesh()->SetPhysMaterialOverride(NonSlipperyMaterial);
			bIsLowFriction = false;
		}
		else
		{
			GetMesh()->SetPhysMaterialOverride(SlipperyMaterial);
			bIsLowFriction = true;
		}
	}
}

void AToyCar::ChangePossesion()
{
	if (possesActor != nullptr)
	{
		GetWorld()->GetFirstPlayerController()->Possess(possesActor);
		AToyPlane* tp = Cast<AToyPlane>(possesActor);
		AToyTrain* tt = Cast<AToyTrain>(possesActor);
		if (tp != nullptr) 
		{
			tp->SetIsActive(true);
		} 
		else if (tt != nullptr)
		{
			tt->SetIsActive(true);
			tt->SetToyCar(this);
		}
	}
}

void AToyCar::ChangeHUD()
{
	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->DisplayPlanePartsCollected(true);
}

void AToyCar::OnBeginOverlap(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (bCanPosses || !isActive || !OtherActor->FindComponentByClass<UPossessableActorComponent>())
		return;

	OUTPUT_STRING("HIT");

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->SetVehicleHUD();

	bCanPosses = true;
	possesActor = Cast<APawn>(OtherActor);
}

void AToyCar::OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!bCanPosses)
		return;

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->RemoveVehicleHUD();

	bCanPosses = false;
	possesActor = nullptr;
}

void AToyCar::ResetPositionAndRotation()
{
	static float lastRespawn(0.0f);

	if (GetWorld()->GetTimeSeconds() - lastRespawn >= RespawnDelay)
	{
		lastRespawn = GetWorld()->GetTimeSeconds();

		FVector currentLoaction = this->GetActorLocation();
		FRotator currentRotation = this->GetActorRotation();

		this->SetActorRelativeLocation(FVector(currentLoaction.X, currentLoaction.Y, currentLoaction.Z + 100), false, NULL, ETeleportType::TeleportPhysics);
		this->SetActorRotation(FRotator(0.0f, currentRotation.Yaw, 0.0f), ETeleportType::TeleportPhysics);

		UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
		UPrimitiveComponent* pPrimComponent = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
		pPrimComponent->SetPhysicsLinearVelocity(FVector(0, 0, 0));
		pPrimComponent->SetPhysicsAngularVelocity(FVector(0, 0, 0));
	}
}

void AToyCar::Respawn()
{
	static float lastRespawn(0.0f);

	if (GetWorld()->GetTimeSeconds() - lastRespawn >= RespawnDelay)
	{
		lastRespawn = GetWorld()->GetTimeSeconds();

		for (TActorIterator<ARespawnPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			if (ActorItr->GetIsCurrentSpawnPoint())
			{
				FRotator currentRotation = this->GetActorRotation();

				FVector spawnLocation = ActorItr->GetActorLocation();
				spawnLocation.Z += 100;
				this->SetActorLocationAndRotation(spawnLocation, FRotator(0.0f, currentRotation.Yaw, 0.0f), false, NULL, ETeleportType::TeleportPhysics);
				GetWorld()->GetTimeSeconds();
				UWheeledVehicleMovementComponent4W* Vehicle4W = CastChecked<UWheeledVehicleMovementComponent4W>(GetVehicleMovement());
				UPrimitiveComponent* pPrimComponent = Cast<UPrimitiveComponent>(Vehicle4W->UpdatedComponent);
				pPrimComponent->SetPhysicsLinearVelocity(FVector(0, 0, 0));
				pPrimComponent->SetPhysicsAngularVelocity(FVector(0, 0, 0));
			}
		}
	}
}

void AToyCar::SetIsActive(bool Value)
{
	isActive = Value;
}

void AToyCar::OnSticky()
{
	fSitckyFriction = 0.3f;
}

void AToyCar::OffSticky()
{
	fSitckyFriction = 1.0f;
}