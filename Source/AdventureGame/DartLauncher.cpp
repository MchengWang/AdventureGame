// Fill out your copyright notice in the Description page of Project Settings.


#include "DartLauncher.h"
#include "AdventureGame/AdventureCharacter.h"

void ADartLauncher::Use()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Using the dart launcher!"));
}

void ADartLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADartLauncher::BindInputAction(const UInputAction* InputToBind)
{
	// 设置绑定行为
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController))
		{
			// 开火
			EnhancedInputComponent->BindAction(InputToBind, ETriggerEvent::Triggered, this, &ADartLauncher::Use);
		}
	}
}