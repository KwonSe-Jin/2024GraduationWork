#pragma once

#define MAX_INFO_SIZE   20
const short SERVER_PORT = 12345;

const int BUFSIZE = 256;
const int  MAX_NAME_SIZE = 20;

const int  MAX_USER = 10000;
const int  MAX_OBJ = 20;

const char CS_LOGIN = 1;
const char CS_MOVE = 2;
const char CS_SELECT_CHAR = 3;
const char CS_SELECT_WEP = 4;


const char SC_LOGIN_OK = 1;
const char SC_OTHER_PLAYER = 2;
const char SC_MOVE_PLAYER = 3;




//const char SC_PACKET_REMOVE_OBJECT = 4;
//
//const char SC_PACKET_LOGIN_FAIL = 6;
//const char SC_PACKET_STATUS_CHANGE = 7;
//const char SC_PACKET_DISCONNECT = 8;
//const char SC_PACKET_HP = 9;
//
//const char SC_PACKET_ATTACK = 11;
//const char SC_PACKET_GET_ITEM = 12;
enum PlayerType
{
	Character1,
	Character2,
	Character3,
	Character4
};
enum WeaponType
{
	RIFLE,
	ShotGun,
	Launcher
};
#pragma pack (push, 1)
struct CS_LOGIN_PACKET
{
	unsigned char size;
	char type;
	char id[MAX_INFO_SIZE];
	char pw[MAX_INFO_SIZE];
	float z;

};
struct SC_LOGIN_BACK {
	unsigned char size;
	char type;
	/*char id[MAX_INFO_SIZE];
	char pw[MAX_INFO_SIZE];*/
	int clientid;
	float x, y, z;
	float yaw;
	int cl_id;
};
struct CS_MOVE_PACKET
{
	unsigned char size;
	char type;
	int	id;
	float Max_speed;
	float x, y, z;
	float vx, vy, vz;
	float yaw;
};
//struct SC_MOVE_BACK {
//	unsigned char size;
//	char type;
//	int		id;
//	float x, y, z;
//};
//����ȭ �� ��Ŷ
struct SC_PLAYER_SYNC {
	unsigned char size;
	char type;
	int id;
	float Max_speed;
	float x, y, z;
	float yaw;
	char object_type;
	char name[MAX_INFO_SIZE];
};
struct CS_SELECT_CHARACTER
{
	unsigned char size;
	char type;
	int id;
	PlayerType character_type;
};
struct CS_SELECT_WEAPO
{
	unsigned char size;
	char type;
	int id;
	WeaponType weapon_type;
};


//�߰� ���� ����
struct cs_packet_start { // ���� ���� ��û
	unsigned char size;
	char	type;
	bool	ready;
};
struct sc_packet_ready { // Ÿ �÷��̾� ����
	unsigned char size;
	char	type;
	char	name[MAX_NAME_SIZE];
};

struct sc_packet_start_ok { // ����
	unsigned char size;
	char type;
	char	name[MAX_NAME_SIZE];
	float x, y, z;
	char image_num;
};

struct cs_packet_attack {
	unsigned char size;
	char	type;
	int s_id;
};

struct cs_packet_damage {
	unsigned char size;
	char	type;
};

struct cs_packet_get_item {
	unsigned char size;
	char	type;
	int s_id;
	char    item_num;
};

struct cs_packet_chat {
	unsigned char size;
	char	type;
	int s_id;
	float x, y, z;
	char	message[100];
};

struct cs_packet_respawn {

	unsigned char size;
	char	type;
};

struct sc_packet_remove_object {
	unsigned char size;
	char type;
	int id;
};

struct sc_packet_chat {
	unsigned char size;
	char type;
	int id;
	char message[100];
};

struct sc_packet_login_fail {
	unsigned char size;
	char type;
	int	 reason;		
};

#pragma pack(pop)
