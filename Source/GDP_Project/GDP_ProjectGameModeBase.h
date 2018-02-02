// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HUDWidget.h"
#include "GDP_ProjectGameModeBase.generated.h"

enum PlaneParts
{
	PlanePart1,
	PlanePart2,
	PlanePart3,
	PlanePart4,
	PlanePart5,
	PlaneParts_MAX // Used to calculate the number of plane parts in the game
};

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API AGDP_ProjectGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	virtual void BeginPlay() override;
	
public:

	/* Updates the current HUD Widget
	* @param Name the name of the current pawn.
	*/
	void ChangeHUD(const FString& name);

	void AddHUD();
	void RemoveHUD();

	void SetPlanePartCollected(PlaneParts PartCollected);
	bool GetPlanePartCollected(PlaneParts PartCollected);

	void DisplayPlanePartsCollected(bool bToggle);

protected:

	/** Remove the current menu widget and create a new one from the specified class, if provided. */
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> PlaneHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> TrainHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> CarHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> ChangeVehicleHUDClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "HUD", Meta = (BlueprintProtected = true))
	TSubclassOf<class UUserWidget> PlanePartsHUDClass; // The main HUD. Used to display plane parts collected.

	UPROPERTY()
	UUserWidget* CurrentWidget;

	UPROPERTY()
	UUserWidget* VehicleWidget;

	UPROPERTY()
	UUserWidget* PlanePartsWidget;

	bool CollectedPlaneParts[PlaneParts_MAX];

	void DisplayPlanePartsWidget();
};
