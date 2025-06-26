// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdventureGame/EquippableToolBase.h"
#include "DartLauncher.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API ADartLauncher : public AEquippableToolBase
{
	GENERATED_BODY()
	
public:
	virtual void Use() override;

	// ÿһ֡����
	virtual void Tick(float DeltaTime) override;

	virtual void BindInputAction(const UInputAction* InputToBind) override;
};
