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

	// 创建第一人称相机组件
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FirstPersonCameraComponent != nullptr);

	// 为拥有的玩家创建第一人称望网格组件
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	// 为拥有的玩家创建库存组件
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// 将第一人称网格体附加到骨骼体
	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	// 将摄像机组件附加到第一人称骨骼网格体
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("Head"));

	// 启用 pawn 以控制摄像机旋转
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// 只用拥有的玩家才能看到第一人称网格
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// 在第一人称网格体上设置动画
	FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	// 拥有的玩家看不到常规 （第三人称） Body Mesh
	GetMesh()->SetOwnerNoSee(true);


	// 将相机放置在眼睛略上方的位置。
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.8f, 5.9f, 0.0f));

	// 获取此角色的玩家控制器
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// 获取增强的输入本地玩家子系统，并向其添加新的输入映射上下文
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(LookContext, 0);
		}
	}

	// 显示调试消息 5 秒钟。
	// -1 “Key” 值参数可防止更新或刷新消息。
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
	//检查传递给此函数的 UInputComponent 并将其转换为 UEnhancedInputComponent
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
	const FVector2d MovementValue = Value.Get<FVector2D>();

	// 检查拥有此 Actor 的控制器是否有效
	if (Controller)
	{
		// 将 Forward 和 Right movement 值添加到 Actor
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);
	}
}

void AAdventureCharacter::Look(const FInputActionValue& Value)
{
	// Look 值的 2D 矢量
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	// 检查拥有此 Actor 的控制器是否有效
	if (Controller)
	{
		// 将 Pitch 和 Yaw movement 值添加到 Actor
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AAdventureCharacter::GiveItem(UItemDefinition* ItemDefinition)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Character: Attempting to gain item"));
	EItemType ItemTypeText = ItemDefinition->ItemType;
	FText ItemName = ItemDefinition->ItemText.Name;

	// 基于项目类型的大小写
	switch (ItemDefinition->ItemType)
	{
	case EItemType::Tool:
	{
		// 如果该物品是工具，请尝试将其投射并附加到角色上
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
	// 检查角色是否还没有此特定工具
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

	// 仅在尚未拥有此工具时装备它
	if (not IsToolAlreadyOwned(ToolDefinition))
	{
		// 生成该工具的新实例以进行装备
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());


		// 将工具附加到第一人称角色
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);

		// 取消连接活动工具
		if (EquippedTool != nullptr)
		{
			UnequipCurrentTool();
		}

		// 将工具附加到此角色，然后附加到其第一人称网格的右手
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));
		//ToolToEquip->AttachToComponent(GetMesh(), AttachmentRules, FName(TEXT("HandGrip_R")));

		ToolToEquip->OwningCharacter = this;

		// 将工具添加到此角色的库存中
		InventoryComponent->ToolInventory.Add(ToolDefinition);

		// 在第一人称网格上设置动画。
		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnim->GeneratedClass);

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

FVector AAdventureCharacter::GetCameraTargetLocation()
{
	// 要返回的目标位置
	FVector TargetPosition;

	UWorld* const World = GetWorld();

	if (World != nullptr)
	{
		// 线追踪的结果 FHitResult 命中;
		FHitResult Hit;

		// 模拟角色沿其向下看的矢量的线条轨迹
		const FVector TraceStart = FirstPersonCameraComponent->GetComponentLocation();
		const FVector TraceEnd = TraceStart + FirstPersonCameraComponent->GetForwardVector() * 10000.0;

		World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECollisionChannel::ECC_Visibility);

		TargetPosition = Hit.bBlockingHit ? Hit.ImpactPoint : Hit.TraceEnd;
	}

	return TargetPosition;
}

void AAdventureCharacter::UnequipCurrentTool()
{

	// 获取此角色的玩家控制器
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// 删除工具的映射上下文
			Subsystem->RemoveMappingContext(EquippedTool->ToolMappingContext);
		}
	}

	EquippedTool->Destroy();

}