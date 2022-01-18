// Fill out your copyright notice in the Description page of Project Settings.


#include "STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "SHealthComponent.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Sound/SoundCue.h"
#include "EngineUtils.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp=CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	RootComponent=MeshComp;

	HealthCompx=CreateDefaultSubobject<USHealthComponent>("HealthComp");
	HealthCompx->OnHealthChanged.AddDynamic(this,&ASTrackerBot::HandleTakeDamage);
	
	SphereComp=CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetSphereRadius(200);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn,ECollisionResponse::ECR_Overlap);

	SphereComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();
	if(GetLocalRole()==ROLE_Authority){
		NextPathPoint=GetNextPathPoint();
	}
	//NextPathPoint=GetNextPathPoint();
	
}
/*
FVector ASTrackerBot::GetNextPathPoint(){
	TArray<AActor*> PlayersArray;
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ASCharacter::StaticClass(),PlayersArray);
	ACharacter* PlayerPawn=UGameplayStatics::GetPlayerCharacter(this,0);
	int32 RandomPlayer=0;
	if(PlayersArray.Num()>1){
		RandomPlayer=1;
		//RandomPlayer=FMath::RandRange(0,PlayersArray.Num()-1);
	}
	
	
	
	
	UNavigationPath *NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);
	if (NavPath&& NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}
	return GetActorLocation();
}
*/


FVector ASTrackerBot::GetNextPathPoint()
{
    AActor* BestTarget = nullptr;
    float NearestTargetDistance = FLT_MAX;
 
    for (TActorIterator<APawn> It(GetWorld()); It; ++It)
    {
        APawn* TestPawn=*It;
        if (TestPawn == nullptr || USHealthComponent::IsFriendly(TestPawn, this))
        {
            continue;
        }
 
        USHealthComponent* TestPawnHealthComp = Cast<USHealthComponent>(TestPawn->GetComponentByClass(USHealthComponent::StaticClass()));
        if (TestPawnHealthComp && TestPawnHealthComp->GetHealth() > 0.0f)
        {
            float Distance = (TestPawn->GetActorLocation() - GetActorLocation()).Size();
 
            if (Distance < NearestTargetDistance)
            {
                BestTarget = TestPawn;
                NearestTargetDistance = Distance;
            }
        }
    }
 
    if (BestTarget)
    {
        UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), BestTarget);
 
        GetWorldTimerManager().ClearTimer(TH_RefreshPath);
        GetWorldTimerManager().SetTimer(TH_RefreshPath, this, &ASTrackerBot::RefreshPath , 5.0f, false);
 
        if (NavPath && NavPath->PathPoints.Num() > 1)
        {
            // Return next point in the path
            return NavPath->PathPoints[1];
        }
    }
 
    // Failed to find path
    return GetActorLocation();
}

void ASTrackerBot::HandleTakeDamage(USHealthComponent * HealthComp, float Health, float HealthDelta,const class UDamageType * DamageType, class AController * InstigatedBy, AActor * DamageCauser){

	if(MatInst==nullptr){
		MatInst=MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0,MeshComp->GetMaterial(0));

	
	}
	if(MatInst){
		MatInst->SetScalarParameterValue("LastTimeDamageTaken",GetWorld()->TimeSeconds);

	}
	if(Health<=0){
		SelfDestruct();
	}

}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
	{
		return;
	}
	bExploded = true;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());
	MeshComp->SetVisibility(false,true	);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	if (GetLocalRole() == ROLE_Authority)
	{

		TArray<AActor *> IgnoredActors;
		IgnoredActors.Add(this);

		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, nullptr, IgnoredActors, this, GetInstigatorController(), true);

		

		SetLifeSpan(2.f);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor *OtherActor)
{

	Super::NotifyActorBeginOverlap(OtherActor);
	
	if(!bStartedSelfDestruction&&!bExploded) {
		ASCharacter *PlayerPawn = Cast<ASCharacter>(OtherActor);
		if (PlayerPawn)
		{

			if (GetLocalRole() == ROLE_Authority)
			{
				GetWorldTimerManager().SetTimer(TH_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.f);
			}

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent);
		}
	}
	
}

void ASTrackerBot::DamageSelf(){
	UGameplayStatics::ApplyDamage(this,20,GetInstigatorController(),this,nullptr);
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (GetLocalRole() == ROLE_Authority&&!bExploded)
	{

		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();
		if (DistanceToTarget <= RequiredMinDistanceToTarget)
		{

			NextPathPoint = GetNextPathPoint();
		}
		else
		{

			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();
			ForceDirection *= MovementForce;
			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);
		}
	}
}
void ASTrackerBot::RefreshPath(){
	if(GetLocalRole()==ROLE_Authority){
		NextPathPoint=GetNextPathPoint();
	}
}


//ASTrackerBot
