// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "PNJAIController.generated.h"

/**
 * 
 */
UCLASS()
class HITMANDEV_API APNJAIController : public AAIController
{
	GENERATED_BODY()

private:
    class UNavigationSystemV1* NavArea;
    FTimerHandle TimerToChangeTargetDirection;
    FVector RandomLocation;

protected:
    void BeginPlay() override;

public:

    UFUNCTION()
        void RandomPatrol();
    UFUNCTION()
        void Stop();
};
