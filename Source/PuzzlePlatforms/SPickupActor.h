// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SPickupActor.generated.h"

class USphereComponent;
class UDecalComponent;
class ASPowerupActor;


UCLASS()
class PUZZLEPLATFORMS_API ASPickupActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASPickupActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere,Category=Components)
	USphereComponent* SphereComp;

	UPROPERTY(VisibleAnywhere,Category=Components)
	UDecalComponent* DecalComp;

	UPROPERTY(EditAnywhere,Category=PickupActor)
	TSubclassOf<ASPowerupActor> PowerUpClass;

	ASPowerupActor* PowerUpInstance;


	UPROPERTY(EditDefaultsOnly,Category=PickupActor)
	float CoolDownDuration;

	FTimerHandle TH_RespawnTimer;

	UFUNCTION()
	void Respawn();

public:	
	
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

};
