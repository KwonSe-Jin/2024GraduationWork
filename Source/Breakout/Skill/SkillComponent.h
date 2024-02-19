// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BREAKOUT_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USkillComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	TObjectPtr<class ACharacterBase> Character;
	TObjectPtr<class AController> Controller;

public:
	FORCEINLINE void SetSelectedSkill(ESelectedSkill Selected) { CurSelectedSKill = Selected; }
	FORCEINLINE ESelectedSkill GetSelectedSkill() { return CurSelectedSKill; }

	FORCEINLINE void SetIsReverse(bool _IsReverse) { bTimeReplay = _IsReverse; }
	FORCEINLINE void SetIsDash(bool _IsDash) { bDash = _IsDash; }
	FORCEINLINE int32 GetDashPoint() { return DashPoint; }
	FORCEINLINE void SetIsGhost(bool _bGhost) { bGhost = _bGhost; }
	FORCEINLINE void SetIsCharageTime(bool _GhostCoolChargeTime) { GhostCoolChargeTime = _GhostCoolChargeTime; }
	void DashStart();
private:
	ESelectedSkill CurSelectedSKill;
	//Skill1 �ð��ǵ�����
	TDoubleLinkedList<FCharacterFrameData> FrameDatas;
	void StoreFrameData(float DeltaTime);
	void Replay(float DeltaTime);
	bool bTimeReplay=false;
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

	//Skill2 �뽬3��
	bool bDash=false;
	bool bCoolTimeFinish = true;
	int32 DashPoint = 3;
	float DashCoolChargeTime=0.f;
	//void DashStart();
	FVector OldVelocity;
	FTimerHandle DashTimer;
	void FinishDashTimer();
	void CoolTimeDashTimer();

	//Skill3 ����ȭ
	bool bGhost = false;
	float GhostCoolChargeTime = 0.f;
	float RecordedGhostTime = 0.f;
	FTimerHandle GhostTimer;
	void GhostStart();
	void GhostEnd();
	TObjectPtr<class UNiagaraSystem> GhostFX;
	//�����̵�
	FVector SavedLocation;
	bool TelepoChargeTime=true;
	bool bSaved=false;
	bool CanTelepo = false;
	void SetCanTelepo() { CanTelepo = true; 		Toggle += 1;}
public:
	void SaveCurLocation();
	void SetLocation();
	int Toggle = 1;
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


UENUM(BlueprintType)
enum class ESelectedSkill : uint8
{
	E_Skill1 UMETA(DisplayName = "E_Skill1"),
	E_Skill2 UMETA(DisplayName = "E_Skill2"),
	E_Skill3 UMETA(DisplayName = "E_Skill3"),
	E_Skill4 UMETA(DisplayName = "E_Skill4"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};