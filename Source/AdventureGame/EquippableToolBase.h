// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "EquippableToolBase.generated.h"

class AAdventureCharacter;
class UInputAction;
class UInputMappingContext;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API AEquippableToolBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AEquippableToolBase();

	// 第一人称动画
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimBlueprint> FirstPersonToolAnim;

	// 第三人称动画
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimBlueprint> ThirdPersonToolAnim;

	// 工具骨骼网格体
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ToolMeshComponent;
	
	// 工具持有者
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AAdventureCharacter> OwningCharacter;

	// 关联此工具的输入映射内容
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> ToolMappingContext;

	// First Person animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UseCooldown;

	// 使用工具
	UFUNCTION()
	virtual void Use();

	// 将 Use 函数绑定到所拥有角色
	UFUNCTION()
	virtual void BindInputAction(const UInputAction* ActionToBind);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
