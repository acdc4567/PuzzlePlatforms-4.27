// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_SixParams(FOnHealthChangedSignature,
 USHealthComponent *, HealthComp, 
 float, Health, float, HealthDelta,
  const class UDamageType *, DamageType, 
  class AController *, InstigatedBy, AActor *, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PUZZLEPLATFORMS_API USHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USHealthComponent();

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=HealthComponent)
	uint8 TeamNum=255;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	bool bIsDead=false;

	UPROPERTY(ReplicatedUsing=OnRep_Health, BlueprintReadOnly,Category=HealthComponent)
	float Health;

	UFUNCTION() 
	void OnRep_Health(float OldHealth);

	UFUNCTION()
	void HandleTakeAnyDamage( AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(Client, Reliable)
	void ClientOnHealthChanged(USHealthComponent* HealthComponent, 
	float SHealth, float HealthDelta, 
	const class UDamageType* DamageType, class AController* InstigatedBy,
	 AActor* DamageCauser);
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category=HealthComponent)
	float DefaultHealth=100.f;


public:	
	UPROPERTY(BlueprintAssignable,Category="Events")
	FOnHealthChangedSignature OnHealthChanged;
		
	UFUNCTION(BlueprintCallable,Category=HealthComponent)
	void Heal(float HealAmount);

	FORCEINLINE float GetHealth() const {return Health;}

	UFUNCTION(BlueprintCallable,BlueprintPure,Category=HealthComponent)
	static bool IsFriendly(AActor* ActorA,AActor* ActorB);


};

