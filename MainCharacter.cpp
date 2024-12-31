// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/MainCharacter.h"
#include "Animations/PlayerAnimInstance.h"
#include "Characters/StatsComponent.h"
#include "Characters/EStat.h"
#include "Combat/LockOnComponent.h"
#include "Combat/CombatComponent.h"
#include "Combat/TraceComponent.h"
#include "Combat/BlockComponent.h"
#include "Characters/PlayerActionsComponent.h"


// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StatsComp = CreateDefaultSubobject<UStatsComponent>(TEXT("Stats Component"));
	LockOnComp = CreateDefaultSubobject<ULockOnComponent>(TEXT("LockOn Component"));
	CombatComp = CreateDefaultSubobject<UCombatComponent>(TEXT("Combat Component"));
	TraceComp = CreateDefaultSubobject<UTraceComponent>(TEXT("Trace Component"));
	BlockComp = CreateDefaultSubobject<UBlockComponent>(TEXT("Block Component"));
	PlayerActionsComp = CreateDefaultSubobject<UPlayerActionsComponent>(TEXT("PlayerActions Component"));

	HurtAnimMontage = nullptr;
	DeathAnimMontage = nullptr;

}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	PlayerAnim = Cast<UPlayerAnimInstance>(GetMesh()->GetAnimInstance());
	
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float AMainCharacter::GetDamage()
{
	return StatsComp->Stats[EStat::Strength];
}

bool AMainCharacter::HasEnoughStamina(float Cost)
{
	return StatsComp->Stats[EStat::Stamina] >= Cost;
}

void AMainCharacter::HandleDeath()
{
	if (!DeathAnimMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("DeathAnimMontage is null! Cannot play death animation."));
		return;
	}
	
	PlayAnimMontage(DeathAnimMontage);

	DisableInput(GetController<APlayerController>());
}

void AMainCharacter::EndLockOnWithActor(AActor* ActorRef)
{
	if (LockOnComp->CurrentTargetActor != ActorRef)
	{
		return;
	}

	LockOnComp->EndLockOn();

}

bool AMainCharacter::CanTakeDamage(AActor* Opponent)
{
	UE_LOG(LogTemp, Warning, TEXT("CanTakeDamage called"));

	if (PlayerActionsComp && PlayerActionsComp->bIsRollActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot take damage while rolling"));
		return false;
	}

	if (PlayerAnim && PlayerAnim->isBlocking)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blocking detected; checking BlockComp"));
		return BlockComp && BlockComp->BlockSuccess(Opponent);
	}

	return true;
}

void AMainCharacter::PlayHurtAnim(TSubclassOf<class UCameraShakeBase> CameraShakeTemplate)
{

	if (!HurtAnimMontage)
	{
		UE_LOG(LogTemp, Error, TEXT("HurtAnimMontage is null! Cannot play hurt animation."));
		return;
	}

	PlayAnimMontage(HurtAnimMontage);

	if (CameraShakeTemplate)
	{
		GetController<APlayerController>()
			->ClientStartCameraShake(CameraShakeTemplate);
	}
}

