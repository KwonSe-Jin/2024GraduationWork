// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterBase.generated.h"

class UInputAction;

UCLASS()
class BREAKOUT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	virtual void BeginPlay() override;

	//ĳ���� ����
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health = 100.f;

	ECharacterState CharacterState;

private:
	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> Movement;

	//�Է°�
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<class UInputMappingContext> DefalutMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> MoveAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SprintAction;


	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint_S(const FInputActionValue& Value);
	void Sprint_E(const FInputActionValue& Value);
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	IDLE UMETA(DisplayName = "Idle"),
	RUN UMETA(DisplayName = "Run"),
	SPRINT UMETA(DisplayName = "Sprint"),
	FALLING UMETA(DisplayName = "Falling"),

	DEFAULT UMETA(DisplayName = "Default")
};
