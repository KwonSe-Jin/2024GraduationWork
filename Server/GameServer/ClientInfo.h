#pragma once
#include "CorePch.h"
#include "Overlapped.h"

//�α��� ���� struct
enum Character{
	Character_ARION,
	Character_DANTE,
	Character_EDAN,
	Character_KRATOS
};
enum Weapon {
	Weapon_Rifle,
	Weapon_Shotgun,
	Weapon_RocketLauncher,
};

struct LoginInfo {
	 char id[20];
	 char pw[20];
	 Character c_type;
	 Weapon w_type;
	 int Kill_num;
	 int Death_num;
};
enum CL_STATE { ST_FREE, ST_ACCEPT, ST_INGAME, ST_SERVER }; //  ���� ����
// Ŭ��� ���� ���� Ŭ���� 
class ClientInfo
{
public:
	int cl_id;
	LoginInfo LoginInfo;
	float x, y, z;
	float Yaw, Pitch, Roll;
	float VX, VY, VZ;
	int hp;
	SOCKET c_socket;
	Overlapped c_overlapped;
	int prev;
	CL_STATE cl_state;  //  ���� ����
public:
	ClientInfo();
	~ClientInfo() { closesocket(c_socket); }
	bool c_recv();
	bool c_send(int num_bytes, void* mess);
private:

};

