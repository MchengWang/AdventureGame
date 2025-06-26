// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "EnhancedInputComponent.h" // �����ǿ�������ģ��
#include "EnhancedInputSubsystems.h" // �����Ա��������ϵͳ�ķ���
#include "InputActionValue.h" // ���ö�����������������������ֵ�ķ���
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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> FirstPersonContext;

	// �ƶ�����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	// ��Ծ
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	// �鿴
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAction;

	// ʹ������ʵ��
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> UseAction;

	// ��ǰװ���Ĺ���
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tools)
	TObjectPtr<AEquippableToolBase> EquippedTool;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ���� 2D �ƶ�����
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// ����鿴����
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	// ��һ�˳����
	UPROPERTY(VisibleAnywhere, Category = Camera)
	UCameraComponent* FirstPersonCameraComponent;

	// ��һ�˳����ƫ����
	UPROPERTY(VisibleAnywhere, Category = Camera)
	FVector FirstPersonCameraOffset = FVector(2.8f, 5.9f, 0.0f);

	// ��һ�˳ƻ�Ԫ��Ұ
	UPROPERTY(EditAnywhere, Category = Camera)
	float FirstPersonFieldOfView = 70.0f;

	//��һ�˳ƻ�������ͼ����
	UPROPERTY(EditAnywhere, Category = Camera)
	float FirstPersonScale = 0.6f;

	// ��һ�˳����񣬽���ӵ�е���ҿɼ�
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	USkeletalMeshComponent* FirstPersonMeshComponent;

	// ��һ�˳ƶ���
	UPROPERTY(EditAnywhere, Category = Animation)
	UAnimBlueprint* FirstPersonDefaultAnim;

	// �嵥���
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	TObjectPtr<UInventoryComponent> InventoryComponent;

	// �����Ƿ�����Ƿ��Ѿ�ӵ�д˹���
	UFUNCTION()
	bool IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition);

	// ����Ҹ��Ӻ�װ���Ϲ���
	UFUNCTION()
	void AttachTool(UEquippableToolDefinition* ToolDefinition);

	// ��������Ե����Գ���������ṩ��Ʒ�Ĺ�������
	UFUNCTION()
	void GiveItem(UItemDefinition* ItenDefinition);
};
