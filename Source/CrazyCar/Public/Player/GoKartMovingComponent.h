// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GoKartMovingComponent.generated.h"


USTRUCT()
struct FGoKartMove
{
	GENERATED_BODY()
	
	UPROPERTY()
	float _throttle = 0.0f;

	UPROPERTY()
	float _steeringThrow = 0.0f;

	UPROPERTY()
	float _deltatime = 0.0f;

	UPROPERTY()
	float _time = 0.0f;

	bool IsValid() const
	{
		return FMath::Abs( _throttle ) <= 1.0f &&
					FMath::Abs( _steeringThrow ) <= 1.0f;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAZYCAR_API UGoKartMovingComponent : public UActorComponent
{
	GENERATED_BODY()

private :
	const float SPEED = 20.0f;
	
	const float MULTIPLY_FACTOR = 100.0f;
	// the mass of the car (kg)
	UPROPERTY( EditAnywhere )
	float _mass = 1000.0f;

	// the force applied to the car when the  throttle is fully down ( N ) 
	UPROPERTY( EditAnywhere )
	float _maxDrivingForce = 10000.0f;

	// minimum radius of the car turning circle at full lock (m)
	UPROPERTY( EditAnywhere )
	float _minTurningRadius = 10.0f;

	// highter mean more drag
	UPROPERTY( EditAnywhere )
	float _dragCoefficient = 16.0f;

	// highter mean more rolling resistence
	UPROPERTY( EditAnywhere )
	float _rollingResistenceCoefficient = 0.015f;

public:	
	// Sets default values for this component's properties
	UGoKartMovingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
//===
	void SetThrottle(float in_value) { _throttle = in_value; };
	void SetSteeringThrow(float in_value) { _steeringThrow = in_value; };
	void SetVelocity(const FVector& vector) { _velocity = vector; };
	FVector GetVelocity() { return _velocity; };
//===
	void SimulateMove( const FGoKartMove& in_move );
	void ApplyRotation(float DeltaTime, float SteeringThrow);
	void UpdateLocationFromVelocity(float DeltaTime);
	
	

	FGoKartMove GetLastMove() { return _lastMove; };
	
	FVector GetAirResistence();
	FVector GetRollingResistence();
private:
	FGoKartMove CreateMove(float in_deltatime);

	FVector _velocity;

	float _throttle = 0.0f;
	float _steeringThrow = 0.0f;

	FGoKartMove _lastMove;

};
