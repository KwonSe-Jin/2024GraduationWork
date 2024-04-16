
// Fill out your copyright notice in the Description page of Project Settings.


#pragma once
#include "CoreMinimal.h"
#include <queue>
#include "ClientSocket.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include <memory>
#include "Character/CharacterBase.h"
#include "CharacterController.generated.h"

//ClientSocket* c_socket = nullptr;
class CPlayer;
class ClientSocket;
class CPlayerInfo;
class UBOGameInstance;
/**
 *
 */
UCLASS()
class BREAKOUT_API ACharacterController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset1;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset2;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset3;
	UPROPERTY(EditAnywhere)
	TObjectPtr<USkeletalMesh> SkMeshAsset4;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInstance> OldMaterial;

	TObjectPtr<class UMaterialInstanceDynamic> DynamicMaterial;

	// ���� ����ȭ�� 
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> LauncherRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> GrenadeRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> WallRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> BoobyTrapRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> BulletRef;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBullet> ShotgunRef;

	// �÷��̾� ����ȭ ��
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim1;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim2;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim3;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UClass> Anim4;
	// ���̾ư��� ��ų ���� ����ȭ �� 
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UNiagaraSystem> TimeReplayNiagaraRef;


	// �������� ���� �� �ִϸ��̼� ����ȭ ��
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;

	/*UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;*/
	//-------------------------------------------

	UPROPERTY(EditAnywhere, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> Rifle;

	UPROPERTY(EditAnywhere, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> ShotGun;

	UPROPERTY(EditAnywhere, Category = "Combat System", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class AWeaponBase> Lancher;

public:
	ACharacterController();
	CPlayer initplayer;
	// �ٸ� ĳ���͵��� ����
	std::queue<std::shared_ptr<CPlayer>> NewPlayer;
	//int my_session_id;
	//bool ProcessPacket(char* p);

	//virtual void OnPossess(APawn* InPawn) override;
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDStamina(float Stamina, float MaxStamina);
	void SetHUDAmmo(int32 Ammo);
	void SetHUDEscapeTool(int32 EscapeTool);
	void SetHUDBojoImage(EBojoMugiType Type);
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
	void SetHUDMatchingUi();
	float StartCnt = 5.f;
	void SetHUDMatchingUi(float Time);
	void showWeaponSelect();
	void ShowRespawnSelect();
	void ShowMatchingUi();
	// �ʱ� �÷��̾� ����
	//void SetPlayerID(const int playerid) {
	//	UE_LOG(LogTemp, Warning, TEXT("%d -> my_id"), playerid);
	//	id = playerid; }
	void SetPlayerInfo(CPlayerInfo* p_info) {
		if (p_info != nullptr)
			PlayerInfo = p_info;
	}
	void SetInitPlayerInfo(const CPlayer& owner_player);
	//-----------------------------------------------------
	void SetNewCharacterInfo(std::shared_ptr<CPlayer> InitPlayer);
	void SetAttack(int _id);
	void SetHitEffect(int _id);
	void UpdatePlayer();
	//����ȭ ��
	void UpdateSyncPlayer();
	bool UpdateWorld();
	void SetHp(float DamagedHp);
	float damaged = 0;
	//�ʱ� ��Ʈ�ѷ� ����
	
	void InitPlayer();
	//Tick�Լ�
	virtual void Tick(float DeltaTime);
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//void RecvNewPlayer(int sessionID, float x, float y, float z);
	//void SendPlayerPos(int id);

	// ������ų �ٸ� ĳ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
	TSubclassOf<class ACharacterBase> ToSpawn;
	void Set_Weapon_Type(EWeaponType Type);
	//void UpdateWeaponMesh();
	virtual void OnPossess(APawn* InPawn) override;
public:
	TObjectPtr<class AMainHUD> MainHUD;
	int id;
	bool bNewPlayerEntered = false;
	bool bNewWeaponEntered = false;
	bool bInitPlayerSetting = false;
	//ClientSocket* c_socket;
	CPlayerInfo* PlayerInfo;
	int p_cnt;
	bool connect;
	bool Set_Weapon;
	char data[BUFSIZE] = { 0 };
	int remainData = 0;
	bool login_cond;
	
	UBOGameInstance* inst;

};
