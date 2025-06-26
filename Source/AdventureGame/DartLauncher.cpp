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

		// ��ȡ��ȷ�Ĳ���Դ��������䵯
		FVector SocketLocation = ToolMeshComponent->GetSocketLocation("Muzzle");

		FRotator SpawnRotation = UKismetMathLibrary::FindLookAtRotation(SocketLocation, TargetPosition);

		FVector SpawnLocation = SocketLocation * UKismetMathLibrary::GetForwardVector(SpawnRotation) * 10.0;

		// Set Spawn Collision Handling Override������������ײ�����ǣ�
		FActorSpawnParameters ActorSpawnParams;

		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		// ��ǹ�ڴ�����Ͷ����
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

	// ���ð���Ϊ
	if (APlayerController* PlayerController = Cast<APlayerController>(OwningCharacter->GetController()))
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// ����
			EnhancedInputComponent->BindAction(InputToBind, ETriggerEvent::Triggered, this, &ADartLauncher::Use);
		}
	}
}