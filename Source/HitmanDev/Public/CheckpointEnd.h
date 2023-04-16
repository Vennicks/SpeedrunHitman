// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"
#include "CheckpointEnd.generated.h"

UCLASS()
class HITMANDEV_API ACheckpointEnd : public APawn
{
	GENERATED_BODY()


		/******************* Membres **********************/

public:
	// Sets default values for this pawn's properties
	ACheckpointEnd();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Zone")
		UStaticMeshComponent* TriggerZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Zone")
		UMaterialInterface* ZoneMaterial;
private:
	bool isEnable = false;


		/******************* Méthodes **********************/

protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable)
	void Enable();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
};
