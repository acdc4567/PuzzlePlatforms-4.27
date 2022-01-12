// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	SetIsReplicatedByDefault(true);
}


// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();
	if (GetOwnerRole() == ROLE_Authority){
		AActor *MyOwner = GetOwner();
		if (MyOwner){
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}

	Health=DefaultHealth;
}
void USHealthComponent::HandleTakeAnyDamage(
 AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
  class AController* InstigatedBy, AActor* DamageCauser){
	if (Damage <= 0){
		return;
	}
	Health=FMath::Clamp(Health-Damage,0.f,DefaultHealth);
	UE_LOG(LogTemp,Warning,TEXT("HealthChanged:%s"),*FString::SanitizeFloat(Health));

	OnHealthChanged.Broadcast(this,Health,Damage,DamageType,InstigatedBy,DamageCauser);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(USHealthComponent,Health);
}

//USHealthComponent
