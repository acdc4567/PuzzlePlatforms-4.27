// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "STrackerBot.generated.h"

class UStaticMeshComponent;
class USHealthComponent;
class USphereComponent;

class USoundCue;



UCLASS()
class PUZZLEPLATFORMS_API ASTrackerBot : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ASTrackerBot();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleDefaultsOnly,Category=Components)
	UStaticMeshComponent* MeshComp;

	

	FVector NextPathPoint;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	float MovementForce=500;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	bool bUseVelocityChange=true;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	float RequiredMinDistanceToTarget=100;

	USHealthComponent* HealthCompx;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	USphereComponent* SphereComp;

	UFUNCTION()
	void HandleTakeDamage(USHealthComponent * HealthComp, float Health, float HealthDelta,const class UDamageType * DamageType, class AController * InstigatedBy, AActor * DamageCauser);

	UMaterialInstanceDynamic* MatInst;

	void SelfDestruct();

	UPROPERTY(EditDefaultsOnly,Category=Components)
	UParticleSystem* ExplosionEffect;

	bool bExploded=false;

	bool bStartedSelfDestruction=false;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	float ExplosionRadius=200.f;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	float ExplosionDamage=40.f;

	UPROPERTY(EditDefaultsOnly,Category=Components)
	float SelfDamageInterval=.25f;

	FTimerHandle TH_SelfDamage;

	FTimerHandle TH_RefreshPath;

	UFUNCTION()
	void DamageSelf();

	UPROPERTY(EditDefaultsOnly,Category=Sound)
	USoundCue* SelfDestructSound;

	UPROPERTY(EditDefaultsOnly,Category=Sound)
	USoundCue* ExplodeSound;

	UFUNCTION()
	void RefreshPath();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	FVector GetNextPathPoint();
};
