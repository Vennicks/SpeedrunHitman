// Fill out your copyright notice in the Description page of Project Settings.


#include "HitmanDev/Public/PNJAIController.h"

#include "HitmanDev/Public/PNJs.h"
#include "Kismet/KismetMathLibrary.h"


void APNJAIController::BeginPlay()
{
    Super::BeginPlay();

    NavArea = FNavigationSystem::GetCurrent<UNavigationSystemV1>(this);
}

//débute la marche aléatoire
void APNJAIController::RandomPatrol()
{
    if (NavArea)
    {
        NavArea->K2_GetRandomReachablePointInRadius(GetWorld(), GetPawn()->GetActorLocation(),
            RandomLocation, 2500.0);

        FVector TargetToRotate = FVector(RandomLocation.X, RandomLocation.Y, GetPawn()->GetActorLocation().Z);
        FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), TargetToRotate);

        GetPawn()->SetActorRotation(TargetRot);

        MoveToLocation(RandomLocation);

        GetWorld()->GetTimerManager().SetTimer(TimerToChangeTargetDirection, this, &APNJAIController::RandomPatrol, FMath::RandRange(1.0, 4.0));
    }
}

//Stop la marche alétoire
void APNJAIController::Stop()
{
    if (NavArea)
    {
        MoveToLocation(GetPawn()->GetActorLocation());
    	GetWorldTimerManager().ClearTimer(TimerToChangeTargetDirection);
    }
}