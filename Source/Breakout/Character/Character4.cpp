// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Character4.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"
#include "FX/Skill4Actor.h"
#include "Player/CharacterController.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "Game/BOGameInstance.h"
#include "Weapon/WeaponBase.h"
#include "ClientSocket.h"
#include "EnhancedInputSubsystems.h"
ACharacter4::ACharacter4()
{
	NiagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComp"));
	NiagaraComp->SetAutoActivate(false);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> FxRef(TEXT("/Game/Niagara/SKill/Skill4/NS_Skill4_S.NS_Skill4_S"));
	NiagaraComp->SetAsset(FxRef.Object);
}

void ACharacter4::BeginPlay()
{
	Super::BeginPlay();
	TelepoChargeTime = true;
	inst = Cast<UBOGameInstance>(GetGameInstance());
	if(MainController)
		MainController->SetHUDCoolVisibility(false);
}

void ACharacter4::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Blue, FString::Printf(TEXT("RecordedTelpoTime : %f"), CoolChargeTime));
	if (!TelepoChargeTime && MainController)
	{
		CoolChargeTime += DeltaTime;
		MainController->SetHUDCool(CoolChargeTime, 15.f);
		if (CoolChargeTime >= 15.f)
		{
			TelepoChargeTime = true;
			MainController->SetHUDCoolVisibility(false);
			MainController->SetHUDSkillOpacity(1.f);
		}
	}
}

void ACharacter4::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Started, this, &ACharacter4::Skill_S);
		EnhancedInputComponent->BindAction(SkillAction, ETriggerEvent::Completed, this, &ACharacter4::Skill_E);
	}
}

void ACharacter4::Skill_S(const FInputActionValue& Value)
{
	if (!bSaved&& TelepoChargeTime)
	{

		SaveCurLocation();
		FActorSpawnParameters SpawnParameters;
		Temp = GetWorld()->SpawnActor<ANiagaraActor>(NiagaraActor, GetActorLocation(), GetActorRotation(), SpawnParameters);
		if (inst)
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Niagara_packetch1(_SessionId, PlayerType::Character4, GetActorLocation());
	}
	else if(bSaved)
	{
		NiagaraComp->Activate();
		//��Ŷ
		GetMesh()->SetVisibility(false, true);
		GetWorld()->GetTimerManager().SetTimer(TelpoTimer, this, &ACharacter4::SetLocation, 0.5f, false);
		if (inst)
			Cast<UBOGameInstance>(GetGameInstance())->m_Socket->Send_Niagara_cancel(true, _SessionId);
	}
}

void ACharacter4::Skill_E(const FInputActionValue& Value)
{

}

void ACharacter4::SaveCurLocation()
{

	SavedLocation = GetActorLocation();
	bSaved = true;
	
}

void ACharacter4::SetLocation()
{
	MainController->SetHUDCoolVisibility(true);
	MainController->SetHUDSkillOpacity(0.3);

	bSaved = false;
	TelepoChargeTime = false;
	SetActorLocation(SavedLocation);

	NiagaraComp->Deactivate();
	GetMesh()->SetVisibility(true, true);

	Temp->Destroy();
}

void ACharacter4::ServerStartNiagara()
{
	NiagaraComp->Activate();
	//��Ŷ
	GetMesh()->SetVisibility(false, true);
	GetWorld()->GetTimerManager().SetTimer(TelpoTimer, this, &ACharacter4::ServerSetLocation, 0.5f, false);
}

void ACharacter4::ServerSetLocation()
{
	SetActorLocation(SavedLocation);

	NiagaraComp->Deactivate();
	GetMesh()->SetVisibility(true, false);
	CurWeapon->GetWeaponMesh()->SetVisibility(true);
	Temp->Destroy();
}
