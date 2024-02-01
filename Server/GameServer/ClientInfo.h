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
public:
	ClientInfo();
	~ClientInfo() { };
private:

};

