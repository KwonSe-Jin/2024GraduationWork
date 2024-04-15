#pragma once
#define SERVER_PORT		8001
#include <vector>
#include <array>
#define SERVER_IP		"192.168.103.224"
#define MAX_INFO_SIZE   20
//const char CS_PACKET_ATTACK = 3;
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
	SHOTGUN,
	LAUNCHER
};
struct SHOTGUNLOC {
	float x;
	float y;
	float z;
};
constexpr int BUFSIZE = 1048;
const int  ReZone_HEIGHT = 2000;
const int  ReZone_WIDTH = 2000;
const int  MAX_NAME_SIZE = 20;
const int  MAX_CHAT_SIZE = 100;
const int  MAX_USER = 10000;
const int  MAX_OBJ = 20;

const char CS_LOGIN = 1;
const char CS_MOVE_Packet = 0;
const char CS_SELECT_CHAR = 3;
const char CS_SELECT_WEP = 4;
const char CS_READY = 5;
const char CS_ATTACK = 6;
const char CS_HIT_EFFECT = 7;
const char CS_DAMAGE = 8;
const char CS_SIGNAl = 9;
const char CS_SHOTGUN_BEAM = 10;
const char CS_SHOTGUN_DAMAGED = 11;
const char CS_NiAGARA = 12;



const char SC_LOGIN_OK = 1;
const char SC_OTHER_PLAYER = 2;
const char SC_MOVE_PLAYER = 3;
const char SC_CHAR_BACK = 4;
const char SC_OTHER_WEAPO = 5;
const char SC_ALL_READY = 6;
const char SC_ATTACK = 7;
const char SC_EFFECT = 8;
const char SC_PLAYER_DAMAGE = 9;
const char SC_SHOTGUN_BEAM = 10;
const char SC_SHOTGUN_DAMAGED = 11;
const char SC_NiAGARA = 12;
//const char CS_PACKET_DAMAGE = 7;
//const char CS_PACKET_GET_ITEM = 8;
//
//const char SC_LOGIN_OK = 1;
//const char SC_OTHER_PLAYER = 2;
//const char SC_MOVE_PLAYER = 3;
//
//const char SC_PACKET_REMOVE_OBJECT = 4;
//
//const char SC_PACKET_LOGIN_FAIL = 6;
//const char SC_PACKET_STATUS_CHANGE = 7;
//const char SC_PACKET_DISCONNECT = 8;
//const char SC_PACKET_HP = 9;
//
//const char SC_PACKET_ATTACK = 11;
//const char SC_PACKET_GET_ITEM = 12;

#pragma pack (push, 1)
struct CS_LOGIN_PACKET
{
	unsigned char size;
	unsigned type;
	char id[MAX_INFO_SIZE];
	char pw[MAX_INFO_SIZE];
};	
struct SC_LOGIN_BACK {
	unsigned char size;
	unsigned type;
	int id;
};
#pragma pack(pop)


