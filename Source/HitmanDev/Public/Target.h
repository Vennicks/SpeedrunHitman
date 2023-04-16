// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Target.generated.h"

UCLASS()
class HITMANDEV_API ATarget : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ATarget();
	UFUNCTION(BlueprintImplementableEvent)
		void OnDeath();
};
