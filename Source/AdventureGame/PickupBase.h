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

	// 使用数据表中的值初始化此拾取。
	void InitializePickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 此取件在关联数据表中的 ID。
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	FName PickupItemID;

	// 包含此 pickup 的数据表。
	UPROPERTY(EditInstanceOnly, Category = "Pickup | Item Table")
	TSoftObjectPtr<UDataTable> PickupDataTable;

	// 与此项目关联的数据资产。
	UPROPERTY(VisibleAnywhere, Category = "Pickup | Reference Item")
	TObjectPtr<UItemDefinition> ReferenceItem;

	// 在世界中表示此拾取物的网格组件。
	UPROPERTY(VisibleDefaultsOnly, Category = "Pickup | Mesh")
	TObjectPtr<UStaticMeshComponent> PickupMeshComponent;

	// 球体 （Sphere） 组件，用于定义此拾取物的碰撞半径，用于交互目的。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Components")
	TObjectPtr<USphereComponent> SphereComponent;

	// 当某些内容与 SphereComponent 重叠时的代码。
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// 此拾取物在被拾取后是否应重新生成。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	bool bShouldRespawn;

	// 在重生此拾取物之前等待的时间（以秒为单位）。
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup | Respawn")
	float RespawnTime = 4.0f;

	// 用于区分重生计时器的计时器手柄。
	FTimerHandle RespawnTimerHandle;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};