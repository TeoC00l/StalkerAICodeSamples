//Author: Teodor Tysklind

#include "GP3AIBTPerformSearch.h"
#include "DrawDebugHelpers.h"
#include "GP3AIPawnStalker.h"
#include "GP3AIStalkerController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GP3_Team4/PlayerCharacter/GP3PawnPlayer.h"
#include "Kismet/KismetSystemLibrary.h"

UGP3AIBTPerformSearch::UGP3AIBTPerformSearch()
{
	NodeName = "Perform Search";
	SearchTimePerState = 2.0f;
	SearchPadding = 50.0f;
	SearchDistance = 300.f;

	bNotifyTick = true;
}

EBTNodeResult::Type UGP3AIBTPerformSearch::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AGP3AIStalkerController* AIcon = Cast<AGP3AIStalkerController>(OwnerComp.GetAIOwner());
	if(!AIcon)
	{
		return EBTNodeResult::Failed;
	}

	AGP3AIPawnStalker* PawnStalker = Cast<AGP3AIPawnStalker>(AIcon->GetPawn());

	if(!PawnStalker)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComp = AIcon->GetBlackBoardComp();

	if(!BlackboardComp)
	{
			return EBTNodeResult::Failed;
	}

	FGP3AIBTPerformSearchMemory* MyMemory = CastInstanceNodeMemory<FGP3AIBTPerformSearchMemory>(NodeMemory);
	MyMemory->bExtendingForward = true;
	MyMemory->CurrentSearchState = 0;
	MyMemory->LerpTimeElapsed = 0.f;
	MyMemory->RemainingSearchTime = SearchTimePerState;
	
	PawnStalker->StartSearch(AIcon->GetPawn()->GetDefaultHalfHeight() - SearchPadding);

	return::EBTNodeResult::InProgress;
}

void UGP3AIBTPerformSearch::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FGP3AIBTPerformSearchMemory* MyMemory = CastInstanceNodeMemory<FGP3AIBTPerformSearchMemory>(NodeMemory);
	AGP3AIStalkerController* AIcon = Cast<AGP3AIStalkerController>(OwnerComp.GetAIOwner());
	AGP3AIPawnStalker* PawnStalker = Cast<AGP3AIPawnStalker>(AIcon->GetPawn());
	UBlackboardComponent* BlackboardComp = AIcon->GetBlackBoardComp();
	
	AActor* Player = Cast<AActor>(BlackboardComp->GetValueAsObject("Player"));
	APawn* Pawn = AIcon->GetPawn();

	float ZValue;
	if(MyMemory->CurrentSearchState == 0)
	{
		ZValue = Pawn->GetActorLocation().Z + Pawn->GetDefaultHalfHeight() - SearchPadding;	
	}
	else if (MyMemory->CurrentSearchState ==1)
	{
		ZValue = AIcon->GetPawn()->GetActorLocation().Z;
	}
	else
	{
		ZValue = Pawn->GetActorLocation().Z -Pawn->GetDefaultHalfHeight() + SearchPadding;
	}

	FVector Start(Pawn->GetActorLocation().X, Pawn->GetActorLocation().Y, ZValue);
	FVector PlayerLoc = Player->GetActorLocation();
	FVector Direction(PlayerLoc.X-Start.X,PlayerLoc.Y-Start.Y, 0);
	FVector DirectionNormal = Direction.GetSafeNormal();

	MyMemory->LerpTimeElapsed += GetWorld()->DeltaTimeSeconds *2;

	if(MyMemory->LerpTimeElapsed >= SearchTimePerState && MyMemory->bExtendingForward == true)
	{
		MyMemory->LerpTimeElapsed= 0.f;
		MyMemory->bExtendingForward = false;
	}
	else if(MyMemory->LerpTimeElapsed >= SearchTimePerState && MyMemory->bExtendingForward == false)
	{
		MyMemory->LerpTimeElapsed= 0.f;
		MyMemory->bExtendingForward = true;
	}
	
	Direction = DirectionNormal * SearchDistance;
	FVector End = Start + Direction;
	FVector SearchPosition;
	
	if(MyMemory->bExtendingForward)
	{
		SearchPosition = FMath::Lerp(Start, End, (MyMemory->LerpTimeElapsed/SearchTimePerState));
	}
	else
	{
		SearchPosition = FMath::Lerp(End, Start, (MyMemory->LerpTimeElapsed/SearchTimePerState));
	}

	AGP3AIPawnStalker* AI = Cast<AGP3AIPawnStalker>(AIcon->GetPawn());
	AI->TentacleLength = FVector::Distance(Start, SearchPosition);
		
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;
	
	bool bHit = UKismetSystemLibrary::LineTraceSingle( GetWorld(), Start, SearchPosition, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	if(bHit)
	{
		AGP3PawnPlayer* PlayerPawn = Cast<AGP3PawnPlayer>(Hit.Actor);

		if(PlayerPawn)
		{
			BlackboardComp->SetValueAsBool("bChasePlayer", true);
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}

	 if(MyMemory->RemainingSearchTime <= 0.0f)
	 {
	 	if(MyMemory->CurrentSearchState == 0)
	 	{
	 		PawnStalker->StartSearch(0.f);
	 	}
 
	 	if(MyMemory->CurrentSearchState == 1)
	 	{
	 		PawnStalker->StartSearch(-AIcon->GetPawn()->GetDefaultHalfHeight() + SearchPadding);
	 	}
	 }

	if(MyMemory->CurrentSearchState == 2 && MyMemory->RemainingSearchTime <= 0.0f)
	{
		MyMemory->CurrentSearchState = 0;
		MyMemory->RemainingSearchTime = SearchTimePerState;
			
		if(AIcon)
		{
			if(BlackboardComp)
			{
				BlackboardComp->SetValueAsBool("bSearch", false);
				BlackboardComp->SetValueAsBool("bChasePlayer", false);
			}
			
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}

	if(MyMemory->RemainingSearchTime <= 0.0f)
	{
		MyMemory->CurrentSearchState++;
		MyMemory->RemainingSearchTime = SearchTimePerState;
	}
	else
	{
		MyMemory->RemainingSearchTime -= DeltaSeconds;
	}
}

uint16 UGP3AIBTPerformSearch::GetInstanceMemorySize() const
{
	return sizeof(FGP3AIBTPerformSearchMemory);
}

