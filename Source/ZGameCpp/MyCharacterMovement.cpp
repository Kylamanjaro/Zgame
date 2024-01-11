#include "MyCharacterMovement.h"
#include "MyCharacter.h"
#include "Components/CapsuleComponent.h"

DEFINE_LOG_CATEGORY(LogCharacterMovement);

UAMyCharacterMovement::UAMyCharacterMovement()
{
	RotationRate = FRotator(0.f, 3072.f, 0.f);
	AirControl = 0.5f;
	Mass = 500.f;
	JumpZVelocity = 1450.f;
	MaxWalkSpeed = 600.f;
	GravityScale = 3.f;
	MaxAcceleration = 1200.f;
	bConstrainToPlane = false;
	bOrientRotationToMovement = true;
	bInvertOrientRotationToMovement = false;
}

void UAMyCharacterMovement::InitializeComponent()
{
	Super::InitializeComponent();
	MyCharacterOwner = Cast<AMyCharacter>(GetOwner());
	check(MyCharacterOwner);
}


float UAMyCharacterMovement::GetSpeed() const
{
	return Velocity.Length();
}

bool UAMyCharacterMovement::IsWalk() const
{
	if (!IsWalking() || FMath::IsNearlyZero(GetSpeed()))
	{
		return false;
	}

	return true;
}

bool UAMyCharacterMovement::IsStrafe() const
{
	return bIsStrafing;
}

void UAMyCharacterMovement::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	// Slide
	FFindFloorResult FloorHit;
	if (IsMovingOnGround() && bWantsToDodge && CanDodge(FloorHit))
	{
		Dodge();
	}

	if ( IsDodging() && !bWantsToDodge)
	{
		ExitDodge();
	}

	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);
}

void UAMyCharacterMovement::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);

	switch (CustomMovementMode)
	{
		case CMOVE_Dodge:
			PhysDodge(deltaTime, Iterations);
			break;
		default:
			UE_LOG(LogCharacterMovement, Error, TEXT("Invalid Movement Mode"));
	}
}

#pragma region "Dodge"

bool UAMyCharacterMovement::CanDodge(FFindFloorResult& FloorHit) const
{
	//if (Velocity.SizeSquared() <= pow(MinDodgeSpeed, 2))
	//{
	//	return false;
	//}

	if (!bWantsToDodge)
	{
		return false;
	}

	FindFloor(UpdatedComponent->GetComponentLocation(), FloorHit, false);
	return FloorHit.bWalkableFloor;
}

void UAMyCharacterMovement::Dodge()
{
	bIsDodging = true;
	DodgeTime = 0.f;
	
	if (Acceleration.Length() < UE_KINDA_SMALL_NUMBER)
	{
		Velocity = MyCharacterOwner->GetActorForwardVector() * EnterDodgeImpulse;
	}
	else
	{
		Velocity = Acceleration.GetSafeNormal() * EnterDodgeImpulse;
	}

		
	if (UE_BUILD_DEBUG)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, MyCharacterOwner->GetControlRotation().ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Velocity.ToString());
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Dodge Movement!"));
	}
	//SetCollisionSizeToDodgeDimensions();
	SetMovementMode(MOVE_Custom, CMOVE_Dodge);
}

void UAMyCharacterMovement::ExitDodge()
{
	//if (!RestoreDefaultsCollisionDimensions())
	//{
	//	return;
	//}

	// restore character rotation
	//const FQuat NewRotation = FRotationMatrix::MakeFromXZ(
	//	UpdatedComponent->GetForwardVector().GetSafeNormal2D(),
	//	FVector::UpVector
	//	).ToQuat();
	//MoveUpdatedComponent(FVector::ZeroVector, NewRotation, true);

	bWantsToDodge = false;
	bIsDodging = false;
	SetMovementMode(MOVE_Walking);
}

