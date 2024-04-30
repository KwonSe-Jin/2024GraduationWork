#pragma once
#include "pch.h"
#include "CorePch.h"
#include "protocol.h"
#include "Overlap.h"
class CLIENT
{
public:
    int _s_id; //�÷��̾� �迭 �ѹ�
    char name[MAX_NAME_SIZE]; //�÷��̾� nick
    char _pw[MAX_NAME_SIZE];  // pw
    // ��ġ
    float	x;
    float	y;
    float	z;
    // ȸ����
    // ������ ȹ�� ī��Ʈ
    int myItemCount;
    bool connected = false;
    bool selectweapon = false;
    float	Yaw;
    float	Pitch;
    float	Roll;
    float   Speed;
    // �ӵ�
    float VX;
    float VY;
    float VZ;
    float Max_Speed;
    int _max_hp; // �ִ� ü��
    int _hp; // ü��
    int damage;
    int _atk; // ���ݷ�
    int _def;
    bool is_bone = false;
    int32 iMaxSnowballCount;
    int32 iCurrentSnowballCount;
    int32 iCurrentMatchCount;
    bool recvdamage;
    bool bHasUmbrella;
    bool bHasBag;
    bool bCancel;
    bool bEndGame = false;
    WeaponType w_type;
    PlayerType p_type;
    float s_x, s_y, s_z;
    float e_x, e_y, e_z;
    int wtype;
    //--------------------
    bool bStopAnim;
    unordered_set   <int>  viewlist; // �þ� �� ������Ʈ
    mutex vl;
    mutex hp_lock;
    mutex lua_lock;

    mutex state_lock;
    CL_STATE _state;
    atomic_bool  _is_active = false;

    //COMBAT _combat;
    int num;
    atomic_int  _count;
    int      _type;
    //-------------
    float pitch0, yaw0, roll0;
    float pitch1, yaw1, roll1;
    float pitch2, yaw2, roll2;
    float pitch3, yaw3, roll3;
    float pitch4, yaw4, roll4;
    float pitch5, yaw5, roll5;
    float pitch6, yaw6, roll6;
    float pitch7, yaw7, roll7;
    float pitch8, yaw8, roll8;
    //-------------
    Overlap _recv_over;
    SOCKET  _socket;
    int      _prev_size;
    int      last_move_time;
public:
    CLIENT() : _state(ST_FREE), _prev_size(0)
    {
        _hp = 100;
        myItemCount = 0;
    }



    ~CLIENT()
    {
        closesocket(_socket);
    }

    void do_recv()
    {
        DWORD recv_flag = 0;
        ZeroMemory(&_recv_over._wsa_over, sizeof(_recv_over._wsa_over));
        _recv_over._wsa_buf.buf = reinterpret_cast<char*>(_recv_over._net_buf + _prev_size);
        _recv_over._wsa_buf.len = sizeof(_recv_over._net_buf) - _prev_size;
        int ret = WSARecv(_socket, &_recv_over._wsa_buf, 1, 0, &recv_flag, &_recv_over._wsa_over, NULL);
        if (SOCKET_ERROR == ret) {
            int error_num = WSAGetLastError();
            if (ERROR_IO_PENDING != error_num)
                error_display(error_num);
        }
    }

    void do_send(int num_bytes, void* mess)
    {
        Overlap* ex_over = new Overlap(IO_SEND, num_bytes, mess);
        int ret = WSASend(_socket, &ex_over->_wsa_buf, 1, 0, 0, &ex_over->_wsa_over, NULL);
        if (SOCKET_ERROR == ret) {
            int error_num = WSAGetLastError();
            if (ERROR_IO_PENDING != error_num)
                error_display(error_num);
        }
    }

    void Item_Ability(int _iAtt, int _iHp);
    void UnEquip_Item(int _iIdx);

};

