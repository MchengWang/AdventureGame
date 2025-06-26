// Fill out your copyright notice in the Description page of Project Settings.


#include "DartLauncher.h"
#include "Kismet/KismetMathLibrary.h"
#include "AdventureGame/AdventureCharacter.h"

void ADartLauncher::Use()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Using the dart launcher!"));

	UWorld* const World = GetWorld();
	if (World != nullptr && ProjectileClass != nullptr)
	{
		FVector TargetPosition = OwningCharacter->GetCameraTargetLocation();

		// 获取正确的插槽以从中生成射弹
		FVector SocketLocation = ToolMeshComponent->GetSocketLocation("Muzzle");

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, TargetPosition);

		FVector SpawnLocation = SocketLocation * UKismetMathLibrary::GetForwardVector(SpawnRotation) * 10.0;

		// Set Spawn Collision Handling Override（设置生成碰撞处理覆盖）
		FActorSpawnParameters ActorSpawnParams;

		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// 在枪口处生成投射物
		World->SpawnActor<AFirstPersonProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
	}
}

void ADartLauncher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADartLauncher::BindInputAction(const UInputAction* InputToBind)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("Attempting to bind DartLauncher Use to character"));

	// 设置绑定行为
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// 开火
			EnhancedInputComponent->BindAction(InputToBind, ETriggerEvent::Triggered, this, &ADartLauncher::Use);
		}
	}
}