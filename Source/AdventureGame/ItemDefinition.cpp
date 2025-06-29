// Fill out your copyright notice in the Description page of Project Settings.


#include "ItemDefinition.h"

/**
* 创建并返回此项定义的副本
* @return 项目的副本
*/
UItemDefinition* UItemDefinition::CreateItemCopy() const
{
	UItemDefinition* ItemCopy = NewObject<UItemDefinition>(StaticClass());

	ItemCopy->ID = this->ID;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->ItemText = this->ItemText;
	ItemCopy->WorldMesh = this->WorldMesh;

	return ItemCopy;
}