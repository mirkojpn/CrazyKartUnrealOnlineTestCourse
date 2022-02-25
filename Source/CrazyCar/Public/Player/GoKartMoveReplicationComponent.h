// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/GoKartMovingComponent.h"
#include "GoKartMoveReplicationComponent.generated.h"


USTRUCT()
struct FGoKartState
{
	GENERATED_BODY()

	UPROPERTY()
	FGoKartMove _lastMove;

	UPROPERTY()
	FVector _velocity;

	UPROPERTY()
	FTransform _transform;
};

struct FHermiteCubicSpline
{
	FVector _startLocation;
	FVector _startDerivative;
	FVector _targetLocation;
	FVector _targetDerivative;

	FVector InterpolateLocation( float in_lerpRatio ) const
	{
		return 	FMath::CubicInterp(
						_startLocation,
						_startDerivative,
						_targetLocation,
						_targetDerivative,
						in_lerpRatio
						 );
	}
	FVector InterpolateDerivetive( float in_lerpRatio ) const
	{
		return FMath::CubicInterpDerivative(
											_startLocation,
											_startDerivative,
											_targetLocation,
											_targetDerivative,
											in_lerpRatio
											);
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CRAZYCAR_API UGoKartMoveReplicationComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGoKartMoveReplicationComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	

private:
	void ClearAcknowledgeMoves( FGoKartMove in_lastMove);
	void UpdateServerState( const FGoKartMove& in_move );
	void ClientTick(float in_deltatime);
	void InterpolateLocation(float lerpRatio, const FHermiteCubicSpline spline);
	void InterpolateVelocity(const FHermiteCubicSpline spline, float lerpRatio);
	void InterpolateRotation(float lerpRatio);
	FHermiteCubicSpline CreateSpline();
	float VelocityToDerivetive();

	/**
	 * @brief move the car to the forward vector based on it's input value
	 * @param in_move input value coming from controller
	 */
	UFUNCTION(Server,Reliable,WithValidation)
	void Server_SendMove(FGoKartMove in_move);
	UFUNCTION()
	void OnRep_ServerState();

	void SimulatedProxy_OnRep_ServerState();
	void AutonomusProxy_OnRep_ServerState();

	FTransform _clientStartTransform;
	FVector _clientStartVelocity;
	
	UPROPERTY(ReplicatedUsing= OnRep_ServerState )
	FGoKartState _serverState;
	UPROPERTY(VisibleAnywhere)
	UGoKartMovingComponent* _movingComponent;
	UPROPERTY()
	USceneComponent* _meshOffsetRoot;
	UFUNCTION(BlueprintCallable)
	void SetMeshOffsetRoot( USceneComponent* in_meshOffsetRoot ) { _meshOffsetRoot = in_meshOffsetRoot; };

	TArray<FGoKartMove> _unacknowledgedMoves;

	float _clientTimeSinceUpdate = 0.0f;
	float _clientTimeBetweenLastUpdate = 0.0f;

	float _clientSimulatedTime = 0.0f;
};
