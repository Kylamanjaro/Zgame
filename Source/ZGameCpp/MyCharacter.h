// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Logging/LogMacros.h"
#include "MyCharacter.generated.h"

#define UE_BUILD_DEBUG 1

class UAMyCharacterMovement;
class USpringArmComponent;
class UCameraComponent;
class UPostProcessComponent;
class UInputMappingContext;
class UInputAction;

DECLARE_LOG_CATEGORY_EXTERN(LogCharacter, Log, All);

UCLASS(config = Game)

class ZGAMECPP_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, Category = Camera) USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = Camera) UCameraComponent* FollowCamera;
	UPROPERTY(VisibleAnywhere, Category = Camera) UPostProcessComponent* PostProcessComponent;
	UPROPERTY(EditAnywhere, Category = Input) UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* MoveAction;
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* DodgeAction;
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* StrafeAction;
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* PrintAction; // For printing debug stuff to play screen
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* JumpAction;
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* RotateCameraAction; // Experimental, rotates camera behind player


public:
	explicit AMyCharacter(const FObjectInitializer& ObjectInitializer);
	FCollisionQueryParams GetQueryParams() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pDodgeForwardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pDodgeBackwardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pJumpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pJumpDownMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pRunningJumpMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pRunningJumpDownMontage;

	// Enter Strafe Montages NOT USED ATM
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	//UAnimMontage* m_pRunTurnLeftMontage;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	//UAnimMontage* m_pWalkTurnLeftMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock On Camera")
	float LockonControlRotationRate;;
	float CapsuleHalfHeight;


protected:
	FVector2D MovementVector;
	FVector2D OldMovementVector;
	void Move(const FInputActionValue& Value);
	void Dodge();
	
	// Experimental 
	void Print();
	void RotateCamera();
	FRotator CameraAngle = FRotator(0.f, 0.f, 0.f);

	// Set Object Transparent between camera and player
	FVector CameraLocation;
	FVector PlayerLocation;
	float PlayerRadius;
	void SetObjectTranslucent(FVector& Start, FVector& End);
	TArray<AActor*> OldActors;
	TArray<UMaterialInterface*> OldMaterials;
	// Material that all objects betwen Player and Camera are set to, assigned in Player Blueprint
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Materials) UMaterial* TranslucentMaterial;

	// Override ACharacter class jump function to block specific inputs
	void StartJumping();
	void Jump() override;
	bool bIsJumpStartUp;
	bool bIsJumping;
	bool bIsLanding;
	bool bIsRunJump;
	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode) override;

	void StartStrafe();
	void EndStrafe();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(Transient) TObjectPtr<UAMyCharacterMovement> MyCharacterMovement;
	UPROPERTY(Transient) TObjectPtr<APlayerController> MyPlayerController;

public:	
	UFUNCTION()
	void HandleOnMontageNotifyBegin(FName a_nNotifyName, const FBranchingPointNotifyPayload& a_pBranchingPayload);
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};