void UAMyCharacterMovement::PhysDodge(float DeltaTime, int32 Iterations)
{
	if (DeltaTime < MIN_TICK_TIME)
	{
		return;
	}

	RestorePreAdditiveRootMotionVelocity();
	FFindFloorResult FloorHit;
	if (!CanDodge(FloorHit))
	{
		ExitDodge();
		StartNewPhysics(DeltaTime, Iterations);
	}
	
	// strafe
	if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5f)
	{
		Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
	}
	else
	{
		Acceleration = FVector::ZeroVector;
	}

	// calc velocity
	if (!HasAnimRootMotion() && CurrentRootMotion.HasOverrideVelocity())
	{
		CalcVelocity(DeltaTime, DodgeFriction, true, GetMaxBrakingDeceleration());
	}

	ApplyRootMotionToVelocity(DeltaTime);

	Iterations++;
	bJustTeleported = false;
	FVector OldLocation = UpdatedComponent->GetComponentLocation();
	FHitResult HitResult(1.f);
	FVector Adjusted = Velocity * DeltaTime;
	FVector VelPlaneDir = FVector::VectorPlaneProject(Velocity, FloorHit.HitResult.Normal).GetSafeNormal();
	FQuat NewRotation = FQuat::Identity;
	if (bInvertOrientRotationToMovement)
	{
		NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir*FVector(-1,-1,1), FloorHit.HitResult.Normal).ToQuat();
	}
	else
	{
		NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, FloorHit.HitResult.Normal).ToQuat();
	}
	//FQuat NewRotation = FRotationMatrix::MakeFromXZ(VelPlaneDir, FloorHit.HitResult.Normal).ToQuat();

	// perform move
	//MoveUpdatedComponent(Adjusted, NewRotation, true, &HitResult);
	MoveUpdatedComponent(Adjusted, NewRotation, true, &HitResult);
	if (HitResult.Time < 1.f)
	{
		HandleImpact(HitResult, DeltaTime, Adjusted);
		SlideAlongSurface(Adjusted, 1.f - HitResult.Time, HitResult.Normal, HitResult, true);
	}

	// check if dodge conditions are met
	if (FFindFloorResult NewFloorHit; !CanDodge(NewFloorHit))
	{
		ExitDodge();
	}

	// exit dodge if max time (or animation montage is complete)
	if (MaxDodgingSeconds > 0.f)
	{
		DodgeTime += DeltaTime;
		if (DodgeTime >= MaxDodgingSeconds)
		{
			if (UE_BUILD_DEBUG)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Dodge Timout!"));
			}
			ExitDodge();
		}
	}

	// update outgoing velocity && acceleration
	if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
	{
		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
	}
	UpdateComponentVelocity();

}

#pragma endregion


#pragma region "Helpers"

bool UAMyCharacterMovement::IsCustomMovementMode(const ECustomMovementMode InCustomMovementMode) const
{
	return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

bool UAMyCharacterMovement::CannotPerformPhysMovement() const
{
	return !CharacterOwner || !CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy);
}

bool UAMyCharacterMovement::CanPerformFrameTickMovement(const float RemainingTime, const int32 Iterations) const
{
	return (RemainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy));
}

float UAMyCharacterMovement::GetCharacterCapsuleRadius() const
{
	return CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();
}

void UAMyCharacterMovement::SetCollisionSizeToDodgeDimensions()
{
	// Change collision size to dodge dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	// height is not allowed to be smaller than radius
	const float ClampedCrouchedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, DodgeHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedCrouchedHalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedCrouchedHalfHeight);

	if (bCrouchMaintainsBaseLocation)
	{
		UpdatedComponent->MoveComponent(
			FVector(0.f, 0.f, -(HalfHeightAdjust * ComponentScale)),
			UpdatedComponent->GetComponentQuat(),
			true,
			nullptr,
			MOVECOMP_NoFlags,
			ETeleportType::TeleportPhysics
		);
	}

	bForceNextFloorCheck = true;

	// OnStartDodge takes the change from the Default size, not the current one (though they are usually the same)
	const ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - ClampedCrouchedHalfHeight;

	AdjustProxyCapsuleSize();
	CharacterOwner->OnStartCrouch(HalfHeightAdjust, HalfHeightAdjust * ComponentScale);
}

