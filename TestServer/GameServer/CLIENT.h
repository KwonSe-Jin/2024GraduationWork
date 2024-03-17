#pragma once
#include "pch.h"
#include "CorePch.h"
#include "Overlap.h"
class CLIENT
{
public:
    int _s_id; //�÷��̾� �迭 �ѹ�
    char name[MAX_NAME_SIZE]; //�÷��̾� nick
    char _id[MAX_NAME_SIZE]; // id
    char _pw[MAX_NAME_SIZE];  // pw
        // ��ġ
    float	x;
    float	y;
    float	z;
    // ȸ����
    float	Yaw;
    float	Pitch;
    float	Roll;
    float   Speed;
    // �ӵ�
    float VX;
    float VY;
    float VZ;
    float Max_Speed;
   
    bool is_bone = false;



    unordered_set   <int>  viewlist; // �þ� �� ������Ʈ
    mutex vl;
    mutex hp_lock;

    mutex state_lock;
    CL_STATE _state;
    atomic_bool   _is_active = false;

    atomic_int    _count;
    int      _type;   // 1.Player 2.���  3.���� 4.�巡��(Boss)   

    Overlap _recv_over;
    SOCKET  _socket;
    int      _prev_size;
    int      last_move_time;
public:
    CLIENT() : _state(ST_FREE), _prev_size(0)
    {
        x = 0;
        y = 0;
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



};

