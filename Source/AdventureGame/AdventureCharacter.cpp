// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureCharacter.h"

// Sets default values
AAdventureCharacter::AAdventureCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// ������һ�˳�������
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FirstPersonCameraComponent != nullptr);

	// Ϊӵ�е���Ҵ�����һ�˳��������
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	// ����һ�˳����񸽼ӵ���������
	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	// ��һ�˳���������ڵ�һ�˳���Ⱦ�У��ڴ�������ʹ�� FirstPersonFieldofView �� FirstPersonScale��
	FirstPersonMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// ֻ��ӵ�е���Ҳ��ܿ�����һ�˳�����
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// ӵ�е���ҿ��������棨�����˳ƣ��������񣬵�����Ͷ����Ӱ
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	// ����һ�˳���������Ϊ��������������ײ
	FirstPersonMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("head"));

	// ��������������۾����Ϸ�����������ת����ҵ�ͷ������
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FirstPersonCameraOffset, FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// ������������õ�һ�˳���Ⱦ������Ĭ�� FOV ������ֵ
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = FirstPersonFieldOfView;
	FirstPersonCameraComponent->FirstPersonScale = FirstPersonScale;

	// Ϊ��ӵ���ߴ���һ���嵥���
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// ֻ��ӵ�е���Ҳ��ܿ�����һ�˳�����
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// �ڵ�һ�˳����������ö���
	FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	// ӵ�е���ҿ��������� �������˳ƣ� Body Mesh
	GetMesh()->SetOnlyOwnerSee(true);

	// ������������۾����Ϸ���λ�á�
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.8f, 5.9f, 0.0f));

	// ��ȡ�˽�ɫ����ҿ�����
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// ��ȡ��ǿ�����뱾�������ϵͳ������������µ�����ӳ��������
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(FirstPersonContext, 0);
		}
	}

	// Display a debug message for five seconds. 
	// The -1 "Key" value argument prevents the message from being updated or refreshed.
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
	// ��鴫�ݸ��˺����� UInputComponent ������ת��Ϊ UEnhancedInputComponent
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
	const FVector2D MovementValue = Value.Get<FVector2D>();

	// ���ӵ�д� Actor �Ŀ������Ƿ���Ч
	if (Controller)
	{
		// ���������Ϊ
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		// ���ǰ����Ϊ
		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);
	}
}

void AAdventureCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	if (Controller)
	{
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

bool AAdventureCharacter::IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition)
{
	// ����ɫ�Ƿ�û�д��ض�����
	for (UEquippableToolDefinition* InventoryItem : InventoryComponent->ToolInventory)
	{
		if (ToolDefinition->ID == InventoryItem->ID)
		{
			return true;
		}
	}
}

void AAdventureCharacter::AttachTool(UEquippableToolDefinition* ToolDefinition)
{
	// ������δӵ�д˹���ʱװ����
	if (not IsToolAlreadyOwned(ToolDefinition))
	{
		// ���ɸù��ߵ���ʵ���Խ���װ��
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());

		// // �����߸��ӵ���һ�˳ƽ�ɫ
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		// �����߸��ӵ��˽�ɫ��Ȼ�󸽼ӵ����һ�˳����������
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));

		// �ڽ�ɫ�����������ö�����
		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnim->GeneratedClass);

		// ��������ӵ��˽�ɫ����Ʒ����
		InventoryComponent->ToolInventory.Add(ToolDefinition);

		ToolToEquip->OwningCharacter = this;

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

void AAdventureCharacter::GiveItem(UItemDefinition* ItemDefinition)
{
	// ������Ŀ���͵Ĵ�Сд
	switch (ItemDefinition->ItemType)
	{
	case EItemType::Tool:
	{
		// �������Ʒ�ǹ��ߣ��볢�Խ���Ͷ�䲢���ӵ���ɫ��

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
		// Not implemented
		break;
	}
	default:
		break;

	}
}