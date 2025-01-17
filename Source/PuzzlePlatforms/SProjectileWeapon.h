// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "SProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API ASProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()
public:
ASProjectileWeapon();

protected:
	virtual void Fire() override;
	
	UPROPERTY(EditDefaultsOnly,Category=ProjectileWeapon)
	TSubclassOf<AActor> ProjectileClass;


};
