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

	// ����ӳ���ı�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> LookContext;

	// �ƶ�������Ϊ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	// ��Ծ��Ϊ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	// �鿴��Ϊ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	// ���߱����Ϊ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SwitchToolAction;

	// ʹ��������Ϊ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> UseAction;

	// ��һ�˳ƶ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimBlueprint* FirstPersonDefaultAnim;

	// ��ǰװ������
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tools)
	TObjectPtr<AEquippableToolBase> EquippedTool;

public:
	// ��֡����
	virtual void Tick(float DeltaTime) override;

	// �����Խ����ܰ󶨵�����
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���� 2D �ƶ�
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// ����鿴
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	// Ϊ��Ҹ��Ӻ�װ������
	UFUNCTION()
	void AttachTool(UEquippableToolDefinition* ToolDefinition);

	// ��������Ե����Գ���������ṩ��Ʒ�Ĺ�������
	UFUNCTION()
	void GiveItem(UItemDefinition* ItemDefinition);

	// ��������Ƿ��Ѿ�ӵ�д˹���
	UFUNCTION()
	bool IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition);

	// ��һ�˳����
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// ��һ�˳����񣬽���ӵ�е���ҿɼ�
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	// ������
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	TObjectPtr<UInventoryComponent> InventoryComponent;
};