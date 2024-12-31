// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/BTT_MeleeAttack.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "Interfaces/FighterInterface.h"
#include "GameFramework/Character.h"
#include "Characters/EEnemyState.h"

EBTNodeResult::Type UBTT_MeleeAttack::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	bIsFinished = false;
	
	float Distance
	{
		OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"))
	};

	AAIController* AIRef{ OwnerComp.GetAIOwner() };

	if (Distance > AttackRadius)
	{
		APawn* PlayerRef{ GetWorld()->GetFirstPlayerController()->GetPawn() };
		FAIMoveRequest MoveRequest{ PlayerRef };
		MoveRequest.SetUsePathfinding(true);
		MoveRequest.SetAcceptanceRadius(AcceptableRadius);

		AIRef->ReceiveMoveCompleted.AddUnique
		(
			MoveDelegate
		);

		AIRef->MoveTo(MoveRequest);
		AIRef->SetFocus(PlayerRef);

	}
	else
	{
		IFighterInterface* FighterRef{ Cast<IFighterInterface>(AIRef->GetCharacter()) };

		FighterRef->Attack();

		FTimerHandle AttackTimerHandle;

		AIRef->GetCharacter()->GetWorldTimerManager().SetTimer
		(
			AttackTimerHandle, 
			this, 
			&UBTT_MeleeAttack::FinishAttackTask,
			FighterRef->GetAnimDuration(),
			false
		);
			
	}
	
	return EBTNodeResult::InProgress;
}

void UBTT_MeleeAttack::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	float Distance
	{
		OwnerComp.GetBlackboardComponent()->GetValueAsFloat(TEXT("Distance"))
	};

	AAIController* AIRef{ OwnerComp.GetAIOwner() };
			
	IFighterInterface* FighterRef
	{
		Cast<IFighterInterface>(AIRef->GetCharacter())
	};

	if (Distance > FighterRef->GetMeleeRange())
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsEnum
		(TEXT("CurrentState"), EEnemyState::Range);

		AbortTask(OwnerComp, NodeMemory);

		FinishLatentTask(OwnerComp, EBTNodeResult::Aborted);

		AIRef->StopMovement();
		AIRef->ClearFocus(EAIFocusPriority::Gameplay);

		AIRef->ReceiveMoveCompleted.Remove(MoveDelegate);
			
	}
	
	if (!bIsFinished)
	{
		return;
	}

	OwnerComp.GetAIOwner()->ReceiveMoveCompleted.Remove(MoveDelegate);

	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
}

UBTT_MeleeAttack::UBTT_MeleeAttack()
{
	MoveDelegate.BindUFunction(this, "FinishAttackTask");
	bNotifyTick = true;
}

void UBTT_MeleeAttack::FinishAttackTask()
{

	bIsFinished = true;
}
