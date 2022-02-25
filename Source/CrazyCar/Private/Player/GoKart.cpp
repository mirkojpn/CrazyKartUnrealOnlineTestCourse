// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoKart.h"

#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
#include "Math/UnitConversion.h"
#include "Player/GoKartMovingComponent.h"
#include "Net//UnrealNetwork.h"

// Sets default values
AGoKart::AGoKart()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	_movingComponent =
		CreateDefaultSubobject<UGoKartMovingComponent>(TEXT("Movement Component"));
	_movingReplicatorComponent =
		CreateDefaultSubobject<UGoKartMoveReplicationComponent>(TEXT("Movement Replication Component"));
}

// Called when the game starts or when spawned
void AGoKart::BeginPlay()
{
	Super::BeginPlay();
	bReplicates = true;
	SetReplicateMovement(false);

	if( HasAuthority() )
	{
		NetUpdateFrequency = 1;
	}
}

FString GetEnumText(ENetRole Role)
{
	switch (Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

/**
 * @brief Called every frame
 */
void AGoKart::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);
}


/**
 * @brief Called to bind functionality to input
 */
void AGoKart::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward", this, &AGoKart::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AGoKart::MoveRight);
}

void AGoKart::MoveForward(float in_value)
{
	if( _movingComponent == nullptr ) return;
	_movingComponent->SetThrottle(in_value);
	
}

void AGoKart::MoveRight(float in_value)
{
	if( _movingComponent == nullptr ) return;
	_movingComponent->SetSteeringThrow ( in_value );
}



