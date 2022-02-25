// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoKartMovingComponent.h"

// Sets default values for this component's properties
UGoKartMovingComponent::UGoKartMovingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

// Called when the game starts
void UGoKartMovingComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UGoKartMovingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if( GetOwnerRole() == ROLE_AutonomousProxy )
	{
		_lastMove =
			CreateMove(DeltaTime);
		SimulateMove(_lastMove);	
	}
}

//	========================

void UGoKartMovingComponent::SimulateMove(
	const FGoKartMove& in_move
	)
{
	FVector Force = GetOwner()->GetActorForwardVector() * _maxDrivingForce * in_move._throttle;
	Force += GetAirResistence();
	Force += GetRollingResistence();
	FVector Acceleration = Force / _mass;
	_velocity = _velocity + Acceleration * in_move._deltatime;
	ApplyRotation(in_move._deltatime, in_move._steeringThrow);

	UpdateLocationFromVelocity(in_move._deltatime);
}

FGoKartMove UGoKartMovingComponent::CreateMove(
	float in_deltatime)
{
	FGoKartMove move;
	move._deltatime = in_deltatime;
	move._steeringThrow = _steeringThrow;
	move._throttle = _throttle;
	move._time = GetWorld()->TimeSeconds;

	return move;
	
}

FVector UGoKartMovingComponent::GetAirResistence()
{
	return -_velocity.GetSafeNormal() * _velocity.SizeSquared() * _dragCoefficient;
}

FVector UGoKartMovingComponent::GetRollingResistence()
{
	float accelerationDueToGravity = - GetWorld()->GetGravityZ() /100;
	float normalForce = _mass * accelerationDueToGravity;
	return - _velocity.GetSafeNormal() * _rollingResistenceCoefficient * normalForce;
}

void UGoKartMovingComponent::UpdateLocationFromVelocity(float DeltaTime)
{
	// Get The New Location Based on Pawn Velocity
	FVector translation =
		_velocity *
		MULTIPLY_FACTOR *
		DeltaTime;
	FHitResult result;
	// add new Forward vector position to the pawn
	GetOwner()->AddActorWorldOffset(translation,true,&result);

	if( result.IsValidBlockingHit() )
	{
		_velocity = FVector::ZeroVector;
	}
}

void UGoKartMovingComponent::ApplyRotation(
	float DeltaTime,
	float SteeringThrow)
{
	float DeltaLocation = FVector::DotProduct(GetOwner()->GetActorForwardVector(), _velocity) * DeltaTime;
	float RotationAngle =  DeltaLocation / _minTurningRadius * SteeringThrow;
	FQuat RotationDelta(GetOwner()->GetActorUpVector(), RotationAngle);

	_velocity = RotationDelta.RotateVector(_velocity);

	GetOwner()->AddActorWorldRotation(RotationDelta);
}
