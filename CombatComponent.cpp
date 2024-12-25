// Fill out your copyright notice in the Description page of Project Settings.


#include "Combat/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interfaces/MainCharacterInterface.h" 

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	CharacterRef = GetOwner<ACharacter>();
	
}


// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UCombatComponent::ComboAttack()
{
	if (CharacterRef->Implements<UMainCharacterInterface>())
	{
		IMainCharacterInterface* IMainCharacterRef{ Cast<IMainCharacterInterface>(CharacterRef) };

		if (IMainCharacterRef  && !IMainCharacterRef->HasEnoughStamina(StaminaCost))
		{
			return;
		}
	}
	
	if (!bCanAttack)
	{
		return;
	}

	bCanAttack = false;
	
	CharacterRef->PlayAnimMontage(AttackAnimations[ComboCounter]); //play the animation from the first iteration

	ComboCounter++;//increment each animation

	int MaxCombo{ AttackAnimations.Num() };

	ComboCounter = UKismetMathLibrary::Wrap //allos the array to restart on completion
	(
	ComboCounter,
	-1, //using wrap always go one belowe first number	
	(MaxCombo - 1)
	);

	OnAttackPerformedDelegate.Broadcast(StaminaCost);
}

void UCombatComponent::HandleResetAttack()
{
	bCanAttack = true;
}

