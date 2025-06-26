// Fill out your copyright notice in the Description page of Project Settings.


#include "AdventureCharacter.h"

// Sets default values
AAdventureCharacter::AAdventureCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 创建第一人称相机组件
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FirstPersonCameraComponent != nullptr);

	// 为拥有的玩家创建第一人称网格组件
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	// 将第一人称网格附加到骨骼网格
	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	// 第一人称网格包含在第一人称渲染中（在此网格上使用 FirstPersonFieldofView 和 FirstPersonScale）
	FirstPersonMeshComponent->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::FirstPerson;

	// 只有拥有的玩家才能看到第一人称网格
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// 拥有的玩家看不到常规（第三人称）身体网格，但它会投射阴影
	GetMesh()->FirstPersonPrimitiveType = EFirstPersonPrimitiveType::WorldSpaceRepresentation;

	// 将第一人称网格设置为不与其他对象碰撞
	FirstPersonMeshComponent->SetCollisionProfileName(FName("NoCollision"));

	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("head"));

	// 将摄像机放置在眼睛略上方，并将其旋转到玩家的头部后面
	FirstPersonCameraComponent->SetRelativeLocationAndRotation(FirstPersonCameraOffset, FRotator(0.0f, 90.0f, -90.0f));
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// 在摄像机上启用第一人称渲染并设置默认 FOV 和缩放值
	FirstPersonCameraComponent->bEnableFirstPersonFieldOfView = true;
	FirstPersonCameraComponent->bEnableFirstPersonScale = true;
	FirstPersonCameraComponent->FirstPersonFieldOfView = FirstPersonFieldOfView;
	FirstPersonCameraComponent->FirstPersonScale = FirstPersonScale;

	// 为所拥有者创建一个清单组件
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// 只有拥有的玩家才能看到第一人称网格
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// 在第一人称网格上设置动画
	FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	// 拥有的玩家看不到常规 （第三人称） Body Mesh
	GetMesh()->SetOnlyOwnerSee(true);

	// 将相机放置在眼睛略上方的位置。
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.8f, 5.9f, 0.0f));

	// 获取此角色的玩家控制器
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// 获取增强的输入本地玩家子系统，并向其添加新的输入映射上下文
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
	// 检查传递给此函数的 UInputComponent 并将其转换为 UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 绑定移动行为
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Move);

		// 绑定查看行为
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Look);

		// 绑定跳跃行为
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void AAdventureCharacter::Move(const FInputActionValue& Value)
{
	// 从输入作返回的移动值的 2D 向量
	const FVector2D MovementValue = Value.Get<FVector2D>();

	// 检查拥有此 Actor 的控制器是否有效
	if (Controller)
	{
		// 添加左右行为
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		// 添加前后行为
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
	// 检查角色是否还没有此特定工具
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
	// 仅在尚未拥有此工具时装备它
	if (not IsToolAlreadyOwned(ToolDefinition))
	{
		// 生成该工具的新实例以进行装备
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());

		// // 将工具附加到第一人称角色
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		// 将工具附加到此角色，然后附加到其第一人称网格的右手
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));

		// 在角色的网格上设置动画。
		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnim->GeneratedClass);

		// 将工具添加到此角色的物品栏中
		InventoryComponent->ToolInventory.Add(ToolDefinition);

		ToolToEquip->OwningCharacter = this;

		EquippedTool = ToolToEquip;

		// 获取此角色的玩家控制器
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
	// 基于项目类型的大小写
	switch (ItemDefinition->ItemType)
	{
	case EItemType::Tool:
	{
		// 如果该物品是工具，请尝试将其投射并附加到角色上

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