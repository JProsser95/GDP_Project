// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class GDP_PROJECT_API UHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	 /** FText value displayed by this widget */
     UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Text")
     FText DisplayText;
 
     /** Retrieves FText value currently held in DisplayText */
     UFUNCTION(BlueprintPure, Category = "Widgets|Text")
     FText GetDisplayText();
 
     /** Assigns passed FText to DisplayText */
     UFUNCTION(BlueprintCallable, Category = "Widgets|Text")
     void SetDisplayText(const FText& NewDisplayText);
	
};
