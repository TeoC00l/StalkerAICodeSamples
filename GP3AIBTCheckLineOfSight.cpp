// Fill out your copyright notice in the Description page of Project Settings.


#include "GP3AIBTCheckLineOfSight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GP3AIStalkerController.h"
#include "GP3_Team4/PlayerCharacter/GP3PawnPlayer.h"
#include "Kismet/KismetSystemLibrary.h"

UGP3AIBTCheckLineOfSight::UGP3AIBTCheckLineOfSight()
{
	bNotifyTick = true;
}

void UGP3AIBTCheckLineOfSight::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	AGP3AIStalkerController* AIcon = Cast<AGP3AIStalkerController>(OwnerComp.GetAIOwner());
	
	if(!AIcon)
	{
		return;
	}

	UBlackboardComponent* BlackboardComp = AIcon->GetBlackBoardComp();

	if(!BlackboardComp)
	{
		return;
	}
	
	
	AActor* Player = Cast<AActor>(BlackboardComp->GetValueAsObject("Player"));
	APawn* Pawn = AIcon->GetPawn();

	//PERFORMING SEARCH
	if(!Pawn)
	{
		return;
	}

	FVector Position = AIcon->GetPawn()->GetActorLocation();

	float ZValue = Pawn->GetActorLocation().Z - (Pawn->GetDefaultHalfHeight() - 30.f);

	FVector Start(Pawn->GetActorLocation().X, Pawn->GetActorLocation().Y, ZValue);
	FVector End = Player->GetActorLocation();
		
	TArray<AActor*> ActorsToIgnore;
	FHitResult Hit;
	
	bool bHit = UKismetSystemLibrary::LineTraceSingle( GetWorld(), Start, End, ETraceTypeQuery::TraceTypeQuery2, false, ActorsToIgnore, EDrawDebugTrace::None, Hit, true);

	if(bHit)
	{
		AGP3PawnPlayer* PlayerPawn = Cast<AGP3PawnPlayer>(Hit.Actor);

		if(PlayerPawn)
		{
			AActor* Act = Cast<AActor>(PlayerPawn);

			BlackboardComp->SetValueAsBool("bHasLineOfSight", true);
			BlackboardComp->SetValueAsVector("LastSeenPlayerPosition", End);
		}else
		{
			BlackboardComp->SetValueAsBool("bHasLineOfSight", false);
		}
	}else
	{
		BlackboardComp->SetValueAsBool("bHasLineOfSight", false);
	}

}
