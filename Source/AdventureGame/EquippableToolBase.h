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

	// ��һ�˳ƶ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimBlueprint> FirstPersonToolAnim;

	// �����˳ƶ���
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimBlueprint> ThirdPersonToolAnim;

	// ���߹���������
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> ToolMeshComponent;
	
	// ���߳�����
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<AAdventureCharacter> OwningCharacter;

	// �����˹��ߵ�����ӳ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> ToolMappingContext;

	// First Person animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float UseCooldown;

	// ʹ�ù���
	UFUNCTION()
	virtual void Use();

	// �� Use �����󶨵���ӵ�н�ɫ
	UFUNCTION()
	virtual void BindInputAction(const UInputAction* ActionToBind);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
