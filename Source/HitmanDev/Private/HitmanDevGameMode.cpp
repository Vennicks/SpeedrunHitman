// Copyright Epic Games, Inc. All Rights Reserved.

#include "HitmanDevGameMode.h"
#include "HitmanDevCharacter.h"
#include "UObject/ConstructorHelpers.h"

AHitmanDevGameMode::AHitmanDevGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/BluePrints/BP_Hitman"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
