// Fill out your copyright notice in the Description page of Project Settings.

// ReSharper disable All
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Player/GoKartMovingComponent.h"
#include "Player/GoKartMoveReplicationComponent.h"
#include "GoKart.generated.h"




UCLASS()
class CRAZYCAR_API AGoKart : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AGoKart();
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UGoKartMovingComponent* _movingComponent;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UGoKartMoveReplicationComponent* _movingReplicatorComponent;
protected: 
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
private:
	

	

	void MoveForward(float in_value);
	void MoveRight( float in_value );

};
