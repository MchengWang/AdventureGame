// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ItemDefinition.h"
#include "EquippableToolDefinition.generated.h"

class AEquippableToolBase;
class UInputMappingContext;

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API UEquippableToolDefinition : public UItemDefinition
{
	GENERATED_BODY()

public:
	// 关联此条目的 Actor 工具
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEquippableToolBase> ToolAsset;

	// 创建并返回此条目的副本
	UFUNCTION()
	virtual UEquippableToolDefinition* CreateItemCopy() const override;
};
