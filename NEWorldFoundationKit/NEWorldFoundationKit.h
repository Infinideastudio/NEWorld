// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� NEWORLDFOUNDATIONKIT_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// NEWORLDFOUNDATIONKIT_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
#ifdef NEWORLDFOUNDATIONKIT_EXPORTS
#define NEWORLDFOUNDATIONKIT_API __declspec(dllexport)
#else
#define NEWORLDFOUNDATIONKIT_API __declspec(dllimport)
#endif

// �����Ǵ� NEWorldFoundationKit.dll ������
class NEWORLDFOUNDATIONKIT_API CNEWorldFoundationKit {
public:
	CNEWorldFoundationKit(void);
	// TODO:  �ڴ�������ķ�����
};

extern NEWORLDFOUNDATIONKIT_API int nNEWorldFoundationKit;

NEWORLDFOUNDATIONKIT_API int fnNEWorldFoundationKit(void);
