
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GP3AINavMovementComponent.generated.h"

UCLASS()
class GP3_TEAM4_API UGP3AINavMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

//VARIABLES:
public:
	UPROPERTY(EditAnywhere, Category= Movement)
	FVector RequestedVelocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, Category= Movement)
	float MovementSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category= Movement)
	float PatrolSpeed = 200.f;

	bool bHasRequestedVelocity = true;
	
//METHODS
public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed) override;
	// virtual bool CanStartPathFollowing() const override;
	// virtual void RequestPathMove(const FVector& MoveInput) override;
	// virtual bool CanStopPathFollowing() const override;
    virtual void StopActiveMovement() override;
    //virtual float GetPathFollowingBrakingDistance(float MaxSpeed) const override;

	const INavigationDataInterface* GetNavData() const;
	bool FindNavFloor(const FVector& TestLocation, FNavLocation& NavFloorLocation) const;
	
};
