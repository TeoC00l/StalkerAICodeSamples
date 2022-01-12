//@Author: Teodor Tysklind

#include "GP3AIBTPatrolPath.h"
#include "GP3AIStalkerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NavigationSystem.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GP3AINavMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UGP3AIBTPatrolPath::UGP3AIBTPatrolPath()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UGP3AIBTPatrolPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{	
	AGP3AIStalkerController* AICon = Cast<AGP3AIStalkerController>(OwnerComp.GetAIOwner());
	if(!AICon) return EBTNodeResult::Failed;
	AICon->StopMovement();

	UBlackboardComponent* BlackBoardComp = AICon->GetBlackBoardComp();
	if(!BlackBoardComp) return EBTNodeResult::Failed;

	FGP3AIBTPatrolPathMemory* MyMemory = CastInstanceNodeMemory<FGP3AIBTPatrolPathMemory>(NodeMemory);
	MyMemory->PathSpline = AICon->CurrentPath;
	MyMemory->AINavMovementComponent = Cast<UGP3AINavMovementComponent>(AICon->GetPawn()->GetMovementComponent());

	if( MyMemory->PathSpline == nullptr || MyMemory->AINavMovementComponent == nullptr ) return EBTNodeResult::Failed;

	const float InputKey = AICon->CurrentPath->FindInputKeyClosestToWorldLocation(AICon->GetPawn()->GetActorLocation());
	MyMemory->DistanceAlongPath = AICon->CurrentPath->GetDistanceAlongSplineAtSplineInputKey(InputKey);	

	SetMovementDirection(MyMemory);

	return EBTNodeResult::InProgress;
}

void UGP3AIBTPatrolPath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FGP3AIBTPatrolPathMemory* MyMemory = CastInstanceNodeMemory<FGP3AIBTPatrolPathMemory>(NodeMemory);
	AGP3AIStalkerController* AICon = Cast<AGP3AIStalkerController>(OwnerComp.GetAIOwner());

	MyMemory->DistanceAlongPath +=  MyMemory->MovementMagnitude * DeltaSeconds;

	if(MyMemory->DistanceAlongPath > MyMemory->PathSpline->GetSplineLength() || MyMemory->DistanceAlongPath < 0)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
	
	APawn* Pawn = AICon->GetPawn();
	const FVector Position = MyMemory->PathSpline->GetTransformAtDistanceAlongSpline(MyMemory->DistanceAlongPath,
		ESplineCoordinateSpace::World).GetTranslation();
	INavigationDataInterface* NavData = FNavigationSystem::GetNavDataForActor(*Pawn);
	const FNavAgentProperties& AgentProps = Pawn->GetMovementComponent()->GetNavAgentPropertiesRef();
	FNavLocation OutLocation;

	if(AgentProps == NULL || NavData == nullptr)
	{
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	const float SearchRadius = AgentProps.AgentRadius * 2.0f;
	const float SearchHeight = AgentProps.AgentHeight * AgentProps.NavWalkingSearchHeightScale;
	
	if(NavData->ProjectPoint(Position, OutLocation, FVector(SearchRadius,SearchRadius,SearchHeight)))
	{
		FVector CurrentLocation = Position;
		CurrentLocation.Z = Pawn->GetActorLocation().Z;

		const FRotator PlayerRot = UKismetMathLibrary::FindLookAtRotation(Pawn->GetActorLocation(), CurrentLocation);
		AICon->GetPawn()->SetActorLocation(CurrentLocation);
		Pawn->SetActorRotation(PlayerRot);
	}
}

void UGP3AIBTPatrolPath::SetMovementDirection(FGP3AIBTPatrolPathMemory* MyMemory)
{
	MyMemory->MovementMagnitude = MyMemory->AINavMovementComponent->PatrolSpeed;

	if(MyMemory->PathSpline->IsClosedLoop())
	{
		return;
	}
	
	const bool bReverseMoveDirection = MyMemory->DistanceAlongPath > MyMemory->PathSpline->GetSplineLength() / 2;

	if(bReverseMoveDirection)
	{
		MyMemory->MovementMagnitude *= -1;
	}
}

uint16 UGP3AIBTPatrolPath::GetInstanceMemorySize() const
{
	return sizeof(FGP3AIBTPatrolPathMemory);
}

