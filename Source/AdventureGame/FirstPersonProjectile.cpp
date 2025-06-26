// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
AFirstPersonProjectile::AFirstPersonProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 使用球体作为简单的碰撞表示
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	check(CollisionComponent != nullptr);

	// 创建此射弹的网格组件
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectilemMesh"));
	check(ProjectileMesh != nullptr);

	// 使用 ProjectileMovementComponent 来控制此射弹的运动
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	check(ProjectileMovement != nullptr);

	// 将 sphere 组件附加到 mesh 组件
	ProjectileMesh->SetupAttachment(CollisionComponent);

	CollisionComponent->InitSphereRadius(5.0f);

	// 将碰撞配置文件设置为 “Projectile” 碰撞预设
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");

	// 设置此组件遇到阻塞内容时的通知
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFirstPersonProjectile::OnHint);

	// 设置根组件
	RootComponent = CollisionComponent;

	ProjectileMovement->UpdatedComponent = CollisionComponent;

	// InitialSpeed：发射物生成时的初始速度。 将此值设置为。3000.0f
	ProjectileMovement->InitialSpeed = 3000.0f;
	// MaxSpeed：发射物的最大速度。 将此值设置为。3000.0f
	ProjectileMovement->MaxSpeed = 3000.0f;
	// bRotationFollowVelocity：对象是否应旋转以匹配速度的方向。 例如，纸飞机上升和下降时的俯仰方式。 将此值设置为。true
	ProjectileMovement->bRotationFollowsVelocity = true;
	// bShouldBounce：发射物是否应从障碍物弹开。 将此值设置为。true
	ProjectileMovement->bShouldBounce = true;

	// 默认情况下，在 5.0 秒后消失。
	InitialLifeSpan = ProjectileLifespan;
}

// Called when the game starts or when spawned
void AFirstPersonProjectile::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFirstPersonProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFirstPersonProjectile::OnHint(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 仅在我们击中物理时添加 impulse 并摧毁 projectile
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherComp->IsSimulatingPhysics()))
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsForce, GetActorLocation());

		Destroy();
	}
}