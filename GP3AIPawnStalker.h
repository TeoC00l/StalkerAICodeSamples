
#pragma once

#include "CoreMinimal.h"
#include "GP3AIBTChangeColor.h"
#include "GP3AIPawn.h"
#include "GP3AIPawnStalker.generated.h"

class USplineComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnTentacles, float, Height);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FChangeColor, AIColor, Color);

UCLASS()
class GP3_TEAM4_API AGP3AIPawnStalker : public AGP3AIPawn
{
	GENERATED_BODY()

//CONSTRUCTORS
	AGP3AIPawnStalker();

//VARIABLES
public:
	UPROPERTY(EditAnywhere, Category = Collision)
	class UCapsuleComponent* CapsuleComp;
	
	UPROPERTY(EditInstanceOnly)
	TArray<AActor*> PathActors;
	
	UPROPERTY(Transient)
	TArray<USplineComponent*> PathSplines;

	UPROPERTY(BlueprintAssignable)
	FSpawnTentacles TentacleEvent;

	UPROPERTY(BlueprintAssignable)
	FChangeColor ChangeColorEvent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float TentacleLength;

	enum AIColor Color; 

private:
	UPROPERTY(Category=Character, EditDefaultsOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;
	
//FUNCTIONS
public:
	UFUNCTION()
    void OnSeePawn(APawn* Pawn);

	UFUNCTION()
    void OnHearNoise(APawn* Pawn, const FVector &Location, float Volume);

	UFUNCTION( )
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, 
                  AActor* OtherActor, 
                  UPrimitiveComponent* OtherComp, 
                  int32 OtherBodyIndex, 
                  bool bFromSweep, 
                  const FHitResult& SweepResult);

	UFUNCTION()
	void StartSearch(float Height);

	UFUNCTION(BlueprintCallable)
	float GetTentacleLength();

	

protected:
	virtual void BeginPlay() override;
};
