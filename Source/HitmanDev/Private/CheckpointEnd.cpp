// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckpointEnd.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

//////////////////////////////////////////////////////////////////////////
// ACheckpointEnd

ACheckpointEnd::ACheckpointEnd()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	TriggerZone = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Trigger Zone"));
	TriggerZone->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeBase(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeBase.Succeeded())
			TriggerZone->SetStaticMesh(CubeBase.Object);

	TriggerZone->SetGenerateOverlapEvents(true);
	TriggerZone->SetCollisionProfileName("OverlapAll");
	TriggerZone->OnComponentBeginOverlap.AddDynamic(this, &ACheckpointEnd::OnOverlapBegin);

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialZone(TEXT("/Game/Materials/M_CheckpointTexture.M_CheckpointTexture"));
	if (MaterialZone.Succeeded())
		ZoneMaterial = MaterialZone.Object;
	else
		UE_LOG(LogTemp, Warning, TEXT("NOT LOAD"));

}

// Called to bind functionality to input
void ACheckpointEnd::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

//permet d'activer le checkpoint et de le montrer
void ACheckpointEnd::Enable()
{
	isEnable = true;
	TriggerZone->SetMaterial(0, ZoneMaterial);
}

//Permet de détecter l'entrée d'entité et notamment du joueur dedans
void ACheckpointEnd::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("PLAYER"))
	{
		if (isEnable)
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	}
}
