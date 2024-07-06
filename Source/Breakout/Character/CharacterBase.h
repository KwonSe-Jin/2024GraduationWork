#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Type/TurningType.h"

#include "CharacterBase.generated.h"

class UInputAction;
#define DEFAULTCAMERALENGTH 200
#define SPRINTCAMERALENGTH 100

UCLASS()
class BREAKOUT_API ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FORCEINLINE ETurningInPlace GetTurningType() { return TurningType; }

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	class AWeaponBase* GetCurWeapon() const { return CurWeapon; }
	void UpdateObtainedEscapeTool();
	/*float GetAO_Yaw();
	float GetAO_Pitch();*/
	int		_SessionId;

	EWeaponType CurWeaponType;
protected:
	virtual void BeginPlay() override;

	void UpdateSprintCamera(float DeltaTime);
	void UpdateStamina(float DeltaTime);
	void SetHUDCrosshair(float DeltaTime);
	void UpdateHpHUD();
	void UpdateStaminaHUD();
	//void UpdateObtainedEscapeTool();

	//ĳ���� ����
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Health = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxStamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float Stamina = 100.f;
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float DeadTime = 4.f;

	bool bCanEscape;
	bool StaminaExhaustionState;

	int32 GrendeNum;
	int32 WallGrendeNum;
	int32 BoobyTrapNum;

	ECharacterState CharacterState;
	EBojoMugiType BojoMugiType;
	int32 ObtainedEscapeToolNum;

	FVector SWAimLastLoc;
	FTransform StartTransform;
public:
	void SetResetState();

	UAnimMontage* GetDeadMontage() { return DeadMontage; }
	void SetbCanEscape(bool _bCanEscape) { bCanEscape = _bCanEscape; }
	int32 GetEscapeToolNum(){ return ObtainedEscapeToolNum; }
	void SetEscapeToolNum(int32 Num) { ObtainedEscapeToolNum = Num; }
	void SetWeapon(TSubclassOf<class AWeaponBase> Weapon, FName SocketName);
	void SetWeaponUi();
	void SetRespawnUi();
	void SetbInRespon(bool _bInRespon) { bInRespon = _bInRespon; }
	bool GetbInRespon() { return bInRespon; }
	void SetbShowSelect(bool _bShowSelect) {bShowSelectUi = _bShowSelect;}
	void SetbCanObtainEscapeTool(bool _bCanObtain);
	void SetHealth(float DamagedHp);

	class AWeaponBase* GetWeapon() { return CurWeapon; }

	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE bool GetbFfirePressed() const { return bFirePressed; }
	FORCEINLINE float MaxGetHealth() const { return MaxHealth; }
	FORCEINLINE float GetStamina() const { return Stamina; }
	FORCEINLINE float MaxGetStamina() const { return MaxStamina; }
	void PlayFireActionMontage();
	FVector GetHitTarget() { return HitTarget; }
	UFUNCTION()
	void ReciveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	TObjectPtr<class AEscapeTool> OverlappingEscapeTool;

	void GrandeThrow();
	void GrandeAim();

	UFUNCTION(BlueprintCallable)
	void GrandeThrowFinish();

	UFUNCTION(BlueprintCallable)
	void SpawnGrenade();

	UFUNCTION(BlueprintCallable)
	void ReloadForMontage();

	void SetSpawnGrenade(TSubclassOf<class AProjectileBase> Projectile);

	void Dead();
	FTimerHandle DeadTimer;
	//int p_id; // player own id
	TObjectPtr<class AWeaponBase> CurWeapon;
