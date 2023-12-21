// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"
#include "MyCharacter.h"
#include "MyCharacterMovement.h"
#include "UObject/ConstructorHelpers.h"


AMyGameMode::AMyGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/Character/Niki"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
