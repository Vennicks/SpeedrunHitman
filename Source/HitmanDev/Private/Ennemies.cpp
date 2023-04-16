// Fill out your copyright notice in the Description page of Project Settings.

#include "HitmanDev/Public/Ennemies.h"

//////////////////////////////////////////////////////////////////////////
// AEnnemies

AEnnemies::AEnnemies()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnnemies::BeginPlay()
{
	Super::BeginPlay();
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);
	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AEnnemies::OnHit);
	
	LocationsToPatrol.Add(GetActorLocation());
	if (LocationsToPatrol.Num() > 1)
		SetState(EnnemiesState::Patroling);
	else
		SetState(EnnemiesState::Idle);
}


void AEnnemies::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetState() == EnnemiesState::Patroling)
		GoToLocation();
}


/****************************** Gestion de la patrouille *********************************************/


//Permet de vérifier si l'ennemie est a la position a atteindre pour la patrouille
bool AEnnemies::IsNearLocations()
{
	if (GetActorLocation().X >= LocationsToPatrol[IndexCurrentLocation].X - 5 &&
		GetActorLocation().X <= LocationsToPatrol[IndexCurrentLocation].X + 5 &&
		GetActorLocation().Y >= LocationsToPatrol[IndexCurrentLocation].Y - 5 &&
		GetActorLocation().Y <= LocationsToPatrol[IndexCurrentLocation].Y + 5)
		return true;

	return false;
}

//Permet de définir la position a atteindre/l'étape de la patrouille
void AEnnemies::GoToLocation()
{
	if (TargetDirection == FVector(0, 0, 0) || IsNearLocations())
	{
		if (IsNearLocations())
			IndexCurrentLocation++;

		if (IndexCurrentLocation >= LocationsToPatrol.Num())
			IndexCurrentLocation = 0;
	}

	TargetDirection = LocationsToPatrol[IndexCurrentLocation] - GetActorLocation();
	TargetDirection.Normalize();

	FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), FVector(LocationsToPatrol[IndexCurrentLocation].X, LocationsToPatrol[IndexCurrentLocation].Y, GetActorLocation().Z));
	SetActorRotation(TargetRot);

	AddMovementInput(TargetDirection, speed);
}


/****************************** Gest interraction avec le juoueur *********************************************/


//Perrmet de vérifier la collision avec le joueur -> recommence le niveau = mort
void AEnnemies::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag("PLAYER"))
	{
		if (GetState() == EnnemiesState::Dead)
			return;

		UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}

//Permet de changer l'état de l'ennemie en fonction de la vue du joueur
void AEnnemies::OnSeePlayer(bool state)
{
	SetState(state == true ? EnnemiesState::Chasing : EnnemiesState::Patroling);
}


/****************************** gestion de l'état de l'IA *********************************************/


//retourne l'état
EnnemiesState AEnnemies::GetState()
{
	return CurrentState;
}

//Défini l'état
void AEnnemies::SetState(EnnemiesState state)
{
	CurrentState = state;
}

//Permet de changer l'état en chasing = chasse le joueur
void AEnnemies::ChasePlayer()
{
	SetState(EnnemiesState::Chasing);
	TargetPlayer(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}
