//@Author: Teodor Tysklind

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GP3AIBTPatrolPath.generated.h"

class USplineComponent;
class  UGP3AINavMovementComponent;

struct FGP3AIBTPatrolPathMemory
{
	float DistanceAlongPath;
	bool bGoingForward;
	float MovementMagnitude;

	TWeakObjectPtr<UGP3AINavMovementComponent> AINavMovementComponent;
	TWeakObjectPtr<USplineComponent> PathSpline;
};

/**
* Move AI Along spline path
*/
UCLASS()
class GP3_TEAM4_API UGP3AIBTPatrolPath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGP3AIBTPatrolPath();

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	virtual uint16 GetInstanceMemorySize() const override;
	void SetMovementDirection(FGP3AIBTPatrolPathMemory* MyMemory);
};
