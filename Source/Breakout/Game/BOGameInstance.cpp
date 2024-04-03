// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/BOGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CharacterController.h"

void UBOGameInstance::Init()
{
	Super::Init();
	//ACharacterController* ChController = Cast<ACharacterController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	m_Socket = new ClientSocket(this);
	//m_Socket->StartListen();
	connect = m_Socket->Connect();
	if (connect)
	{
		//c_socket->StartListen();
		UE_LOG(LogClass, Warning, TEXT("IOCP Server connect success!"));
		FString c_id = "testuser";
		FString c_pw = "1234";
		m_Socket->Send_Login_Info(TCHAR_TO_UTF8(*c_id), TCHAR_TO_UTF8(*c_pw));
	}
}
