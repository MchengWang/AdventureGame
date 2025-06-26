// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/SphereComponent.h"
#include "CoreMinimal.h"
#include "AdventureCharacter.h"
#include "GameFramework/Actor.h"
#include "PickupBase.generated.h"

class UItemDefinition;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API APickupBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties.
	APickupBase();

	// ʹ�����ݱ��е�ֵ��ʼ����ʰȡ��
	void InitializePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// ��ȡ���ڹ������ݱ��е� ID��
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	FName PickupItemID;

	// ������ pickup �����ݱ�
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	TSoftObjectPtr<UDataTable> PickupDataTable;

	// �����Ŀ�����������ʲ���
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Reference Item")
	TObjectPtr<UItemDefinition> ReferenceItem;

	// �������б�ʾ��ʰȡ������������
	UPROPERTY(VisibleDefaultsOnly, Category = "Pickup | Mesh")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent;

	// ���� ��Sphere�� ��������ڶ����ʰȡ�����ײ�뾶�����ڽ���Ŀ�ġ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Components")
	TObjectPtr<USphereComponent> SphereComponent;

	// ��ĳЩ������ SphereComponent �ص�ʱ�Ĵ��롣
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// ��ʰȡ���ڱ�ʰȡ���Ƿ�Ӧ�������ɡ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	bool bShouldRespawn;

	// ��������ʰȡ��֮ǰ�ȴ���ʱ�䣨����Ϊ��λ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	float RespawnTime = 4.0f;

	// ��������������ʱ���ļ�ʱ���ֱ���
	FTimerHandle RespawnTimerHandle;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};