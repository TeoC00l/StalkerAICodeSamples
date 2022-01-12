//Author: Teodor Tysklind

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GP3AIBTPerformSearch.generated.h"

class AGP3AIPawnStalker;

struct FGP3AIBTPerformSearchMemory
{
	float RemainingSearchTime;
	int CurrentSearchState;
	float LerpTimeElapsed;
	bool bExtendingForward;
};

UCLASS()
class GP3_TEAM4_API UGP3AIBTPerformSearch : public UBTTaskNode
{
	GENERATED_BODY()

//CONSTRUCTORS
	UGP3AIBTPerformSearch();
	
//VARIABLES
public:
	UPROPERTY(Category = Search, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SearchDistance;
	
	UPROPERTY(Category = Search, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SearchTimePerState;
	
	UPROPERTY(Category = Search, EditAnywhere, meta = (ClampMin = "0.0", UIMin = "0.0"))
	float SearchPadding;

//METHODS
protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	virtual uint16 GetInstanceMemorySize() const override;
};
