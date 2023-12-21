// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Actor.h"
#include "MyCharacterMovement.h"
//#include "MyPlayerController.h"


DEFINE_LOG_CATEGORY(LogCharacter);

// Sets default values
AMyCharacter::AMyCharacter(const FObjectInitializer& ObjectInitializer) : Super(
	ObjectInitializer.SetDefaultSubobjectClass<UAMyCharacterMovement>(CharacterMovementComponentName))
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->SetVisibility(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
	LockonControlRotationRate = 1.f;

	constexpr float TargetArmLength = 1000.f;
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(false);
	//CameraBoom->SetWorldRotation(FRotator(0.f, -270.f, 0.f));
	CameraBoom->TargetArmLength = TargetArmLength;
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(RootComponent);
	FPostProcessSettings PostProcessSettings;
	PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	PostProcessSettings.DepthOfFieldFstop = .2f;
	PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
	PostProcessSettings.DepthOfFieldSensorWidth = 100.f;
	PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	PostProcessSettings.DepthOfFieldFocalDistance = TargetArmLength;
	PostProcessComponent->bEnabled = true;
	PostProcessComponent->Settings = PostProcessSettings;
}

FCollisionQueryParams AMyCharacter::GetQueryParams() const
{
	return FCollisionQueryParams();
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	MyCharacterMovement = Cast<UAMyCharacterMovement>(GetCharacterMovement());
	check(MyCharacterMovement);

	if (const auto* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	RotateCamera();
	CameraAngle = FRotator(0.f, 0.f, 0.f);

	// Bind Anim Events
	UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	if (pAnimInst != nullptr)
	{
		pAnimInst->OnPlayMontageNotifyBegin.AddDynamic(this, &AMyCharacter::HandleOnMontageNotifyBegin);
	}
}

void AMyCharacter::HandleOnMontageNotifyBegin(FName a_nNotifyName, const FBranchingPointNotifyPayload& a_pBranchingPayload)
{
	// Check for dodging
	if (a_nNotifyName.ToString() == "Dodge")
	{
		MyCharacterMovement->bWantsToDodge = false;
		if (!MyCharacterMovement->bIsStrafing)
		{
			EndStrafe();
		}
		//EnableInput(MyPlayerController);
	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Vector from player to target
	//CameraBoom->GetComponentRotation();
	//FVector TargetVect = CameraBoom->CameraTarget->GetComponentLocation() - CameraLockArm->GetComponentLocation();
	//FRotator TargetRot = TargetVect.GetSafeNormal().Rotation();
	//FRotator CurrentRot = GetControlRotation();
	//FRotator NewRot = FMath::RInterpTo(GetControlRotation(), CameraBoom->GetComponentRotation(), DeltaTime, LockonControlRotationRate);

	// Update control rotation to face target
	//GetController()->SetControlRotation(NewRot);
	//CameraBoom->SetWorldRotation(NewRot);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AMyCharacter::Dodge);
		EnhancedInputComponent->BindAction(StrafeAction, ETriggerEvent::Triggered, this, &AMyCharacter::StartStrafe);
		EnhancedInputComponent->BindAction(StrafeAction, ETriggerEvent::Completed, this, &AMyCharacter::EndStrafe);
		EnhancedInputComponent->BindAction(PrintAction, ETriggerEvent::Triggered, this, &AMyCharacter::Print);
		EnhancedInputComponent->BindAction(RotateCameraAction, ETriggerEvent::Triggered, this, &AMyCharacter::RotateCamera);
	}
	else
	{
		UE_LOG(LogCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}

}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation().Add(0.f, CameraAngle.Yaw, 0.f);
		//const FRotator Rotation = GetActorForwardVector().GetSafeNormal().Rotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		//MovementVector = MovementVector.GetSafeNormal().GetRotated(CameraAngle.Yaw);
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);

		//CameraBoom->SetWorldRotation(Rotation);
		// add yaw and pitch input to controller
		//FVector LookAxisVector = GetActorForwardVector();

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Rotation"));
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, Rotation.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("CameraAngle"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, CameraAngle.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MovementVector"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, MovementVector.ToString());
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Character"));
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, GetCapsuleComponent()->GetForwardVector().ToString());
		//AddControllerYawInput(MovementVector.Y);
		//FRotator NewRot = FMath::RInterpTo(GetControlRotation(), CameraBoom->GetComponentRotation(), DeltaTime, LockonControlRotationRate);
		//CameraBoom->SetWorldRotation(Controller->GetControlRotation());
		
	}
}

void AMyCharacter::StartStrafe()
{
	MyCharacterMovement->MaxWalkSpeed = 450;
	MyCharacterMovement->bInvertOrientRotationToMovement = true;
	MyCharacterMovement->bIsStrafing = true;
	// Check for valid animation instance
	//UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
	//if (pAnimInst != nullptr)
	//{
	//	if (MyCharacterMovement->GetSpeed() > 125)
	//	{
	//		pAnimInst->Montage_Play(m_pRunTurnLeftMontage);
	//	}
	//	else
	//	{
	//		pAnimInst->Montage_Play(m_pWalkTurnLeftMontage);
	//	}
	//}

}

void AMyCharacter::EndStrafe()
{
	MyCharacterMovement->bIsStrafing = false;
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Strafe Ended!"));
	if (!MyCharacterMovement->bWantsToDodge)
	{
		MyCharacterMovement->MaxWalkSpeed = 600;
		MyCharacterMovement->bInvertOrientRotationToMovement = false;
	}
}

void AMyCharacter::Print()
{

	//FRotator CurrentRot = GetControlRotation();
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FollowCamera->GetForwardVector().ToString());
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, CameraBoom->GetForwardVector().ToString());
	CameraBoom->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, CurrentRot.ToString());
}

void AMyCharacter::RotateCamera()
{
	CameraAngle = GetActorForwardVector().GetSafeNormal().Rotation();
	CameraBoom->SetRelativeRotation(FRotator(0.f, CameraAngle.Yaw, 0.f));
	CameraAngle = CameraAngle.Add(0, -180, 0);
}

void AMyCharacter::Dodge()
{
	//Check for player ability to dodge (Not attacking, ...)
	if (!MyCharacterMovement->bIsDodging)
	{
		MyCharacterMovement->bWantsToDodge = true;
		// Check for valid animation instance
		UAnimInstance* pAnimInst = GetMesh()->GetAnimInstance();
		if (pAnimInst != nullptr)
		{
			if (UE_BUILD_DEBUG)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Dodge Montage!"));
			}
			if (MyCharacterMovement->bIsStrafing)
			{
				pAnimInst->Montage_Play(m_pDodgeBackwardMontage);
			}
			else
			{
				pAnimInst->Montage_Play(m_pDodgeForwardMontage);
			}

			//LaunchCharacter(GetActorForwardVector() * dodgeScalar, true, true);
		}
		//DisableInput(MyPlayerController);
	}
}


