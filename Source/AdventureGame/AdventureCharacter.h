// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "AdventureCharacter.generated.h"

class AEquippableToolBase;
class UAnimBlueprint;
class UEquippableToolDefinition;
class UInputMappingContext;
class UInputAction;
class UInputComponent;
class UItemDefinition;
class UInventoryComponent;

UCLASS()
class ADVENTUREGAME_API AAdventureCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAdventureCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Input Mapping Context
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputMappingContext> LookContext;

	// Move Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;

	// Jump Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;

	// Look Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;

	// Switch Tool Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SwitchToolAction;

	// Use Input Actions
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> UseAction;

	// First Person animations
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	UAnimBlueprint* FirstPersonDefaultAnim;

	// The currently equipped tool
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Tools)
	TObjectPtr<AEquippableToolBase> EquippedTool;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Handles 2D Movement Input
	UFUNCTION()
	void Move(const FInputActionValue& Value);

	// Handles Look Input
	UFUNCTION()
	void Look(const FInputActionValue& Value);

	// Attaches and equips a tool to the player
	UFUNCTION()
	void AttachTool(UEquippableToolDefinition* ToolDefinition);

	// Public function that other classes can call to attempt to give an item to the player
	UFUNCTION()
	void GiveItem(UItemDefinition* ItemDefinition);

	// Returns whether or not the player already owns this tool
	UFUNCTION()
	bool IsToolAlreadyOwned(UEquippableToolDefinition* ToolDefinition);

	// First Person camera
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCameraComponent;

	// First-person mesh, visible only to the owning player
	UPROPERTY(VisibleAnywhere, Category = Mesh)
	TObjectPtr<USkeletalMeshComponent> FirstPersonMeshComponent;

	// Inventory Component
	UPROPERTY(VisibleAnywhere, Category = Inventory)
	TObjectPtr<UInventoryComponent> InventoryComponent;
};