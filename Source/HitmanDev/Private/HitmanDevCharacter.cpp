// Copyright Epic Games, Inc. All Rights Reserved.


#include "HitmanDevCharacter.h"

#include "Ennemies.h"
#include "PNJs.h"
#include "Target.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AHitmanDevCharacter

AHitmanDevCharacter::AHitmanDevCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	StabDetection = CreateDefaultSubobject<UBoxComponent>(TEXT("StabDetector"));
	StabDetection->SetupAttachment(RootComponent);
	StabDetection->SetGenerateOverlapEvents(true);

	CrowdDetection = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CrowdDetector"));
	CrowdDetection->SetupAttachment(RootComponent);
	CrowdDetection->SetGenerateOverlapEvents(true);
	CrowdDetection->OnComponentBeginOverlap.AddDynamic(this, &AHitmanDevCharacter::OnDetectionBegin);
	CrowdDetection->OnComponentEndOverlap.AddDynamic(this, &AHitmanDevCharacter::OnDetectionEnd);
}


/****************************** Input Part *********************************************/

//Mapping des intputs
void AHitmanDevCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Aim", IE_Pressed, this, &AHitmanDevCharacter::StartAiming);
	PlayerInputComponent->BindAction("Aim", IE_Released, this, &AHitmanDevCharacter::StopAiming);
	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AHitmanDevCharacter::StartAttack);

	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AHitmanDevCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &AHitmanDevCharacter::MoveRight);
	
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AHitmanDevCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AHitmanDevCharacter::LookUpAtRate);

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AHitmanDevCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AHitmanDevCharacter::TouchStopped);
}

//Gestion de l'appuie du saut
void AHitmanDevCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

//Gestion de l'arret du saut
void AHitmanDevCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

//Gestion horizontale de la souris
void AHitmanDevCharacter::TurnAtRate(float Rate)
{
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

//Gestion verticale de la souris
void AHitmanDevCharacter::LookUpAtRate(float Rate)
{
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

//Input des mouvements devant/derriere
void AHitmanDevCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

//Input des mouvements droite/gauche
void AHitmanDevCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}


/****************************** State Getter part *********************************************/

//Retourne le status de la visibilité du joueur
HiddenState AHitmanDevCharacter::GetHiddenState()
{
	return Hidden;
}

//Permet de savoir si la cible a été éliminé
bool AHitmanDevCharacter::isTargetEliminated()
{
	return TargetEliminated;
}

/****************************** Attacks part *********************************************/

//Gère le début de la visé
void AHitmanDevCharacter::StartAiming()
{
	Aiming = true;
	CameraBoom->TargetArmLength -= AimingZoom;
}

//Gère la fin de la visé
void AHitmanDevCharacter::StopAiming()
{
	CameraBoom->TargetArmLength += AimingZoom;
	Aiming = false;
}

//Défini l'attaque selon l'état de la visé
void AHitmanDevCharacter::StartAttack()
{
	if (Aiming == true)
		HandleShoot();
	else
		HandleStab();
}

//Gère le tir
void AHitmanDevCharacter::HandleShoot()
{
	FVector CamLoc;
	FRotator CamRot;

	if (!canShoot)
		return;

	canShoot = false;

	Controller->GetPlayerViewPoint(CamLoc, CamRot);

	const FVector EndTrace = CamLoc + CamRot.Vector() * 5000; // and trace end is the camera location + an offset in the direction you are looking, the 200 is the distance at wich it checks

	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("WeaponTrace")), true, this);
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	DrawDebugLine(GetWorld(), CamLoc, EndTrace, FColor(255, 0, 0), true, 20, 0, 2);
	GetWorld()->LineTraceSingleByChannel(Hit, CamLoc, EndTrace, ECollisionChannel::ECC_Visibility);
	if (Hit.GetActor() != nullptr)
	{
		if (Hit.GetActor()->ActorHasTag("TARGET"))
		{
			Cast<ATarget>(Hit.GetActor())->OnDeath();
			TargetEliminated = true;
		}
		else if (Hit.GetActor()->ActorHasTag("PNJ"))
			Cast<APNJs>(Hit.GetActor())->OnDeath();
		else if (Hit.GetActor()->ActorHasTag("ENNEMY"))
			Cast<AEnnemies>(Hit.GetActor())->OnDeath();
	}
	FearAll();
	GetWorld()->GetTimerManager().SetTimer(PlayerTimerHandler, this, &AHitmanDevCharacter::EnableShoot, ShootCooldown, false);
}

