//Author: Teodor Tysklind

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GP3AIBTListenForNoise.generated.h"

struct FGP3AIListenForNoiseMemory
{
	float ElapsedTime;
};

UCLASS()
class GP3_TEAM4_API UGP3AIBTListenForNoise : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UGP3AIBTListenForNoise();

	UPROPERTY(Category = Node, EditAnywhere, meta=(ClampMin = "0.0", UIMin="0.0"))
	float Duration;

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	virtual uint16 GetInstanceMemorySize() const override;

	const float GracePeriodTime = 1.0f;
};
