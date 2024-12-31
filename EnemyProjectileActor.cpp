// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/EnemyProjectileActor.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Engine/DamageEvents.h"

// Sets default values
AEnemyProjectileActor::AEnemyProjectileActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AEnemyProjectileActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyProjectileActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEnemyProjectileActor::HandleBeginOverlap(AActor* OtherActor)
{
	APawn* PawnRef{ Cast<APawn>(OtherActor) };

	if (!PawnRef->IsPlayerControlled()) { return; }

	FindComponentByClass<UParticleSystemComponent>()
		->SetTemplate(HitTemplate);

	FindComponentByClass<UProjectileMovementComponent>()
		->StopMovementImmediately();

	FTimerHandle DeathTimerHandle{};

	GetWorldTimerManager().SetTimer(
		DeathTimerHandle,
		this,
		&AEnemyProjectileActor::DestroyProjectile,
		0.5f
	);

	FindComponentByClass<USphereComponent>()
		->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FDamageEvent ProjectileAttackEvent{};

	PawnRef->TakeDamage(
		Damage,
		ProjectileAttackEvent,
		PawnRef->GetController(),
		this
	);



}

void AEnemyProjectileActor::DestroyProjectile()
{
	Destroy();
}
