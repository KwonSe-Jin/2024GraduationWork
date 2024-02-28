// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "CharacterController.generated.h"
class ClientSocket;
//class CPlayer;
class CPlayerInfo;
/**
 * 
 */
UCLASS()
class BREAKOUT_API ACharacterController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:

	int my_session_id;
	int other_session_id;
	int other_x;
	int other_y;
	int other_z;
	int count;
	
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDStamina(float Stamina, float MaxStamina);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDEscapeTool(int32 EscapeTool);
	void SetHUDCrosshair(const struct FCrosshairPackage& Package);
	//skill
	void SetHUDSkill();
	//��ų ������ ����
	void SetHUDSkillOpacity(float Opacity);
	//skill1,3,4 ��Ÿ��
	void SetHUDCool(float Cool, float MaxCool);
	//skill2 �뽬����Ʈ
	void SetHUDCool(int32 Cool);
	void SetHUDCoolVisibility(bool bVisibility);
	void showWeaponSelect();
	void RecvNewPlayer(int sessionID, float x, float y, float z);
	void SendPlayerPos(int id);
	//void SetNewCharacterInfo(shared_ptr<CPlayer> InitPlayer);
	//����ȭ ��
	//void UpdateSyncPlayer();
	// ������ų �ٸ� ĳ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacter> ToSpawn;
	virtual void OnPossess(APawn* InPawn) override;
private:
	TObjectPtr<class AMainHUD> MainHUD;
	bool bNewPlayerEntered = false;
	ClientSocket* c_socket;
	CPlayerInfo* PlayerInfo;  
	//CPlayer initplayer;
	// �ٸ� ĳ���͵��� ����
	//queue<shared_ptr<CPlayer>>	NewPlayer;
};
