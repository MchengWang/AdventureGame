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

	// Create a first person camera component
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	check(FirstPersonCameraComponent != nullptr);

	// Create a first person mesh component for the owning player
	FirstPersonMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FirstPersonMesh"));
	check(FirstPersonMeshComponent != nullptr);

	// Create an inventory component for the owning player
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	// Attach the FirstPerson mesh to the Skeletal Mesh
	FirstPersonMeshComponent->SetupAttachment(GetMesh());

	// Attach the camera component to the first-person Skeletal Mesh
	FirstPersonCameraComponent->SetupAttachment(FirstPersonMeshComponent, FName("Head"));

	// Enable the pawn to control camera rotation
	FirstPersonCameraComponent->bUsePawnControlRotation = true;
}

// Called when the game starts or when spawned
void AAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	check(GEngine != nullptr);

	// Only the owning player sees the first person mesh.
	FirstPersonMeshComponent->SetOnlyOwnerSee(true);

	// Set the animations on the first person mesh.
	FirstPersonMeshComponent->SetAnimInstanceClass(FirstPersonDefaultAnim->GeneratedClass);

	// The owning player doesn't see the regular (third-person) body mesh
	GetMesh()->SetOwnerNoSee(true);


	// Position the camera slightly above the eyes.
	FirstPersonCameraComponent->SetRelativeLocation(FVector(2.8f, 5.9f, 0.0f));

	// Get the player controller for this character
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		// Get the enhanced input local player subsystem and add a new input mapping context to it
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(LookContext, 0);
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
	// Check the UInputComponent passed to this function and cast it to an UEnhancedInputComponent
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Bind Movement Actions
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Move);

		// Bind Look Actions
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AAdventureCharacter::Look);

		// Bind Jump Actions
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

}


void AAdventureCharacter::Move(const FInputActionValue& Value)
{

	// 2D Vector of movement values returned from the input action
	const FVector2d MovementValue = Value.Get<FVector2D>();

	// Check if the controller possessing this Actor is valid
	if (Controller)
	{
		// Add Forward and Right movement values to the Actor
		const FVector Right = GetActorRightVector();
		AddMovementInput(Right, MovementValue.X);

		const FVector Forward = GetActorForwardVector();
		AddMovementInput(Forward, MovementValue.Y);
	}
}

void AAdventureCharacter::Look(const FInputActionValue& Value)
{
	// 2D Vector of look values 
	const FVector2D LookAxisValue = Value.Get<FVector2D>();

	// Check if the controller possessing this Actor is valid
	if (Controller)
	{
		// Add Pitch and Yaw movement values to the Actor
		AddControllerYawInput(LookAxisValue.X);
		AddControllerPitchInput(LookAxisValue.Y);
	}
}

void AAdventureCharacter::GiveItem(UItemDefinition* ItemDefinition)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Character: Attempting to gain item"));
	EItemType ItemTypeText = ItemDefinition->ItemType;
	FText ItemName = ItemDefinition->ItemText.Name;

	// Case based on the type of the item
	switch (ItemDefinition->ItemType)
	{
	case EItemType::Tool:
	{
		// If the item is a tool, attempt to cast and attach it to the character
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
	// Check that the character does not yet have this particular tool
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

	// Only equip this tool if it isn't already owned
	if (not IsToolAlreadyOwned(ToolDefinition))
	{
		// Spawn a new instance of the tool to equip
		AEquippableToolBase* ToolToEquip = GetWorld()->SpawnActor<AEquippableToolBase>(ToolDefinition->ToolAsset, this->GetActorTransform());


		// Attach the tool to the First Person Character
		FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);


		// Attach the tool to this character, and then the right hand of their first person mesh
		ToolToEquip->AttachToActor(this, AttachmentRules);
		ToolToEquip->AttachToComponent(FirstPersonMeshComponent, AttachmentRules, FName(TEXT("HandGrip_R")));

		ToolToEquip->OwningCharacter = this;

		// Add the tool to this character's inventory
		InventoryComponent->ToolInventory.Add(ToolDefinition);

		// Set the animations on the first person mesh.
		FirstPersonMeshComponent->SetAnimInstanceClass(ToolToEquip->FirstPersonToolAnim->GeneratedClass);
		GetMesh()->SetAnimInstanceClass(ToolToEquip->ThirdPersonToolAnim->GeneratedClass);

		EquippedTool = ToolToEquip;

		// Get the player controller for this character
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