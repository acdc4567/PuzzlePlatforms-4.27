// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPowerupActor.generated.h"

UCLASS()
class PUZZLEPLATFORMS_API ASPowerupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPowerupActor();

protected:
	

	UPROPERTY(EditDefaultsOnly,Category=PowerUps)
	float PowerUpInterval=0.f;

	UPROPERTY(EditDefaultsOnly,Category=PowerUps)
	int32 TotalNoOfTicks=0;

	FTimerHandle TH_PowerUpTick;

	UFUNCTION()
	void OnTickPowerUp();

	int32 TicksProcessed=0;

	UPROPERTY(ReplicatedUsing=OnRep_PowerUpActive)
	bool bIsPowerUpActive=false;

	UFUNCTION()
	void OnRep_PowerUpActive();

	UFUNCTION(BlueprintImplementableEvent,Category=PowerUps)
	void OnPowerUpStateChanged(bool bNewIsActive);

public:	
	UFUNCTION(BlueprintImplementableEvent,Category=PowerUps)
	void OnActivated(AActor* OtherActorx1);

	void ActivatePowerUp(AActor* OtherActorx);

	UFUNCTION(BlueprintImplementableEvent,Category=PowerUps)
	void OnPowerUpTicked();


	UFUNCTION(BlueprintImplementableEvent,Category=PowerUps)
	void OnExpired();

};
