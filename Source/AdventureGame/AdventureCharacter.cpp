// Copyright Epic Games, Inc. All Rights Reserved.

#include "AdventureCharacter.h"
#include "EquippableToolBase.h"
#include "EquippableToolDefinition.h"
#include "ItemDefinition.h"
#include "InventoryComponent.h"

// Sets default values
AAdventureCharacter::AAdventureCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it
	PrimaryActorTick.bCanEverTick = true;

	// ������һ�˳�������
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FirstPersonCameraComponent != nullptr);

	// Ϊӵ�е���Ҵ�����һ�˳����������
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	// Ϊӵ�е���Ҵ���������
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// ����һ�˳������帽�ӵ�������
	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	// �������������ӵ���һ�˳ƹ���������
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("Head"));

	// ���� pawn �Կ����������ת
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// ֻ��ӵ�е���Ҳ��ܿ�����һ�˳�����
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// �ڵ�һ�˳������������ö���
	FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	// ӵ�е���ҿ��������� �������˳ƣ� Body Mesh
	GetMesh()->SetOwnerNoSee(true);


	// ������������۾����Ϸ���λ�á�
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.8f, 5.9f, 0.0f));

	// ��ȡ�˽�ɫ����ҿ�����
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// ��ȡ��ǿ�����뱾�������ϵͳ������������µ�����ӳ��������
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(LookContext, 0);
		}
	}

	// ��ʾ������Ϣ 5 ���ӡ�
	// -1 ��Key�� ֵ�����ɷ�ֹ���»�ˢ����Ϣ��
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("We are using AdventureCharacter."));
}

// Called every frame
void AAdventureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAdventureCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//��鴫�ݸ��˺����� UInputComponent ������ת��Ϊ UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// ���ƶ���Ϊ
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Move);

		// �󶨲鿴��Ϊ
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Look);

		// ����Ծ��Ϊ
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

}


void AAdventureCharacter::Move(const FInputActionValue& Value)
{

	// �����������ص��ƶ�ֵ�� 2D ����
	const FVector2d MovementValue = Value.Get<FVector2D>();

	// ���ӵ�д� Actor �Ŀ������Ƿ���Ч
	if (Controller)
	{
		// �� Forward �� Right movement ֵ��ӵ� Actor
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);
	}
}

void AAdventureCharacter::Look(const FInputActionValue& Value)
{
	// Look ֵ�� 2D ʸ��
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	// ���ӵ�д� Actor �Ŀ������Ƿ���Ч
	if (Controller)
	{
		// �� Pitch �� Yaw movement ֵ��ӵ� Actor
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AAdventureCharacter::GiveItem(UItemDefinition* ItemDefinition)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Character: Attempting to gain item"));
	EItemType ItemTypeText = ItemDefinition->ItemType;
	FText ItemName = ItemDefinition->ItemText.Name;

	// ������Ŀ���͵Ĵ�Сд
	switch (ItemDefinition->ItemType)
	{
	case EItemType::Tool:
	{
		// �������Ʒ�ǹ��ߣ��볢�Խ���Ͷ�䲢���ӵ���ɫ��
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Item to give is a tool"));
		UEquippableToolDefinition* ToolDefinition = Cast<UEquippableToolDefinition>(ItemDefinition);
		if (ToolDefinition != nullptr)
		{
			AttachTool(ToolDefinition);
		}
		else {
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Cast to tool failed!"));
		}
		break;
	}
	case EItemType::Consumable:
	{
		// Not yet implemented
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Item to give is a consumable"));
		break;
	}
	default:
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Item is neither"));
		break;
	}
}

bool AAdventureCharacter::IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition)
{
	// ����ɫ�Ƿ�û�д��ض�����
	for (UEquippableToolDefinition* InventoryItem : InventoryComponent->ToolInventory)
	{
		if (ToolDefinition->ID == InventoryItem->ID)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Tool already in inventory!"));
			return true;
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("This is a new tool"));

	return false;
}



void AAdventureCharacter::AttachTool(UEquippableToolDefinition* ToolDefinition)
{

	// ������δӵ�д˹���ʱװ����
	if (not IsToolAlreadyOwned(ToolDefinition))
	{
		// ���ɸù��ߵ���ʵ���Խ���װ��
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());


		// �����߸��ӵ���һ�˳ƽ�ɫ
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		// ȡ�����ӻ����
		if (EquippedTool != nullptr)
		{
			UnequipCurrentTool();
		}

		// �����߸��ӵ��˽�ɫ��Ȼ�󸽼ӵ����һ�˳����������
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));
		//ToolToEquip->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("HandGrip_R")));

		ToolToEquip->OwningCharacter = this;

		// ��������ӵ��˽�ɫ�Ŀ����
		InventoryComponent->ToolInventory.Add(ToolDefinition);

		// �ڵ�һ�˳����������ö�����
		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnim->GeneratedClass);

		EquippedTool = ToolToEquip;

		// ��ȡ�˽�ɫ����ҿ�����
		if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
			{
				Subsystem->AddMappingContext(ToolToEquip->ToolMappingContext, 1);
			}

			ToolToEquip->BindInputAction(UseAction);
		}

	}
}

FVector AAdventureCharacter::GetCameraTargetLocation()
{
	// Ҫ���ص�Ŀ��λ��
	FVector TargetPosition;

	UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		// ��׷�ٵĽ�� FHitResult ����;
		FHitResult Hit;

		// ģ���ɫ�������¿���ʸ���������켣
		const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
		const FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector() * 10000.0;

		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		TargetPosition = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
	}

	return TargetPosition;
}

void AAdventureCharacter::UnequipCurrentTool()
{

	// ��ȡ�˽�ɫ����ҿ�����
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// ɾ�����ߵ�ӳ��������
			Subsystem->RemoveMappingContext(EquippedTool->ToolMappingContext);
		}
	}

	EquippedTool->Destroy();

}