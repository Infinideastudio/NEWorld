// NEWorldFoundationKit.cpp : ���� DLL Ӧ�ó���ĵ���������
//

#include "stdafx.h"
#include "NEWorldFoundationKit.h"


// ���ǵ���������һ��ʾ��
NEWORLDFOUNDATIONKIT_API int nNEWorldFoundationKit=0;

// ���ǵ���������һ��ʾ����
NEWORLDFOUNDATIONKIT_API int fnNEWorldFoundationKit(void)
{
    return 42;
}

// �����ѵ�����Ĺ��캯����
// �й��ඨ�����Ϣ������� NEWorldFoundationKit.h
CNEWorldFoundationKit::CNEWorldFoundationKit()
{
    return;
}
