// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"


// Sets default values
AFirstPersonProjectile::AFirstPersonProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ʹ��������Ϊ�򵥵���ײ��ʾ
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	check(CollisionComponent != nullptr);

	// �������䵯���������
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectilemMesh"));
	check(ProjectileMesh != nullptr);

	// ʹ�� ProjectileMovementComponent �����ƴ��䵯���˶�
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComponent"));
	check(ProjectileMovement != nullptr);

	// �� sphere ������ӵ� mesh ���
	ProjectileMesh->SetupAttachment(CollisionComponent);

	CollisionComponent->InitSphereRadius(5.0f);

	// ����ײ�����ļ�����Ϊ ��Projectile�� ��ײԤ��
	CollisionComponent->BodyInstance.SetCollisionProfileName("Projectile");

	// ���ô����������������ʱ��֪ͨ
	CollisionComponent->OnComponentHit.AddDynamic(this, &AFirstPersonProjectile::OnHint);

	// ���ø����
	RootComponent = CollisionComponent;

	ProjectileMovement->UpdatedComponent = CollisionComponent;

	// InitialSpeed������������ʱ�ĳ�ʼ�ٶȡ� ����ֵ����Ϊ��3000.0f
	ProjectileMovement->InitialSpeed = 3000.0f;
	// MaxSpeed�������������ٶȡ� ����ֵ����Ϊ��3000.0f
	ProjectileMovement->MaxSpeed = 3000.0f;
	// bRotationFollowVelocity�������Ƿ�Ӧ��ת��ƥ���ٶȵķ��� ���磬ֽ�ɻ��������½�ʱ�ĸ�����ʽ�� ����ֵ����Ϊ��true
	ProjectileMovement->bRotationFollowsVelocity = true;
	// bShouldBounce���������Ƿ�Ӧ���ϰ��ﵯ���� ����ֵ����Ϊ��true
	ProjectileMovement->bShouldBounce = true;

	// Ĭ������£��� 5.0 �����ʧ��
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
	// �������ǻ�������ʱ��� impulse ���ݻ� projectile
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && (OtherComp->IsSimulatingPhysics()))
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * PhysicsForce, GetActorLocation());

		Destroy();
	}
}