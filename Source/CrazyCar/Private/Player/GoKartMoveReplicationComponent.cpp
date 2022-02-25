// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/GoKartMoveReplicationComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UGoKartMoveReplicationComponent::UGoKartMoveReplicationComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicated(true);
	// ...
}


// Called when the game starts
void UGoKartMoveReplicationComponent::BeginPlay()
{
	Super::BeginPlay();

	_movingComponent =
		GetOwner()->FindComponentByClass<UGoKartMovingComponent>();
}


void UGoKartMoveReplicationComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UGoKartMoveReplicationComponent, _serverState);
}


// Called every frame
void UGoKartMoveReplicationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if( _movingComponent == nullptr ) { return; }
	FGoKartMove move =
		_movingComponent->GetLastMove();
	
	switch (GetOwner()->GetLocalRole())
	{
	case ROLE_AutonomousProxy :
		
		_unacknowledgedMoves.Add(
			move
			);
		Server_SendMove(
			move
			)
		;
		break;
	case ROLE_Authority:
		// if we are the server and in control of the pawn
		if(GetOwner()->GetRemoteRole())
		{
			UpdateServerState(move);
		}
		break;

	case ROLE_SimulatedProxy:
		ClientTick(DeltaTime);
		break;
	case ROLE_None :
		break;

	default: break;
	}
}

void UGoKartMoveReplicationComponent::InterpolateLocation(
	float in_lerpRatio,
	const FHermiteCubicSpline in_spline
	)
{
	FVector newLocation =
		in_spline.InterpolateLocation( in_lerpRatio );
	if( _meshOffsetRoot != nullptr )
	{
		_meshOffsetRoot->SetWorldLocation( newLocation );
	}
}

void UGoKartMoveReplicationComponent::InterpolateVelocity(
	const FHermiteCubicSpline spline,
	float lerpRatio
	)
{
	FVector newDerivetive =
		spline.InterpolateDerivetive( lerpRatio );

	FVector newVelocity = newDerivetive / VelocityToDerivetive();

	_movingComponent->SetVelocity( newVelocity );
}

void UGoKartMoveReplicationComponent::InterpolateRotation(
	float lerpRatio
	)
{
	FQuat targetRotation =
		_serverState._transform.GetRotation();
	FQuat startRotation =
		_clientStartTransform.GetRotation();
	FQuat newRotation =
		FQuat::Slerp(startRotation,targetRotation,lerpRatio);
	
	if( _meshOffsetRoot != nullptr )
	{
		_meshOffsetRoot->SetWorldRotation( newRotation );
	}
}

void UGoKartMoveReplicationComponent::ClientTick(
	float in_deltatime)
{
	_clientTimeSinceUpdate += in_deltatime;
	
	if( _clientTimeBetweenLastUpdate < KINDA_SMALL_NUMBER ) return;
	if( _movingComponent == nullptr ) return;


	float lerpRatio =
	_clientTimeSinceUpdate / _clientTimeBetweenLastUpdate;
	
	FHermiteCubicSpline spline =
							CreateSpline();

	InterpolateLocation(
		        lerpRatio,
		        spline
		        );

	InterpolateVelocity(
				spline,
				lerpRatio
				);

	InterpolateRotation(
				lerpRatio
				); 
}

FHermiteCubicSpline UGoKartMoveReplicationComponent::CreateSpline()
{
	float velocityToDerivetive =
						VelocityToDerivetive();
	
	FHermiteCubicSpline spline;
	spline._targetLocation =
		_serverState._transform.GetLocation();

	spline._startLocation =
		_clientStartTransform.GetLocation();

	spline._startDerivative =
		_clientStartVelocity * velocityToDerivetive;
	spline._targetDerivative =
		_serverState._velocity * velocityToDerivetive;

	return spline;
}

float UGoKartMoveReplicationComponent::VelocityToDerivetive()
{
	return _clientTimeBetweenLastUpdate * 100;;
}

void UGoKartMoveReplicationComponent::OnRep_ServerState()
{
	switch ( GetOwnerRole() )
	{
	case ROLE_AutonomousProxy :
		AutonomusProxy_OnRep_ServerState();
		break;
	case ROLE_SimulatedProxy :
		SimulatedProxy_OnRep_ServerState();
		break;
		
	default:
		break;
	}
}

void UGoKartMoveReplicationComponent::SimulatedProxy_OnRep_ServerState()
{
	if( _movingComponent == nullptr ) return;
	_clientTimeBetweenLastUpdate = _clientTimeSinceUpdate;
	_clientTimeSinceUpdate = 0.0f;
	if( _meshOffsetRoot != nullptr )
	{
		_clientStartTransform.SetLocation( _meshOffsetRoot->GetComponentLocation() );
		_clientStartTransform.SetRotation( _meshOffsetRoot->GetComponentQuat() );
	}
	
	_clientStartVelocity = _movingComponent->GetVelocity();

	GetOwner()->SetActorTransform( _serverState._transform );
}

void UGoKartMoveReplicationComponent::AutonomusProxy_OnRep_ServerState()
{
	if( _movingComponent == nullptr ) return;
	
	GetOwner()->SetActorTransform(_serverState._transform);
	_movingComponent->SetVelocity ( _serverState._velocity );

	ClearAcknowledgeMoves(_serverState._lastMove);


	for ( const FGoKartMove& move : _unacknowledgedMoves )
	{
		_movingComponent->SimulateMove(move);
	}
}

void UGoKartMoveReplicationComponent::ClearAcknowledgeMoves(
	FGoKartMove in_lastMove
	)
{
	TArray<FGoKartMove> newMoves;
	for ( const FGoKartMove& move : _unacknowledgedMoves )
	{
		if( move._time > in_lastMove._time)
		{
			newMoves.Add(move);
		}
	}

	_unacknowledgedMoves = newMoves;
}

void UGoKartMoveReplicationComponent::UpdateServerState(
	const FGoKartMove& in_move
	)
{
	_serverState._lastMove = in_move;
	_serverState._transform = GetOwner()->GetActorTransform();
	_serverState._velocity = _movingComponent->GetVelocity();
}

/**
 * @brief move the car to the forward vector based on it's input value
 * @param in_value input value coming from controller
 */
void UGoKartMoveReplicationComponent::Server_SendMove_Implementation(
	FGoKartMove in_move)
{
	if( _movingComponent == nullptr ) return;
	_clientSimulatedTime += in_move._deltatime;
	_movingComponent->SimulateMove(in_move);

	UpdateServerState(in_move);

}
bool UGoKartMoveReplicationComponent::Server_SendMove_Validate(
	FGoKartMove in_move)
{
	float proposedTime = _clientSimulatedTime + in_move._deltatime;
	bool clientNotRunningAhead = proposedTime < GetWorld()->TimeSeconds;

	if( clientNotRunningAhead == false )
	{
		UE_LOG(LogTemp,Error,TEXT("Client is running too fast"));
		return  false;
	}

	if( in_move.IsValid() == false )
	{
		UE_LOG(LogTemp,Error,TEXT("Received invalid move"));
		return  false;		
	}
	return true;
}


