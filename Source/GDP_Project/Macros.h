// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#define OUTPUT_STRING(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT(x));}
#define OUTPUT_INT(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::FromInt(x));}
#define OUTPUT_FLOAT(x) if(GEngine){GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::SanitizeFloat(x));}



class GDP_PROJECT_API Macros
{
};
