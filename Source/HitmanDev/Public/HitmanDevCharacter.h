// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HitmanDevCharacter.generated.h"

UENUM(BlueprintType)
enum class HiddenState : uint8
{
	NONE		UMETA(DisplayName = "None"),
	InCrowd		UMETA(DisplayName = "InCrowd"),
	Costumed	UMETA(DisplayName = "Costumed"),
};

UCLASS(config=Game)
class AHitmanDevCharacter : public ACharacter
{
	GENERATED_BODY()


		/******************* Membres **********************/

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
		UBoxComponent* StabDetection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CrowdDetection")
		UCapsuleComponent* CrowdDetection;

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* FollowCamera;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
		float TurnRateGamepad;
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		float AimingZoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
		float ShootCooldown = 0;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

private:
	HiddenState Hidden = HiddenState::NONE;
	bool Aiming = false;
	bool TargetEliminated = false;
	int crowdCount = 0;
	bool canShoot = true;
	FTimerHandle PlayerTimerHandler;


	/******************* Méthodes **********************/

public:
	AHitmanDevCharacter();
	UFUNCTION(BlueprintCallable)
		HiddenState GetHiddenState();
	UFUNCTION(BlueprintCallable)
		bool isTargetEliminated();
	UFUNCTION(BlueprintImplementableEvent)
		void SetEnnemieMaterial();

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);
	void StartAiming();
	void StopAiming();
	void StartAttack();
	void FearAll();
	void FearPNJs(TArray<AActor*> PNJsToFear);
	void HandleShoot();
	void HandleStab();
	void EnableShoot();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	UFUNCTION()
	void OnDetectionBegin(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnDetectionEnd(UPrimitiveComponent* OverlappedComponent, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};

