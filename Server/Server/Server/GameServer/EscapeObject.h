#pragma once
#include "CorePch.h"

class EscapeObject
{
public:
    float x, y, z;
    float pitch, yaw, roll;
    int ob_id;

public:
    EscapeObject() {}
    ~EscapeObject() {}

    void setPosition(float x_val, float y_val, float z_val) {
        x = x_val;
        y = y_val;
        z = z_val;
    }

    void setRandomPosition(std::mt19937& gen, std::uniform_real_distribution<float>& dis, std::uniform_real_distribution<float>& disz) {
        x = dis(gen);
        y = dis(gen);
        z = disz(gen);
    }

    void removeOBJ() {
        //�����Ǿ��� �� �迭�� �������� �������� ������ ������ ������ ���鿡�� ����ȭ
    }
};