//Défini l'attaque au corp a corp
void AHitmanDevCharacter::HandleStab()
{
	TArray<AActor*> Result;

	StabDetection->GetOverlappingActors(Result, AEnnemies::StaticClass());
	for (int i = 0; i != Result.Num(); i++)
	{
		if (Result[i]->ActorHasTag("ENNEMY"))
		{
			Hidden = HiddenState::Costumed;
			AEnnemies* ennemy = Cast<AEnnemies>(Result[i]);
			ennemy->OnDeath();
			SetEnnemieMaterial();
			return;
		}
	}

	StabDetection->GetOverlappingActors(Result, ATarget::StaticClass());
	for (int i = 0; i != Result.Num(); i++)
	{
		if (Result[i]->ActorHasTag("TARGET"))
		{
			Cast<ATarget>(Result[i])->OnDeath();
			TargetEliminated = true;
			return;
		}
	}

	StabDetection->GetOverlappingActors(Result, APNJs::StaticClass());
	for (int i = 0; i != Result.Num(); i++)
	{
		if (Result[i]->ActorHasTag("PNJ"))
		{
			Cast<APNJs>(Result[i])->OnDeath();
			return;
		}
	}
}

//Réactive l'attaque au corp a corp
void AHitmanDevCharacter::EnableShoot()
{
	canShoot = true;
}


/****************************** PNJs Interaction part *********************************************/


//Permet d'appeurer tous les PNJ et les Ennemis
void AHitmanDevCharacter::FearAll()
{
	TArray<AActor*> FoundIAs;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnnemies::StaticClass(), FoundIAs);
	for (int i = 0; i != FoundIAs.Num(); i++)
	{
		AEnnemies* ennemy = Cast<AEnnemies>(FoundIAs[i]);
		ennemy->ChasePlayer();
	}

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APNJs::StaticClass(), FoundIAs);
	for (int i = 0; i != FoundIAs.Num(); i++)
	{
		Cast<APNJs>(FoundIAs[i])->Fear();
	}
}

//Permet d'appeurer les PNJ donnés
void AHitmanDevCharacter::FearPNJs(TArray<AActor*> PNJsToFear)
{
	for (int i = 0; i != PNJsToFear.Num(); i++)
	{
		Cast<APNJs>(PNJsToFear[i])->Fear();
	}
}


/****************************** Environment Interaction part *********************************************/


//Fonction de détection d'environnement -> gestion de la foule
void AHitmanDevCharacter::OnDetectionBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("PNJ"))
	{
		APNJs* pnj = Cast<APNJs>(OtherActor);
		if (pnj->GetState() == PNJState::Crowded)
		{
			crowdCount += 1;

			if (crowdCount > 1 && GetHiddenState() != HiddenState::Costumed)
				Hidden = HiddenState::InCrowd;
		}
	}
}

//Fonction de détection d'environnement -> gestion de la foule
void AHitmanDevCharacter::OnDetectionEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag("PNJ"))
	{
		APNJs* pnj = Cast<APNJs>(OtherActor);
		if (pnj->GetState() == PNJState::Crowded)
		{
			crowdCount -= 1;

			if (crowdCount < 1 && GetHiddenState() != HiddenState::Costumed)
				Hidden = HiddenState::NONE;
		}
	}
}