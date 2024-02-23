// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/CharacterBase.h"
#include "Character1.generated.h"

/**
 * 
 */
UCLASS()
class BREAKOUT_API ACharacter1 : public ACharacterBase
{
	GENERATED_BODY()
	
public:
	ACharacter1();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent);
protected:
	virtual void Skill_S(const FInputActionValue& Value) override;
	virtual void Skill_E(const FInputActionValue& Value) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UNiagaraComponent> NiagaraComp;
private:
	TDoubleLinkedList<FCharacterFrameData> FrameDatas;
	void StoreFrameData(float DeltaTime);
	void Replay(float DeltaTime);
	bool bTimeReplay = false;
	//out of time data, cannot keep replay
	bool bOutOfData;
	//������ ����Ƚð�
	float RunningTime;
	//�������Ӹ����� ��ŸŸ��
	float LeftRunningTime;
	float RightRunningTime;
	//����� �������Ӹ��ٿ��� ��ϵ� ��ü�ð� 
	float RecordedTime;
	float MaxSaveTime = 5.f;
	float Temp = 0.2f;
};

USTRUCT(BluePrintType)
struct FCharacterFrameData
{
	GENERATED_BODY()

	FVector Location;
	float DeltaTime;

	FCharacterFrameData()
	{
	};
	FCharacterFrameData(FVector _Location, float _DeltaTime)
	{
		Location = _Location;
		DeltaTime = _DeltaTime;
	};
};