// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HitmanDev/Public/PNJAIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "AITypes.h"
#include "PNJs.generated.h"
enum PNJState
{
	Idle,
	Crowded,
	Patroling,
	Feared,
	Dead
};

UCLASS()
class APNJs : public ACharacter
{
	GENERATED_BODY()

		/******************* Membres **********************/

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnvDetection")
	UCapsuleComponent* capsuleDetection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ValuesCPP")
	float TimeBusy = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ValuesCPP")
	float TimeBetweenActions = 1;

	UPROPERTY()
	class APNJAIController* PNJAIController;

private:
	PNJState CurrentState = Idle;
	PNJState SavePreviousState = Idle;
	float timeSinceLastDirection = 0;
	FVector Direction = FVector(0, 0, 0);
	bool canBeBusy = true;
	FTimerHandle ActionsTimerHandle;


	/******************* Méthodes **********************/

public:	
	virtual void Tick(float DeltaTime) override;
	PNJState GetState();

	UFUNCTION(BlueprintCallable, Category = "Output")
	void SetBusy();

	UFUNCTION(BlueprintCallable, Category = "Output")
	void OnDeath();

	UFUNCTION()
	void StartPatrol();

	UFUNCTION(BlueprintCallable, Category = "Output")
	void WarnCloseEnnemies();

	UFUNCTION(BlueprintCallable, Category = "Output")
	void FearCrowd();

	UFUNCTION(BlueprintCallable, Category = "Output")
	void Fear();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	APNJs();
	void Unbusy();
	void EnableBusy();
	bool firstTick = true;
	float currentSpeed;
	FDelegateHandle RetriggerPatrol;

};
