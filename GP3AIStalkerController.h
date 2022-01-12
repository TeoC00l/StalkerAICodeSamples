//Author: Teodor Tysklind
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/SplineComponent.h"
#include "GP3AIStalkerController.generated.h"

/**
 * 
 */
UCLASS()
class GP3_TEAM4_API AGP3AIStalkerController : public AAIController
{
	GENERATED_BODY()

//CONSTRUCTORS
public:
	AGP3AIStalkerController();
	
//VARIBLES
public:
	int32 CurrentPatrolPoint = 0;

	UPROPERTY()
	TArray<USplineComponent*> PathSplines;
	
	UPROPERTY()
	USplineComponent* CurrentPath;

	UPROPERTY()
	class AGP3AIPawnStalker* AIChar;

private:
	UPROPERTY()
	
	class UBehaviorTreeComponent* BehaviorTreeComp;
	
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;

	//blackboard keys
	UPROPERTY(EditDefaultsOnly, Category= AI)
	FName LocationToGetKey;

	UPROPERTY(EditDefaultsOnly, Category=AI)
	FName PlayerKey;
	
	UPROPERTY(EditDefaultsOnly, Category=AI)
	FName Path;
	
	
//METHODS
public:
	void SetPlayerCaught(APawn* Player) const;
	virtual void OnPossess(APawn *InPawn) override;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE UBlackboardComponent* GetBlackBoardComp() const { return BlackboardComp; }

	UFUNCTION()
    void OnPlayerDeath();
	
private:
	UFUNCTION()
	void SetupBlackBoardKeys();
};
