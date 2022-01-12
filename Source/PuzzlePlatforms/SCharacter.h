// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class PUZZLEPLATFORMS_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();


	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Components)
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USHealthComponent* HealthComp;

	bool bWantsToZoom=false;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Player)
	float ZoomedFOV=65;

	float DefaultFOV;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Player)
	float ZoomInterpSpeed=20;

	void BeginZoom();

	void EndZoom();

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category=Components)
	FName WeaponAttachSocketName="WeaponSocket";

	UPROPERTY(EditDefaultsOnly,Category=Player)
	TSubclassOf<ASWeapon> StarterWeaponClass;

	void StartFire();

	void StopFire();

	UFUNCTION()
	void OnHealthChangedx(USHealthComponent * HealthCompx, 
 float Health, float HealthDelta,
  const class UDamageType * DamageType, 
  class AController * InstigatedBy, AActor * DamageCauser);

  UPROPERTY(Replicated,VisibleAnywhere,BlueprintReadOnly,Category=Player)
	bool bDied;
	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FVector GetPawnViewLocation() const override;

private:




};
