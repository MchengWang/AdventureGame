// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupBase.h"
#include "ItemDefinition.h"

// Sets default values
APickupBase::APickupBase()
{
	// 将此 actor 设置为每帧调用 Tick（）。如果不需要，您可以关闭此功能以提高性能。
	PrimaryActorTick.bCanEverTick = true;

	// 创建此拾取物的网格组件
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	check(PickupMeshComponent != nullptr);

	// 创建此拾取物的球体组件
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(SphereComponent != nullptr);

	// 将 sphere 组件附加到 mesh 组件
	SphereComponent->SetupAttachment(PickupMeshComponent);

	// 设置球体的碰撞半径
	SphereComponent->SetSphereRadius(32.f);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	// 使用默认值初始化此拾取
	InitializePickup();
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
*	通过从关联的数据表中检索默认值，使用默认值初始化 pickup。
*/
void APickupBase::InitializePickup()
{
	if (PickupDataTable && !PickupItemID.IsNone())
	{
		// 从数据表中检索与此取件关联的商品数据
		const FItemData* ItemDataRow = PickupDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString());

		UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase.Get();

		// 创建具有类类型的项的副本
		ReferenceItem = TempItemDefinition->CreateItemCopy();

		// 通过调用 IsValid（） 检查网格当前是否已加载。
		if (TempItemDefinition->WorldMesh.IsValid()) {
			// 将拾取物的网格设置为关联项目的网格
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());
		}
		else {
			// 如果网格未加载，请通过调用 LoadSynchronous（） 来加载它。
			UStaticMesh* WorldMesh = TempItemDefinition->WorldMesh.LoadSynchronous();
			PickupMeshComponent->SetStaticMesh(WorldMesh);
		}

		// 将网格设置为 visible。
		PickupMeshComponent->SetVisibility(true);

		// 将网格设置为 visible 和 collidable。
		PickupMeshComponent->SetVisibility(true);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// 注册 Overlap 事件
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereBeginOverlap);
	}

}

/**
*	当角色与此拾取物的 SphereComponent 重叠时广播事件。将拾取物设置为不可见且不可交互，并在设定的时间后重新生成。
*	@param OverlappedComponent - 重叠的组件
*	@param OtherActor - 与此组件重叠的 Actor
*	@param OtherComp - 与此组件重叠的 Actor 组件。
*	@param OtherBodyIndex - 重叠组件的索引。
*	@param bFromSweep - 重叠是否是从扫描生成的。
*	@param SweepResult - 包含有关重叠的信息，例如曲面法线和面。
*/
void APickupBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Attempting a pickup collision"));

	// 检查是否为 AdventureCharacter 重叠
	AAdventureCharacter* Character = Cast<AAdventureCharacter>(OtherActor);

	if (Character != nullptr)
	{
		// 将此条目给角色
		Character->GiveItem(ReferenceItem);

		// 从 Overlap 事件中注销，以便不再触发它
		SphereComponent->OnComponentBeginOverlap.RemoveAll(this);

		// 将此拾取物设置为不可见并禁用碰撞
		PickupMeshComponent->SetVisibility(false);
		PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// 如果拾取物应该重新生成，请等待 fRespawnTime 秒，然后再调用 InitializePickup（） 来重新生成它
	if (bShouldRespawn)
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase::InitializePickup, 4.0f, false, 0);
	}
}

/**
*	每当属性发生更改时，都会更新此拾取。
*	@param PropertyChangedEvent - 包含有关已更改属性的信息.
*/
void APickupBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// 处理父类属性更改
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 如果属性已更改，则获取已更改属性的名称。否则 none。
	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// 验证此类中是否存在 changed 属性，以及 PickupDataTable 是否有效。
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase, PickupItemID) && PickupDataTable)
	{
		// 检索此拾取的关联 ItemData。
		if (const FItemData* ItemDataRow = PickupDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString()))
		{
			UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase;

			// 将拾取物的网格设置为关联项目的网格
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());

			// 设置球体的碰撞半径
			SphereComponent->SetSphereRadius(32.f);
		}
	}
}