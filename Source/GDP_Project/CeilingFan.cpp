// Fill out your copyright notice in the Description page of Project Settings.

#include "CeilingFan.h"
#include "UObject/ConstructorHelpers.h"

// Sets default values
ACeilingFan::ACeilingFan()
	:RotationsPerSecond(720.0f), ToyPlane(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Mesh
	CeilingCylinderMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CeilingFanCylinderMeshComponent"));
	CeilingFanMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CeilingFanMeshComponent"));
	FanAirFlow = CreateDefaultSubobject<UBoxComponent>(TEXT("CeilingFanAirFlow"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> FanCylinder(TEXT("StaticMesh'/Game/Assets/Fan/Fan_Cylinder.Fan_Cylinder'"));
	if (FanCylinder.Object)
		CeilingCylinderMeshComponent->SetStaticMesh(FanCylinder.Object);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Fan(TEXT("StaticMesh'/Game/Assets/Fan/Fan.Fan'"));
	if (Fan.Object)
		CeilingFanMeshComponent->SetStaticMesh(Fan.Object);

	RootComponent = CeilingCylinderMeshComponent;
	CeilingFanMeshComponent->AttachToComponent(CeilingCylinderMeshComponent, FAttachmentTransformRules::KeepWorldTransform);
	FanAirFlow->AttachToComponent(CeilingCylinderMeshComponent, FAttachmentTransformRules::KeepWorldTransform);

	CeilingFanMeshComponent->SetRelativeLocation(FVector(0.0f, 0.0f, 0.0f));

	FanAirFlow->SetBoxExtent(FVector(500.0f, 500.0f, 500.0f));
	FanAirFlow->SetWorldScale3D(FVector(1.0f, 1.0f, 1.0f));
	FanAirFlow->SetRelativeLocation(FVector(0.0f, 0.0f, -220.0f));
	//FanAirFlow->OnComponentBeginOverlap.AddDynamic(this, &ACameraPuzzle::OnBeginOverlap);
	
}

// Called when the game starts or when spawned
void ACeilingFan::BeginPlay()
{
	Super::BeginPlay();
	if (ToyPlane == nullptr)
		UE_LOG(LogTemp, Error, TEXT("No ToyPlane found in Ceiling Fan for: %s , Unique ID: %d"), *GetName(), GetUniqueID());

}

// Called every frame
void ACeilingFan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CeilingFanMeshComponent->SetRelativeRotation(FRotator(0.0f, CeilingFanMeshComponent->GetComponentRotation().Yaw + (DeltaTime * RotationsPerSecond), 0.0f));

	PushPlane(DeltaTime);
}

void ACeilingFan::PushPlane(float DeltaTime)
{
	if (FanAirFlow->IsOverlappingActor(ToyPlane))
	{
		ToyPlane->MoveUp(-1.0f);
		ToyPlane->SetActorLocation(ToyPlane->GetActorLocation() - FVector(0.0f, 0.0f, 100.0f * DeltaTime));
	}
}
