// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

//---------------------------------
// ��Ŷ �������� Ŭ����
//---------------------------------


#define	MAX_BUFFER		4096
#define SERVER_PORT		12345
#define SERVER_IP		"127.0.0.1"
#define MAX_CLIENTS		100
#define MAX_INFO_SIZE   20

const char CS_LOGIN = 1;
const char SC_LOGIN_OK = 1;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET
{
	unsigned char size;
	char type;
	char id[MAX_INFO_SIZE];
	char pw[MAX_INFO_SIZE];

};
struct SC_LOGIN_BACK {
	unsigned char size;
	char type;
	char id[MAX_INFO_SIZE];
	char pw[MAX_INFO_SIZE];
	float x, y, z;
	int cl_id;
};
#pragma pack(pop)


/**
 * 
 */
class BREAKOUT_API PacketData
{
public:
};
