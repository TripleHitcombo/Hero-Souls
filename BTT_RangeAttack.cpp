// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BTT_RangeAttack.h"
#include "AIController.h"
#include "GameFramework/Character.h"

EBTNodeResult::Type UBTT_RangeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacter* CharacterRef
	{
	  OwnerComp.GetAIOwner()->GetPawn<ACharacter>()
	};

	if (!IsValid(CharacterRef))
	{
		return EBTNodeResult::Failed;
	};

	CharacterRef->PlayAnimMontage(AnimMontage);
		
	return EBTNodeResult::Succeeded;

	
}
