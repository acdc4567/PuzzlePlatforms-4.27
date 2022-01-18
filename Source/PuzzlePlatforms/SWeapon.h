// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SWeapon.generated.h"

class USkeletalMeshComponent;
class UDamageType;
class UParticleSystem;

USTRUCT()
struct FHitScanTrace{
	GENERATED_BODY()
public:
	UPROPERTY()
	TEnumAsByte<EPhysicalSurface> SurfaceType;

	UPROPERTY()
	FVector_NetQuantize TraceTo;
	
};

UCLASS()
class PUZZLEPLATFORMS_API ASWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Components)
	USkeletalMeshComponent* MeshComp;

	

	void PlayFireEffects(FVector TraceEnd);

	void PlayImpactEffects(EPhysicalSurface SurfaceType,FVector ImpactPoint);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	FName TracerTargetName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=Weapon)
	UParticleSystem* FleshImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	UParticleSystem *TracerEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float BaseDamage=20.f;

	UFUNCTION()
	virtual void Fire();

	UFUNCTION(Server,Reliable,WithValidation)
	void ServerFire();

	FTimerHandle TH_TimeBWShots;

	float LastFireTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon)
	float RateOfFire=600;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Weapon,meta=(ClampMin=0))
	float BulletSpread=2.0f;

	float TimeBWShots;

	UPROPERTY(ReplicatedUsing=OnRep_HitScanTrace)
	FHitScanTrace HitScanTrace;

	UFUNCTION()
	void OnRep_HitScanTrace();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	

	void StartFire();

	void StopFire();

};
