// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AdventureCharacter.generated.h"

class AEquippableToolBase;
class UAnimBlueprint;
class UEquippableToolDefinition;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class UItemDefinition;
class UInventoryComponent;

UCLASS()
class ADVENTUREGAME_API AAdventureCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventureCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 输入映射文本
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> LookContext;

	// 移动输入行为
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	// 跳跃行为
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	// 查看行为
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	// 工具变更行为
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SwitchToolAction;

	// 使用输入行为
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> UseAction;

	// 第一人称动画
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimBlueprint* FirstPersonDefaultAnim;

	// 当前装备工具
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tools)
	TObjectPtr<AEquippableToolBase> EquippedTool;

public:
	// 逐帧调用
	virtual void Tick(float DeltaTime) override;

	// 调用以将功能绑定到输入
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// 处理 2D 移动
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// 处理查看
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	// 为玩家附加和装备工具
	UFUNCTION()
	void AttachTool(UEquippableToolDefinition* ToolDefinition);

	// 其他类可以调用以尝试向玩家提供物品的公共函数
	UFUNCTION()
	void GiveItem(UItemDefinition* ItemDefinition);

	// 返回玩家是否已经拥有此工具
	UFUNCTION()
	bool IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition);

	// 第一人称相机
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// 第一人称网格，仅对拥有的玩家可见
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	// 库存组件
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	TObjectPtr<UInventoryComponent> InventoryComponent;
};