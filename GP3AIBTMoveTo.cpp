//@Author: Teodor Tysklind

#include "GP3AIBTMoveTo.h"
#include "GP3AINavMovementComponent.h"
#include "GP3AIPawnStalker.h"
#include "GP3AIStalkerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"

UGP3AIBTMoveTo::UGP3AIBTMoveTo()
{
	bNotifyTick = true;
	StopOnOverlap = 1;
}

EBTNodeResult::Type UGP3AIBTMoveTo::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return EBTNodeResult::InProgress;
}

void UGP3AIBTMoveTo::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AGP3AIStalkerController* AIcon = Cast<AGP3AIStalkerController>(OwnerComp.GetAIOwner());
	if(!AIcon)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
		
	UBlackboardComponent* BlackboardComp = AIcon->GetBlackBoardComp();

	AGP3AIPawn* AI = Cast<AGP3AIPawnStalker>(AIcon->GetPawn());
	
	if(!AI)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	
	UGP3AINavMovementComponent* NavMovementComponent = AI->AINavMovementComponent;
	if(!NavMovementComponent)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	NavMovementComponent->bHasRequestedVelocity = true;

	UObject* KeyValue = BlackboardComp->GetValue<UBlackboardKeyType_Object>(BlackboardKey.GetSelectedKeyID());
	AActor* TargetActor = Cast<AActor>(KeyValue);

	FVector TargetLocation = FVector::ZeroVector;
	
	if(TargetActor)
	{
		TargetLocation = TargetActor->GetActorLocation();
	}				
	else if (BlackboardKey.SelectedKeyType == UBlackboardKeyType_Vector::StaticClass())
	{
		TargetLocation = BlackboardComp->GetValue<UBlackboardKeyType_Vector>(BlackboardKey.GetSelectedKeyID());
	}

	if(TargetLocation == FVector::ZeroVector)
	{
		return;
	}

	SetRotation(NavMovementComponent, AI, DeltaSeconds);
	Move(OwnerComp, AIcon, NavMovementComponent, TargetLocation);
}

void UGP3AIBTMoveTo::Move(UBehaviorTreeComponent& OwnerComp, AGP3AIStalkerController* AIcon,
	UGP3AINavMovementComponent* NavMovementComponent, const FVector TargetLocation) const
{
	if(StopOnOverlap)
	{
		AIcon->MoveToLocation(TargetLocation, 0.f, true, true);
	}
	else
	{
		AIcon->MoveToLocation(TargetLocation, 0.f, false, true);
	}

	const float DistSq = FVector::DistSquared(TargetLocation, AIcon->GetPawn()->GetActorLocation());

	if(DistSq <= (AcceptableRadius*AcceptableRadius))
	{
		NavMovementComponent->bHasRequestedVelocity = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

void UGP3AIBTMoveTo::SetRotation(UGP3AINavMovementComponent* NavMovementComponent,AGP3AIPawn* AI, float DeltaSeconds) const
{
	const FVector Velocity = NavMovementComponent->RequestedVelocity;
	const FQuat CurrentRot = AI->GetActorQuat();
	const FQuat TargetRot = Velocity.GetSafeNormal().Rotation().Quaternion();
	AI->SetActorRotation(FQuat::Slerp(CurrentRot, TargetRot, DeltaSeconds * RotationSpeed));
}