protected:

	UPROPERTY(EditAnywhere, Category = Arrow)
	TObjectPtr<class UArrowComponent> PathSorce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<class UNiagaraComponent> Aim;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, Category = Camera)
	TObjectPtr<class UCameraComponent> FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Character, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UCharacterMovementComponent> Movement;

	//������
	bool bDissolve = false;
	float DissolvePercent = -1.f;
	TObjectPtr<class UMaterialInstanceDynamic> MDynamicDissolveInst;
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UMaterialInstance> MDissolveInst;
	//

	TObjectPtr<class AMainHUD> MainHUD;
	TObjectPtr<class ACharacterController> MainController;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> FireActionMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GrenadeMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> DeadMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> ReloadMontage;
	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> InterMontage;

	FName RightSocketName;

	//����ź
	UPROPERTY(EditAnywhere)
	TObjectPtr<class UStaticMeshComponent> Grenade;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> GrenadeClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> WallClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectileBase> BoobyTrapClass;

	//idle turn
	ETurningInPlace TurningType;
	void TurnInPlace(float DeltaTime);

	//���ӿ�����
	void AimOffset(float DeltaTime);
	float AO_Yaw;
	float InterpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;

	//fire
	bool bCanFire;
	bool bFirePressed;
	FTimerHandle FireTimer;
	void StartFireTimer();
	void FireTimerFinished();
	void Fire();
	void FirePressd(bool _Pressd);
	void TraceUnderCrossHiar(FHitResult& TraceHitResult);
	
	bool bInRespon;
	bool bShowSelectUi;
	bool bCanObtainEscapeTool;

	//���ؼ�
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsCenter;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsLeft;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsRight;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsTop;
	UPROPERTY(EditAnywhere, Category = Crosshair)
	TObjectPtr<class UTexture2D> CrosshairsBottom;
	//ũ�ν����������� ��Ÿ���ŭ ���������� ������ �浹��ġ
	FVector HitTarget;

	void SetSprint();
	void SetRun();


	//�Է°�
protected:
	bool CanJump=true;

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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SkillAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> InterAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> ReRoadAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> GrandeFireAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SelectGrandeAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SelectWallAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> SelectTrapAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	TObjectPtr<UInputAction> DetectAction;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint_S(const FInputActionValue& Value);
	void Sprint_E(const FInputActionValue& Value);
	void Fire_S(const FInputActionValue& Value);
	void Fire_E(const FInputActionValue& Value);
	void Inter(const FInputActionValue& Value);
	void Inter_Start(const FInputActionValue& Value);
	void Inter_End(const FInputActionValue& Value);
	void EToolTranfrom(const FInputActionValue& Value);
	void Reroad(const FInputActionValue& Value);
	void GrandeFire_Aiming(const FInputActionValue& Value);
	void GrandeFire(const FInputActionValue& Value);
	void SelectGrande(const FInputActionValue& Value);
	void SelectWall(const FInputActionValue& Value);
	void SelectTrap(const FInputActionValue& Value);
	void StartJump(const FInputActionValue& Value);
	void StopJump(const FInputActionValue& Value);
	virtual void Skill_S(const FInputActionValue& Value);
	virtual void Skill_E(const FInputActionValue& Value);
	void Detect_S(const FInputActionValue& Value);
	void Detect_E(const FInputActionValue& Value);

public:
	//������ �����ϴ� �Լ���
	void SpawnBeam(FVector StartBeam, FVector EndBeam);
	void SpawnHitImpact(FVector HitLoc, FRotator HitRot);
	bool bStarted;
	FTimerHandle StartHandle;
	void StartGame();
	float StartedCnt;
	bool bDeadAnim = false;
	class UBOGameInstance* inst;
	class UCameraComponent* GetCamera() { return FollowCamera; }
	void Server_PlayDeadAnim();
};

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_IDLE UMETA(DisplayName = "Idle"),
	ECS_RUN UMETA(DisplayName = "Run"),
	ECS_SPRINT UMETA(DisplayName = "Sprint"),
	ECS_FALLING UMETA(DisplayName = "Falling"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EBojoMugiType : uint8
{
	E_Grenade UMETA(DisplayName = "Grenade"),
	E_Wall UMETA(DisplayName = "Wall"),
	E_BoobyTrap UMETA(DisplayName = "BoobyTrap"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	E_Rifle UMETA(DisplayName = "Rifle"),
	E_Shotgun UMETA(DisplayName = "Shotgun"),
	E_Launcher UMETA(DisplayName = "Launcher"),

	ECS_DEFAULT UMETA(DisplayName = "Default")
};
