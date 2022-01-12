
#include "GP3AIPawnStalker.h"
#include "GP3AIStalkerController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "Perception/PawnSensingComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SplineComponent.h"
#include "GP3AIPatrolSearchPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GP3_Team4/PlayerCharacter/GP3PawnPlayer.h"
#include "GP3AIBTChangeColor.h"


AGP3AIPawnStalker::AGP3AIPawnStalker()
{
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComp->SetCapsuleSize(40.f, 120.f);
	RootComponent = CapsuleComp;
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	PawnSensingComp->SensingInterval = .25f;
	PawnSensingComp->SetPeripheralVisionAngle(80.f);
	PawnSensingComp->bHearNoises = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

void AGP3AIPawnStalker::BeginPlay()
{
	Super::BeginPlay();

	CapsuleComp->SetCollisionProfileName(TEXT("Trigger"));
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AGP3AIPawnStalker::OnOverlapBegin);
	
	if(PawnSensingComp)
	{
		PawnSensingComp->OnSeePawn.AddDynamic(this, &AGP3AIPawnStalker::OnSeePawn);
		PawnSensingComp->OnHearNoise.AddDynamic(this, &AGP3AIPawnStalker::OnHearNoise);
	}
}

void AGP3AIPawnStalker::OnSeePawn(APawn* Pawn)
{
	// AGP3AIStalkerController* AIController = Cast<AGP3AIStalkerController>(GetController());
	//
	// if(AIController)
	// {
	// 	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("PURSUING PLAYER!!"));
	// 	AIController->SetPlayerCaught(Pawn);
	// }
}

void AGP3AIPawnStalker::OnHearNoise(APawn* Pawn, const FVector& Location, float Volume)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI heard you!"));
	
	AGP3AIStalkerController* AICon = Cast<AGP3AIStalkerController>(GetController());

	if(!AICon)
	{
		return;
	}
	UBlackboardComponent* BBComp = AICon->GetBlackBoardComp();

	if(!BBComp)
	{
		return;
	}

	BBComp->SetValueAsFloat("TimeSinceHeardNoise", 0.f);
	BBComp->SetValueAsVector("LastHeardPlayerPosition", Location);

	//BBComp->SetValueAsBool("bChasePlayer", true);
}

void AGP3AIPawnStalker::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AGP3AIStalkerController* AICon = Cast<AGP3AIStalkerController>(GetController());

	if(AICon)
	{
		AGP3AIPatrolSearchPoint* SearchPoint = Cast<AGP3AIPatrolSearchPoint>(OtherActor);
		UBlackboardComponent* BBComp = AICon->GetBlackBoardComp();

		if(SearchPoint)
		{
			if(BBComp->GetValueAsBool("bChasePlayer") == false)
			{
				if(BBComp->GetValueAsFloat("TimeSinceSearched") > 11.5f)
				{
					BBComp->SetValueAsFloat("TimeSinceSearched", 0.f);
					BBComp->SetValueAsBool("bSearch", true);
				}
			}

			return;
		}

		AGP3PawnPlayer* Player = Cast<AGP3PawnPlayer>(OtherActor);

		if(Player)
		{
			BBComp->SetValueAsBool("bChasePlayer", true);
		}
	}
}

void AGP3AIPawnStalker::StartSearch(float Height)
{
	// make switch here
	TentacleEvent.Broadcast(Height);
}

float AGP3AIPawnStalker::GetTentacleLength()
{
	return TentacleLength;
}