#pragma pack (push, 1)
struct CS_MOVE_PACKET
{
	unsigned char size;
	unsigned type;
	int	id;
	float Max_speed;
	float x, y, z;
	float vx, vy, vz;
	float yaw;
};
struct SC_PLAYER_SYNC {
	unsigned char size;
	unsigned type;
	int id;
	float Max_speed;
	float x, y, z;
	float yaw;
	//char object_type;
	char name[MAX_INFO_SIZE];
	PlayerType p_type;
};
struct CS_SELECT_CHARACTER
{
	unsigned char size;
	unsigned type;
	int id;
	float x, y, z;
	PlayerType p_type;
};
struct SC_SELECT_CHARACTER_BACK {
	unsigned char size;
	unsigned type;
	int clientid;
	float x, y, z;
	PlayerType p_type;
};
struct CS_SELECT_WEAPO
{
	unsigned char size;
	unsigned type;
	int id;
	bool bselectwep;
	WeaponType weapon_type;
};
struct SC_SYNC_WEAPO
{
	unsigned char size;
	unsigned type;
	int id;
	bool bselectwep;
	WeaponType weapon_type;
};
struct CS_READY_PACKET {
	unsigned char size;
	unsigned type;
};
struct SC_ACCEPT_READY {
	unsigned char size;
	unsigned type;
	bool ingame;
};
struct CS_ATTACK_PLAYER {
	unsigned char size;
	unsigned char type;
	int attack_id;
	float sx, sy, sz;
	float ex, ey, ez;
};
struct SC_ATTACK_PLAYER {
	unsigned char size;
	unsigned char type;
	int clientid;
	float sx, sy, sz;
	float ex, ey, ez;
};
struct CS_EFFECT_PACKET {
	unsigned char size;
	unsigned char type;
	int attack_id;
	float lx, ly, lz;
	float r_pitch, r_yaw, r_roll;
	// 0 = ������, 1, ��ó
	int wep_type;			
};
struct CS_DAMAGE_PACKET {
	unsigned char size;
	unsigned char type;
	int damaged_id;
	float damage;
};
struct SC_DAMAGE_CHANGE
{
	unsigned char size;
	unsigned char type;
	int damaged_id;
	int damage;
};
struct CS_SIGNAL_PACKET
{
	unsigned char size;
	unsigned char type;
};
#pragma pack(pop)
#pragma pack (push, 1)
struct CS_SHOTGUN_BEAM_PACKET {
	unsigned char size;
	unsigned char type;
	int attackid;
	float sx, sy, sz;
	float pitch0, yaw0, roll0;
	float pitch1, yaw1, roll1;
	float pitch2, yaw2, roll2;
	float pitch3, yaw3, roll3;
	float pitch4, yaw4, roll4;
	float pitch5, yaw5, roll5;
	float pitch6, yaw6, roll6;
	float pitch7, yaw7, roll7;
	float pitch8, yaw8, roll8;
	
	//float ex9, ey9, ez9;
};
struct CS_SHOTGUN_DAMAGED_PACKET {
	unsigned char size;
	unsigned char type;
	int damaged_id;
	int damaged_id1;
	int damaged_id2;
	float damage;
	float damage1;
	float damage2;
};
struct SC_SHOTGUN_DAMAGED_CHANGE_PACKET {
	unsigned char size;
	unsigned char type;
	int damaged_id1;
	int damaged_id2;
	int damaged_id3;
	float newhp1;
	float newhp2;
	float newhp3;
};
struct CS_NIAGARA_SYNC_PACKET {
	unsigned char size;
	unsigned char type;
	int id;
	PlayerType playertype;
};
#pragma pack(pop)








//
//struct cs_packet_start { // ���� ���� ��û
//	unsigned char size;
//	char	type;
//	bool	ready;
//};
//struct sc_packet_ready { // Ÿ �÷��̾� ����
//	unsigned char size;
//	char	type;
//	char	name[MAX_NAME_SIZE];
//};
//
//struct sc_packet_start_ok { // ����
//	unsigned char size;
//	char type;
//	char	name[MAX_NAME_SIZE];
//	float x, y, z;
//	char image_num;
//};
//
//struct cs_packet_attack {
//	unsigned char size;
//	char	type;
//	int s_id;
//};
//
//struct cs_packet_damage {
//	unsigned char size;
//	char	type;
//};
//
//struct cs_packet_get_item {
//	unsigned char size;
//	char	type;
//	int s_id;
//	char    item_num;
//};
//
//struct cs_packet_chat {
//	unsigned char size;
//	char	type;
//	int s_id;
//	float x, y, z;
//	char	message[MAX_CHAT_SIZE];
//};
//
//struct cs_packet_teleport {
//	// �������� ��ֹ��� ���� ���� ��ǥ�� �ڷ���Ʈ ��Ų��.
//	// ���� Ŭ���̾�Ʈ���� ���� �׽�Ʈ������ ���.
//	unsigned char size;
//	char	type;
//};
//
//struct sc_packet_remove_object {
//	unsigned char size;
//	char type;
//	int id;
//};
//
//struct sc_packet_chat {
//	unsigned char size;
//	char type;
//	int id;
//	char message[MAX_CHAT_SIZE];
//};
//
//struct sc_packet_login_fail {
//	unsigned char size;
//	char type;
//	int	 reason;		// 0: �ߺ� ID,  1:����� Full
//};
//
//struct sc_packet_status_change {
//	unsigned char size;
//	char type;
//	short   state;
//	short	hp, maxhp;
//	bool ice[4]; // ��������
//};
//
//
//struct sc_packet_hp_change {
//	unsigned char size;
//	char type;
//	int target;
//	int	hp;
//};