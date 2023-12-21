#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MyCharacterMovement.generated.h"

class AMyCharacter;

#define UE_BUILD_DEBUG 1

DECLARE_LOG_CATEGORY_EXTERN(LogCharacterMovement, Log, All);

UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None UMETA(DisplayName = "None"),
	CMOVE_Dodge UMETA(DisplayName = "Dodge"),
	CMOVE_MAX UMETA(Hidden)
};

UCLASS(ClassGroup=(Custom), meta =(BlueprintSpawnableComponent))

class ZGAMECPP_API UAMyCharacterMovement : public UCharacterMovementComponent
{
	GENERATED_BODY()

public:
	UAMyCharacterMovement();

	// Blueprint Animation
	UFUNCTION(BlueprintPure) float GetSpeed() const;
	UFUNCTION(BlueprintPure) bool IsWalk() const;
	UFUNCTION(BlueprintPure) bool IsDodging() const{ return IsCustomMovementMode(CMOVE_Dodge); }
	UFUNCTION(BlueprintPure) bool IsStrafe() const;

	// Dodge
	bool bWantsToDodge;
	bool bIsDodging;
	UPROPERTY(EditDefaultsOnly, Category = "Dodge") float MinDodgeSpeed = 350.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dodge") float EnterDodgeImpulse = 500.f;
	UPROPERTY(EditDefaultsOnly, Category = "Dodge") float DodgeFriction = 1.3f;
	UPROPERTY(EditDefaultsOnly, Category = "Dodge") float MaxDodgingSeconds = 2.0f;
	UPROPERTY(EditDefaultsOnly, Category = "Dodge") float DodgeHalfHeight = 40.f;

	// Strafe
	UPROPERTY(Category = "Character Movement (Rotation Settings)", EditAnywhere, BlueprintReadWrite)
	uint8 bInvertOrientRotationToMovement : 1;
	bool bIsStrafing;
	
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

protected:
	virtual void InitializeComponent() override;
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;
	FRotator ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const override;

private:
	UPROPERTY(Transient) TObjectPtr<AMyCharacter> MyCharacterOwner;

	// Dodge
	void Dodge();
	void ExitDodge();
	void PhysDodge(float DeltaTime, int32 Iterations);
	bool CanDodge(FFindFloorResult& FloorHit) const;
	float DodgeTime = 0.f;

	// Helper Functions
	bool IsCustomMovementMode(const ECustomMovementMode InCustomMovementMode) const;
	bool CannotPerformPhysMovement() const;
	bool CanPerformFrameTickMovement(const float RemainingTime, const int32 Iterations) const;
	float GetCharacterCapsuleRadius() const;
	void SetCollisionSizeToDodgeDimensions();
	bool RestoreDefaultsCollisionDimensions();

};
