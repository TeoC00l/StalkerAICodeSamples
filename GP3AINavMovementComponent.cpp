//Author: Teodor Tysklind

#include "GP3AINavMovementComponent.h"
#include "Engine/World.h"
#include "NavigationSystem.h"

void UGP3AINavMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(!bHasRequestedVelocity)
	{
		return;
	}

	//MOVE ACTOR
	GetOwner()->AddActorWorldOffset(RequestedVelocity * DeltaTime);

	//SNAP TO FLOOR
	FNavLocation NavFloorLocation;

	if(FindNavFloor(GetOwner()->GetActorLocation(), NavFloorLocation))
	{
		const float FloorHeight = NavFloorLocation.Location.Z;
		const FVector CurrentLocation = GetOwner()->GetActorLocation();
		GetOwner()->SetActorLocation(FVector(CurrentLocation.X, CurrentLocation.Y, FloorHeight + GetPawnOwner()->GetDefaultHalfHeight()));
	}
}

void UGP3AINavMovementComponent::RequestDirectMove(const FVector& MoveVelocity, bool bForceMaxSpeed)
{
	RequestedVelocity = MoveVelocity.GetSafeNormal() * MovementSpeed;
}

void UGP3AINavMovementComponent::StopActiveMovement()
{
	Super::StopActiveMovement();

	RequestedVelocity = FVector::ZeroVector;
	bHasRequestedVelocity = false;
}

const INavigationDataInterface* UGP3AINavMovementComponent::GetNavData() const
{
	const UWorld* World = GetWorld();

	if(World == nullptr || World->GetNavigationSystem() == nullptr)
	{
		return nullptr;
	}

	const INavigationDataInterface* NavData = FNavigationSystem::GetNavDataForActor(*GetOwner());

	return NavData;
}

bool UGP3AINavMovementComponent::FindNavFloor(const FVector& TestLocation, FNavLocation& NavFloorLocation) const
{
	const INavigationDataInterface* NavData = GetNavData();
	if (NavData == nullptr)
	{
		return false;
	}

	const FNavAgentProperties& AgentProps = GetPawnOwner()->GetNavAgentPropertiesRef();
	const float SearchRadius = AgentProps.AgentRadius * 2.0f;
	const float SearchHeight = AgentProps.AgentHeight * AgentProps.NavWalkingSearchHeightScale;

	return NavData->ProjectPoint(TestLocation - FVector(0.0f, 0.0f, GetPawnOwner()->GetDefaultHalfHeight()), NavFloorLocation, FVector(SearchRadius, SearchRadius, SearchHeight));
}

