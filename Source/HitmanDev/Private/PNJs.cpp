// Fill out your copyright notice in the Description page of Project Settings.


#include "HitmanDev/Public/PNJs.h"

#include "Ennemies.h"
#include "Kismet/GameplayStatics.h"

APNJs::APNJs()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	capsuleDetection = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Detector"));
	capsuleDetection->SetupAttachment(RootComponent);
	capsuleDetection->SetGenerateOverlapEvents(true);
}

void APNJs::BeginPlay()
{
	Super::BeginPlay();
	SpawnDefaultController();
	PNJAIController = Cast<APNJAIController>(GetController());
}

void APNJs::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (firstTick)
	{
		TArray<AActor*> Result;
		CurrentState = Patroling;
		capsuleDetection->GetOverlappingActors(Result, APNJs::StaticClass());
		for (int i = 0; i != Result.Num(); i++)
		{
			if (Result[i] != this)
				CurrentState = Crowded;
		}
		if (CurrentState == Patroling)
			StartPatrol();
		firstTick = false;
	}
}


/****************************** Gestion de l'état *********************************************/


//Permet de rendre l'IA "Occupé" = l'arrete pendant un certain temps
void APNJs::SetBusy()
{
	if (!canBeBusy)
		return;

	canBeBusy = false;
	PNJAIController->Stop();
	SavePreviousState = CurrentState;
	CurrentState = Crowded;
	GetWorld()->GetTimerManager().SetTimer(ActionsTimerHandle, this, &APNJs::Unbusy, TimeBusy, false);
}

//Permet de rendre l'IA "Occupé" = l'arrete pendant un certain temps
void APNJs::EnableBusy()
{
	canBeBusy = true;
}

//Permet de reprendre la marche aléatoire après le temps "occupé"
void APNJs::Unbusy()
{
	CurrentState = CurrentState;
	StartPatrol();
	GetWorld()->GetTimerManager().SetTimer(ActionsTimerHandle, this, &APNJs::EnableBusy, TimeBetweenActions, false);
}

//Permet de reprendre la marche aléatoire après le temps "occupé"e
PNJState APNJs::GetState()
{
	return CurrentState;
}

//Permet de reprendre la marche aléatoire après le temps "occupé"
void APNJs::StartPatrol()
{
	CurrentState = Patroling;
	PNJAIController->RandomPatrol();
}

//Passe l'ia en mort
void APNJs::OnDeath()
{
	FearCrowd();
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll");
	GetMesh()->SetSimulatePhysics(true);
	GetCharacterMovement()->DisableMovement();
	PNJAIController->Stop();
	CurrentState = Dead;
}


/******************************  *********************************************/


//Permet d'appeurer l'ia -> mouvement speed doublé + état appeuré
void APNJs::Fear()
{
	if (CurrentState == Feared)
		return;

	if (CurrentState == Crowded)
		PNJAIController->RandomPatrol();

	CurrentState = Feared;
	GetCharacterMovement()->MaxWalkSpeed *= 2;
}

//Permet d'appeurer la foule dans laquelle l'ia est
void APNJs::FearCrowd()
{
	Fear();
	TArray<AActor*> Result;
	CurrentState = Patroling;
	capsuleDetection->GetOverlappingActors(Result, StaticClass());
	for (int i = 0; i != Result.Num(); i++)
	{
		APNJs* pnj = Cast<APNJs>(Result[i]);
		if (pnj->GetState() == Crowded)
			pnj->FearCrowd();
	}
	WarnCloseEnnemies();
}

//Permet de gérer les ennemis de la foule dans laquelle l'ia est
void APNJs::WarnCloseEnnemies()
{
	TArray<AActor*> Result;
	CurrentState = Patroling;
	capsuleDetection->GetOverlappingActors(Result, AEnnemies::StaticClass());
	for (int i = 0; i != Result.Num(); i++)
	{
		AEnnemies* ennemy = Cast<AEnnemies>(Result[i]);
		if (ennemy->GetState() != EnnemiesState::Chasing)
			ennemy->ChasePlayer();
			
	}
}

