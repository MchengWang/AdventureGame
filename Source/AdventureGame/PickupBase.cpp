// Copyright Epic Games, Inc. All Rights Reserved.

#include "PickupBase.h"
#include "ItemDefinition.h"

// Sets default values
APickupBase::APickupBase()
{
	// ���� actor ����Ϊÿ֡���� Tick�������������Ҫ�������Թرմ˹�����������ܡ�
	PrimaryActorTick.bCanEverTick = true;

	// ������ʰȡ����������
	PickupMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PickupMesh"));
	check(PickupMeshComponent != nullptr);

	// ������ʰȡ����������
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	check(SphereComponent != nullptr);

	// �� sphere ������ӵ� mesh ���
	SphereComponent->SetupAttachment(PickupMeshComponent);

	// �����������ײ�뾶
	SphereComponent->SetSphereRadius(32.f);
}

// Called when the game starts or when spawned
void APickupBase::BeginPlay()
{
	Super::BeginPlay();

	// ʹ��Ĭ��ֵ��ʼ����ʰȡ
	InitializePickup();
}

// Called every frame
void APickupBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/**
*	ͨ���ӹ��������ݱ��м���Ĭ��ֵ��ʹ��Ĭ��ֵ��ʼ�� pickup��
*/
void APickupBase::InitializePickup()
{
	if (PickupDataTable && !PickupItemID.IsNone())
	{
		// �����ݱ��м������ȡ����������Ʒ����
		const FItemData* ItemDataRow = PickupDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString());

		UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase.Get();

		// �������������͵���ĸ���
		ReferenceItem = TempItemDefinition->CreateItemCopy();

		// ͨ������ IsValid���� �������ǰ�Ƿ��Ѽ��ء�
		if (TempItemDefinition->WorldMesh.IsValid()) {
			// ��ʰȡ�����������Ϊ������Ŀ������
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());
		}
		else {
			// �������δ���أ���ͨ������ LoadSynchronous���� ����������
			UStaticMesh* WorldMesh = TempItemDefinition->WorldMesh.LoadSynchronous();
			PickupMeshComponent->SetStaticMesh(WorldMesh);
		}

		// ����������Ϊ visible��
		PickupMeshComponent->SetVisibility(true);

		// ����������Ϊ visible �� collidable��
		PickupMeshComponent->SetVisibility(true);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

		// ע�� Overlap �¼�
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &APickupBase::OnSphereBeginOverlap);
	}

}

/**
*	����ɫ���ʰȡ��� SphereComponent �ص�ʱ�㲥�¼�����ʰȡ������Ϊ���ɼ��Ҳ��ɽ����������趨��ʱ����������ɡ�
*	@param OverlappedComponent - �ص������
*	@param OtherActor - �������ص��� Actor
*	@param OtherComp - �������ص��� Actor �����
*	@param OtherBodyIndex - �ص������������
*	@param bFromSweep - �ص��Ƿ��Ǵ�ɨ�����ɵġ�
*	@param SweepResult - �����й��ص�����Ϣ���������淨�ߺ��档
*/
void APickupBase::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Attempting a pickup collision"));

	// ����Ƿ�Ϊ AdventureCharacter �ص�
	AAdventureCharacter* Character = Cast<AAdventureCharacter>(OtherActor);

	if (Character != nullptr)
	{
		// ������Ŀ����ɫ
		Character->GiveItem(ReferenceItem);

		// �� Overlap �¼���ע�����Ա㲻�ٴ�����
		SphereComponent->OnComponentBeginOverlap.RemoveAll(this);

		// ����ʰȡ������Ϊ���ɼ���������ײ
		PickupMeshComponent->SetVisibility(false);
		PickupMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	// ���ʰȡ��Ӧ���������ɣ���ȴ� fRespawnTime �룬Ȼ���ٵ��� InitializePickup���� ������������
	if (bShouldRespawn)
	{
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &APickupBase::InitializePickup, 4.0f, false, 0);
	}
}

/**
*	ÿ�����Է�������ʱ��������´�ʰȡ��
*	@param PropertyChangedEvent - �����й��Ѹ������Ե���Ϣ.
*/
void APickupBase::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// ���������Ը���
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// ��������Ѹ��ģ����ȡ�Ѹ������Ե����ơ����� none��
	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	// ��֤�������Ƿ���� changed ���ԣ��Լ� PickupDataTable �Ƿ���Ч��
	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickupBase, PickupItemID) && PickupDataTable)
	{
		// ������ʰȡ�Ĺ��� ItemData��
		if (const FItemData* ItemDataRow = PickupDataTable->FindRow<FItemData>(PickupItemID, PickupItemID.ToString()))
		{
			UItemDefinition* TempItemDefinition = ItemDataRow->ItemBase;

			// ��ʰȡ�����������Ϊ������Ŀ������
			PickupMeshComponent->SetStaticMesh(TempItemDefinition->WorldMesh.Get());

			// �����������ײ�뾶
			SphereComponent->SetSphereRadius(32.f);
		}
	}
}