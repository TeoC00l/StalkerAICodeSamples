
#include "GP3AIStalkerController.h"
#include "GP3AIPawnStalker.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "GP3_Team4/PlayerCharacter/GP3PawnPlayer.h"
#include "GP3_Team4/PlayerCharacter/Health/GP3HealthComponent.h"
#include "Kismet/GameplayStatics.h"

AGP3AIStalkerController::AGP3AIStalkerController()
{
	BehaviorTreeComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));

	PlayerKey = "Target";
	LocationToGetKey = "LocationToGo";
	Path = "Path";
	
}

void AGP3AIStalkerController::SetPlayerCaught(APawn* Player) const
{
	if(BlackboardComp)
	{
		BlackboardComp->SetValueAsObject(PlayerKey, Player);
	}
}

void AGP3AIStalkerController::OnPossess(APawn *InPawn)
{
	Super::OnPossess(InPawn);

	AIChar = Cast<AGP3AIPawnStalker>(InPawn);

	if(!AIChar)
	{
		return;
	}

	if(!AIChar->BehaviourTree->BlackboardAsset)
	{
		return;
	}

	//initialize blackboard
	BlackboardComp->InitializeBlackboard(*(AIChar->BehaviourTree->BlackboardAsset));

	//Delay setting up the blackboard keys to ensure that everything is set up correctly
	FTimerHandle BBTimer;
	FTimerDelegate BBTimerDelegate;
	BBTimerDelegate.BindUFunction(this, "SetupBlackBoardKeys");

	GetWorldTimerManager().SetTimer(BBTimer, BBTimerDelegate, 1.0f, false, 0.3f);

}

void AGP3AIStalkerController::OnPlayerDeath()
{
	if(BlackboardComp)
	{
		if(BlackboardComp->GetValueAsBool("bChasePlayer") == true)
		{
			BlackboardComp->SetValueAsFloat("TimeSinceHeardNoise", 2.f);
			BlackboardComp->SetValueAsBool("bChasePlayer", false);
		}
	}
}

void AGP3AIStalkerController::SetupBlackBoardKeys()
{
	//Get Path Splines
	TArray<AActor*> PathActors = AIChar->PathActors;

	if(PathActors.Num() != 0)
	{
		for(auto* Actor : PathActors)
		{
			if(Actor == nullptr)
			{
				continue;
			}
			
			USplineComponent* PathSpline = Actor->FindComponentByClass<USplineComponent>();
			if(PathSpline)
			{
				PathSplines.Add(PathSpline);
				CurrentPath = PathSplines[0];
			}
		}
	}
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *GetName());

	if(!PC)
	{
		UE_LOG(LogTemp, Warning, TEXT(" %s: Player controller not found."), *GetName());
		return;
	}

	AGP3PawnPlayer* Player = Cast<AGP3PawnPlayer>(PC->GetPawn());

	if(!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : Failed to get player pawn from controller."), *GetName());
		return;
	}
			
	if(Player)
	{
		BlackboardComp->SetValueAsObject("Player", Player);
	}

	BlackboardComp->SetValueAsObject("SelfActor", GetPawn());
	BlackboardComp->SetValueAsFloat("TimeSinceHeardNoise", 2.0f);

	

	//START TREE AFTER EVERYTHING IS SETUP
	if(CurrentPath)
	{
		BehaviorTreeComp->StartTree(*AIChar->BehaviourTree);
	}

	UGP3HealthComponent* Health = Cast<UGP3HealthComponent>(Player->GetComponentByClass(UGP3HealthComponent::StaticClass()));

	if(!Health)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s : Failed to get player health component."), *GetName());
	}

	Health->OnDeathEvent.AddDynamic(this, &AGP3AIStalkerController::OnPlayerDeath);
}
