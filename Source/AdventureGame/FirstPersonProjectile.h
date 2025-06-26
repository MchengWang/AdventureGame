// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FirstPersonProjectile.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS(BlueprintType, Blueprintable)
class ADVENTUREGAME_API AFirstPersonProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFirstPersonProjectile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 球体碰撞组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<USphereComponent> CollisionComponent;

	// Projectile movement 组件
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile | Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// 此射弹施加在与其碰撞的物体上的力的大小
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Physics")
	float PhysicsForce = 100.0f;

	// 当射弹与对象碰撞时调用
	UFUNCTION()
	void OnHint(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	// 世界中射弹的网格
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Mesh")
	TObjectPtr<UStaticMeshComponent> ProjectileMesh;

	// 默认在 5 秒后消失
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile | Lifespan")
	float ProjectileLifespan = 5.0f;
};
