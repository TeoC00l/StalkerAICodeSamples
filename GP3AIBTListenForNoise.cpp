//Author: Teodor Tysklind

#include "GP3AIBTListenForNoise.h"
#include "BehaviorTree/BlackboardComponent.h"

UGP3AIBTListenForNoise::UGP3AIBTListenForNoise()
{
	bNotifyTick = true;
}

EBTNodeResult::Type UGP3AIBTListenForNoise::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FGP3AIListenForNoiseMemory* MyMemory = CastInstanceNodeMemory<FGP3AIListenForNoiseMemory>(NodeMemory);

	MyMemory->ElapsedTime = 0.f;
	return EBTNodeResult::InProgress;
}

void UGP3AIBTListenForNoise::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	FGP3AIListenForNoiseMemory* MyMemory = CastInstanceNodeMemory<FGP3AIListenForNoiseMemory>(NodeMemory);

	MyMemory->ElapsedTime+=DeltaSeconds;

	if(MyMemory->ElapsedTime >= Duration)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

	const float TimeSinceNoise = OwnerComp.GetBlackboardComponent()->GetValueAsFloat("TimeSinceHeardNoise");

	if(TimeSinceNoise < GracePeriodTime)
	{
		OwnerComp.GetBlackboardComponent()->SetValueAsBool("bChasePlayer", true);
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

uint16 UGP3AIBTListenForNoise::GetInstanceMemorySize() const
{
	 return sizeof(FGP3AIListenForNoiseMemory);
}
