// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SWeapon.h"
#include "Components/CapsuleComponent.h"
#include "SHealthComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp=CreateDefaultSubobject<USpringArmComponent>("SpringArmComp");
	SpringArmComp->bUsePawnControlRotation=true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp=CreateDefaultSubobject<UCameraComponent>("CameraComp");
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp=CreateDefaultSubobject<USHealthComponent>("HealthComp");

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_GameTraceChannel1,ECR_Ignore);
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	DefaultFOV = CameraComp->FieldOfView;
	HealthComp->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChangedx);

	if (GetLocalRole() == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(
			StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
		if (CurrentWeapon){
			CurrentWeapon->SetOwner(this);
			CurrentWeapon->AttachToComponent(GetMesh(),
			FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachSocketName);
		}
	}
}

void ASCharacter::MoveForward(float Value){

	AddMovementInput(GetActorForwardVector()*Value);

}
void ASCharacter::MoveRight(float Value){
	AddMovementInput(GetActorRightVector()*Value);
}

void ASCharacter::BeginCrouch(){
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch=true;
	Crouch();
}


void ASCharacter::EndCrouch(){
	UnCrouch();
}

FVector ASCharacter::GetPawnViewLocation() const{
	if(CameraComp){
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

void ASCharacter::EndZoom(){
	bWantsToZoom=false;
}

void ASCharacter::BeginZoom(){
	bWantsToZoom=true;
}

void ASCharacter::StartFire(){
	if (CurrentWeapon){
		CurrentWeapon->StartFire();
	}
}

void ASCharacter::StopFire(){
	if (CurrentWeapon){
		CurrentWeapon->StopFire();
	}
}

void ASCharacter::OnHealthChangedx(USHealthComponent * HealthCompx, 
 float Health, float HealthDelta,
  const class UDamageType * DamageType, 
  class AController * InstigatedBy, AActor * DamageCauser){
	if (Health <= 0.f && !bDied){
		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bDied=true;
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV=bWantsToZoom? ZoomedFOV:DefaultFOV;
	float NewFOV=FMath::FInterpTo(CameraComp->FieldOfView,
	TargetFOV,DeltaTime,ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	check(PlayerInputComponent);
	PlayerInputComponent->BindAxis("MoveForward",this,&ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight",this,&ASCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookUp",this,&ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn",this,&ASCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Crouch",IE_Pressed,this,&ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch",IE_Released,this,&ASCharacter::EndCrouch);
	
	PlayerInputComponent->BindAction("Jump",IE_Pressed,this,&ACharacter::Jump);
	
	PlayerInputComponent->BindAction("ADS",IE_Pressed,this,&ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("ADS",IE_Released,this,&ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Fire",IE_Pressed,this,&ASCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire",IE_Released,this,&ASCharacter::StopFire);
	

}

void ASCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ASCharacter,CurrentWeapon);
	DOREPLIFETIME(ASCharacter,bDied);
}
//ASCharacter
