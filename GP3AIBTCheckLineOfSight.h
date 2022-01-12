// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GP3AIBTCheckLineOfSight.generated.h"

/**
 * 
 */
UCLASS()
class GP3_TEAM4_API UGP3AIBTCheckLineOfSight : public UBTService
{
	GENERATED_BODY()
	
	UGP3AIBTCheckLineOfSight();
	
	
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
