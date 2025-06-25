#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ItemData.generated.h"

class UItemDefinition;

/**
*	Defines the type of the item.
*/
UENUM()
enum class EItemType : uint8
{
	Tool UMETA(DisplayName = "Tool"),
	Consumable UMETA(DisplayName = "Consumable")
};

/**
*	Contains textual data about the item.
*/
USTRUCT()
struct FItemText
{
	GENERATED_BODY()

	// The text name of the item.
	UPROPERTY(EditAnywhere)
	FText Name;

	// The text description of the item.
	UPROPERTY(EditAnywhere)
	FText Description;
};

/**
*	Defines a basic item that can be used in a data table.
*/
USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

	// The ID name of this item for referencing in a table row.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FName ID;

	// The type of the item.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	EItemType ItemType;

	// Text struct including the item name and description.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	FItemText ItemText;

	// The Data Asset item definition associated with this item.
	UPROPERTY(EditAnywhere, Category = "Item Data")
	TObjectPtr<UItemDefinition> ItemBase;
};