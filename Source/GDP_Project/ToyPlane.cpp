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
	:MinSpeed(400.0f), MaxSpeed(600.0f), CamShakeSpeed(500.0f), SpeedIncrement(100.0f), BoostSpeedIncrement(200.0f), RotateSpeed(1.5f), TurnSpeed(1.0f), PropRotateSpeed(3.0f),
	MaximumBoost(100.0f), CurrentBoost(0.0f),
	ControlTypeRealistic(false)
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//	PlaneBodyMeshComponent->OnComponentBeginOverlap.AddDynamic(this, &AToyPlane::OnToyPlaneOverlap);
	//	PlaneBodyMeshComponent->BodyInstance.SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics, true);

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
	OurCameraSpringArm->CameraLagSpeed = 4.0f;

	OurCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("GameCamera"));
	OurCamera->SetupAttachment(OurCameraSpringArm, USpringArmComponent::SocketName);

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
}

void AToyPlane::Restart()
{
	Super::Restart();

	AGDP_ProjectGameModeBase* GameMode = (AGDP_ProjectGameModeBase*)GetWorld()->GetAuthGameMode();
	GameMode->ChangeHUD("ToyPlane");
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
	else if (fSpeed > MinSpeed)
	{
		fSpeed -= DeltaTime * SpeedIncrement;
		//UpdateCurrentBoost(DeltaTime * 0.2f * MaximumBoost);
	}
	else
		fSpeed += DeltaTime * SpeedIncrement;

	if (fSpeed >= CamShakeSpeed) 
	{
		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		PC->ClientPlayCameraShake(CameraShake, 1);
	}

	fSpeed = FMath::Clamp(fSpeed, 0.0f, MaxSpeed);

	//Scale our movement input axis values by 100 units per second
	MovementInput = MovementInput.GetSafeNormal() * 100.0f;
	FRotator NewRotation = GetActorRotation();

	float fRotateMod(fSpeed / MaxSpeed);
	if (fRotateMod > 1.0f)
		fRotateMod = 1.0f;

	if (MovementInput.X == 0 && MovementInput.Y == 0)
	{
		NewRotation = FMath::Lerp(NewRotation, FRotator(NewRotation.Pitch, NewRotation.Yaw, 0), DeltaTime * 2);

		//if (NewRotation.Pitch > 0)
		//	NewRotation.Pitch -= DeltaTime*10;
		//else
		//	NewRotation.Pitch += DeltaTime*10;
	}
	else if (MovementInput.X == 0)
	{
		//if (NewRotation.Pitch > 0)
		//	NewRotation.Pitch -= DeltaTime*10;
		//else
		//	NewRotation.Pitch += DeltaTime*10;
	}
	else if (MovementInput.Y == 0)
		NewRotation = FMath::Lerp(NewRotation, FRotator(NewRotation.Pitch, NewRotation.Yaw, 0), DeltaTime * 2);

	FRotator PitchRoll(0.0f, 0.0f, 0.0f);
	if (fSpeed > MinSpeed/2)
	{
		if (MovementInput.Y != 0) {
			//A or D pressed
			//FRotator Roll(0.0f, 0.0f, MovementInput.Y * DeltaTime);

			//NewRotation = UKismetMathLibrary::ComposeRotators(NewRotation, Roll);

			PitchRoll.Roll = MovementInput.Y * DeltaTime * RotateSpeed;


			//NewRotation.Roll += MovementInput.Y * DeltaTime * RotateSpeed;
			//NewRotation.Roll = FMath::Clamp(NewRotation.Roll, -90.0f * fRotateMod, 90.0f * fRotateMod);
		}
	}

	if (MovementInput.X != 0) {
		//W or S press
		//FRotator Pitch(FRotator(MovementInput.X * DeltaTime, 0.0f, 0.0f));
		PitchRoll.Pitch = MovementInput.X * DeltaTime * 0.5f;
		//FMath::Clamp(Pitch.Pitch, -60.0f * fRotateMod, 60.0f * fRotateMod);
		//NewRotation = UKismetMathLibrary::ComposeRotators(Pitch, NewRotation);
		////NewRotation.Pitch += MovementInput.X * DeltaTime;
		//NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -60.0f * fRotateMod, 60.0f * fRotateMod);
	}
	NewRotation = UKismetMathLibrary::ComposeRotators(PitchRoll, NewRotation);

	NewRotation.Pitch = FMath::Clamp(NewRotation.Pitch, -60.0f * fRotateMod, 60.0f * fRotateMod);
	NewRotation.Roll = FMath::Clamp(NewRotation.Roll, -90.0f * fRotateMod, 90.0f * fRotateMod);

	if (ControlTypeRealistic)
		NewRotation.Yaw += GetActorRotation().Roll * DeltaTime * fRotateMod * TurnSpeed;
	else
		NewRotation.Yaw += PitchRoll.Roll * 100.0f * DeltaTime * fRotateMod * TurnSpeed;

	SetActorRotation(NewRotation);
	//SetActorLocation(NewLocation);

	CustomMovementComponent->AddInputVector(GetActorForwardVector() * fSpeed);

	fPropRotation += DeltaTime * fSpeed * PropRotateSpeed;
	PlanePropMeshComponent->SetRelativeRotation(FRotator(0.0f, 0.0f, fPropRotation));
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
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 50.0f), FRotator(-30.0f, 0.0f, 0.0f));
		OurCameraSpringArm->TargetArmLength = 700.f;
		OurCameraSpringArm->bEnableCameraLag = true;
	}
	else {
		eCameraType = FIRST_PERSON;
		OurCameraSpringArm->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, 0.0f), FRotator(0.0f, 0.0f, 0.0f));
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
	