bool UAMyCharacterMovement::RestoreDefaultsCollisionDimensions()
{
	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float HalfHeightAdjust = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - OldUnscaledHalfHeight;
	const float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Try to stay in place to see if larger capsule fits
	// we use a slightly taller capsule to avoid penetration
	const UWorld* MyWorld = GetWorld();
	constexpr float SweepInflation = UE_KINDA_SMALL_NUMBER * 10.f;
	FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(CrouchTrace), false, CharacterOwner);
	FCollisionResponseParams ResponseParam;
	InitCollisionParams(CapsuleParams, ResponseParam);

	// Compensate for the difference between current capsule size and standing size
	const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(
		SHRINK_HeightCustom,
		-SweepInflation - ScaledHalfHeightAdjust
	);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
	bool bEncroached = true;


	if (!bCrouchMaintainsBaseLocation)
	{
		// Expand in place
		bEncroached = MyWorld->OverlapBlockingTestByChannel(
			PawnLocation,
			FQuat::Identity,
			CollisionChannel,
			StandingCapsuleShape,
			CapsuleParams,
			ResponseParam
		);

		if (bEncroached)
		{
			// Try adjusting capsule position to see if we can avoid encroachment.
			if (ScaledHalfHeightAdjust > 0.f)
			{
				// Shrink to a short capsule, sweep down to base to find where that would hit something, and then try to stand up from there.
				float PawnRadius, PawnHalfHeight;
				CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);
				const float ShrinkHalfHeight = PawnHalfHeight - PawnRadius;
				const float TraceDist = PawnHalfHeight - ShrinkHalfHeight;
				const FVector Down = FVector(0.f, 0.f, -TraceDist);

				FHitResult Hit(1.f);
				const FCollisionShape ShortCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, ShrinkHalfHeight);
				MyWorld->SweepSingleByChannel(Hit, PawnLocation, PawnLocation + Down, FQuat::Identity, CollisionChannel, ShortCapsuleShape, CapsuleParams);
				if (Hit.bStartPenetrating)
				{
					bEncroached = true;
				}
				else
				{
					// Compute where the base of the sweep ended up, and see if we can stand there
					const float DistanceToBase = (Hit.Time * TraceDist) + ShortCapsuleShape.Capsule.HalfHeight;
					const FVector NewLoc = FVector(PawnLocation.X, PawnLocation.Y, PawnLocation.Z - DistanceToBase + StandingCapsuleShape.Capsule.HalfHeight + SweepInflation + MIN_FLOOR_DIST / 2.f);
					bEncroached = MyWorld->OverlapBlockingTestByChannel(NewLoc, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					if (!bEncroached)
					{
						// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
						UpdatedComponent->MoveComponent(NewLoc - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
					}
				}
			}
		}
	}
	// if still encroached then abort
	if (bEncroached)
	{
		return false;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(), true);
	AdjustProxyCapsuleSize();
	CharacterOwner->OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	return true;
}

#pragma endregion

#pragma region "Strafe"

FRotator UAMyCharacterMovement::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const
{
	if (Acceleration.SizeSquared() < UE_KINDA_SMALL_NUMBER)
	{
		// AI path following request can orient us in that direction (it's effectively an acceleration)
		if (bHasRequestedVelocity && RequestedVelocity.SizeSquared() > UE_KINDA_SMALL_NUMBER)
		{
			return RequestedVelocity.GetSafeNormal().Rotation();
		}

		// Don't change rotation if there is no acceleration.
		return CurrentRotation;
	}

	// Rotate toward direction of acceleration.
	if (bInvertOrientRotationToMovement)
	{
		return Acceleration.GetSafeNormal().Rotation().Add(0, 180, 0);
	}
	return Acceleration.GetSafeNormal().Rotation();

}

#pragma endregion