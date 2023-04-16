// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Ennemies.generated.h"

UENUM(BlueprintType)
enum class EnnemiesState : uint8
{
	Idle		UMETA(DisplayName = "Idle"),
	Patroling	UMETA(DisplayName = "Patroling"),
	Chasing		UMETA(DisplayName = "Chasing"),
	Dead		UMETA(DisplayName = "Dead")
};


UCLASS()
class AEnnemies : public ACharacter
{

	GENERATED_BODY()


		/******************* Membres **********************/

public:

	UPROPERTY(EditAnywhere, Category = "Roaming")
		TArray<FVector> LocationsToPatrol;

	UPROPERTY(EditAnywhere, Category = "Roaming")
		float speed;

	AEnnemies();

private:
	int IndexCurrentLocation = 0;
	EnnemiesState CurrentState = EnnemiesState::Idle;
	FVector TargetDirection = FVector(0, 0, 0);


	/******************* Méthodes **********************/

public:	
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Action")
		void OnSeePlayer(bool state);

	UFUNCTION(BlueprintCallable, Category = "AIState")
		EnnemiesState GetState();

	UFUNCTION(BlueprintCallable, Category = "AIState")
		void ChasePlayer();

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION(BlueprintImplementableEvent)
		void OnDeath();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
		void TargetPlayer(APawn *OtherActor);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	bool IsNearLocations();

	UFUNCTION(BlueprintCallable)
		void SetState(EnnemiesState state);

private :
	void GoToLocation();

	UPROPERTY()
	AAIController* AIController;
};
