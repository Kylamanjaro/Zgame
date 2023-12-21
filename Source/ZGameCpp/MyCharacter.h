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
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* PrintAction;
	UPROPERTY(EditAnywhere, Category = Input) UInputAction* RotateCameraAction;


public:
	explicit AMyCharacter(const FObjectInitializer& ObjectInitializer);
	FCollisionQueryParams GetQueryParams() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pDodgeForwardMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pDodgeBackwardMontage;

	// Enter Strafe Montages
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pRunTurnLeftMontage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation");
	UAnimMontage* m_pWalkTurnLeftMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lock On Camera")
	float LockonControlRotationRate;;


protected:
	void Move(const FInputActionValue& Value);
	void Dodge();
	void Print();
	void RotateCamera();
	FRotator CameraAngle = FRotator(0.f, 0.f, 0.f);

	void StartStrafe();
	void EndStrafe();
	bool bIsStrafing = false;

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
