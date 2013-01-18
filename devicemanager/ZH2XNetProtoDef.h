/* ZH2XNetProtoDef.h - ZH-2X/ZH-3X TCP/IP network communication protocol
                       between Device and Manager/SubStation/MainStation
               ZH-2X��ZH-3X ͨ��Э�鶨��
	1)��Э�������еĽṹ�嶼�ǵ��ֽڶ���ģ�
	2)��Э�������е��������Ͷ���С���ֽ���ģ�
*/
#ifndef __ZH2X_NETWORK_PROTOCOL_DEFINE_6328DDF5_2F9A_4346_9907_11753C1CCE0F_H__
#define __ZH2X_NETWORK_PROTOCOL_DEFINE_6328DDF5_2F9A_4346_9907_11753C1CCE0F_H__

#include <time.h>
#include <string.h>

//#include "datetime.h"

//#include <push_pack1.h>

#define ZHREC_PORT_TCPTOREC	0x764A//30282, TCPͨ��(¼���������˿�)
#define ZHREC_PORT_UDPTOREC	0x7643//30275, UDPͨ��(to ZHREC)(¼���������˿�)

//��������
//typedef unsigned __int64	UINT64;
typedef unsigned int		UINT32;
typedef unsigned short		UINT16;
typedef unsigned char		UINT8;

//typedef __int64			INT64;
typedef int				INT32;
typedef short			INT16;
typedef char			INT8;

enum
{
	PACKETMAX		=16384,
	PACKET_DATA_MAX	=(PACKETMAX-8),

	ZHPACKET_SIZE	=4096,	// Ĭ�Ϸ��������ݵ����ݳ���
	ZHPACKET_STX	=0x76337692,// ��ǿ�ͱ��ĵ���ʼ�ַ�
	ZHPACKET_ETX	=0x76927633,// ��ǿ�ͱ��ĵĽ����ַ�
};

enum
{
// ���������е�����
	PTYPE_TICK			=0x00000000,	// ��������
	PTYPE_RETURN		=0x00000001,	// ����һ���������Ӧ(PACKET_RETURN)
	PTYPE_TIME			=0x00000002,	// �޸�װ�ñ���ʱ��(SYSTEM_TIME)
	PTYPE_PACKETSIZE	=0x00000003,	// ���������ݴ���ʱ�ı������ݴ�С(��������ͷ��248-PACKET_DATA_MAX,0xffff��ʾ����ǰ�İ���С)
	PTYPE_FRAMEDELAY	=0x00000004,	// ���÷��������(0ms-30000ms,0xffff��ʾ����ǰ�ķ��������)
	PTYPE_GETTIME		=0x00000007,	// ����װ�ñ���ʱ��(DATE_TIME)

	// �ļ���Ŀ¼����������������У�
	PTYPE_FILE_INFO		=0x00000010,	// �����ļ�����Ϣ(FILE_INFO)
	PTYPE_FILE_DATA		=0x00000011,	// �����ļ�������(����)

	PTYPE_FILE_GET		=0x00000012,	// Ҫ��Է������ļ�(FILE_GET)

	PTYPE_CANCEL		=0x0000006E,	// Ҫ��ֹͣ��ǰ�Ĵ������(PACKET_RETURN)

// ���ӷ��������

// ���Ʒ��������
	PTYPE_GETNEWSTATUS	=0x00000300,	//�õ��������״̬(�Ƿ����±仯)
	PTYPE_RESETNEWSTATUS=0x00000301,	//��λ�������״̬
	PTYPE_SETNEWSTATUS	=0x00000302,	//���÷������״̬
	PTYPE_GETYC			=0x00000303,	//����ң��
	PTYPE_GETYX			=0x00000304,	//����ң��
	PTYPE_GETYC21		=0x00000305,	//����ң�⵼����

// �豸�Զ���
	PTYPE_CHKFRMFMT		=0x76337692,	//���֡��ʽ

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_RTW		=0x01000004,	//ʵʱ��������(TCP: ZHREC to COMM)
	PTYPE_RTW_080826=0x01000100,	//ʵʱ��������(080826������½ṹ)

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_CFGUPDATE	=0x01000008,	// ���ø���
		UPDTYPE_CON	=0x01000009,	// �����ļ����ı�
		UPDTYPE_DZ	=0x0100000A,	// ��ֵ�ļ����ı�

		UPDTYPE_RXX	=0x0100000B,	// ����
		UPDTYPE_INI	=0x0100000C,	// ��ʼ���ļ����ı�

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_CENTERINFO	=0x0100000D,// ����վ��¼��¼����
	PTYPE_CENTERDATA	=0x0100000E,// ����վע��ʵʱ������Ϣ
	PTYPE_RECORDERINFO	=0x01000010,// ����¼������Ϣ(CONFIG_HEAD)

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_GETMODEL		=0x01000024,//�õ��豸�ͺ�(PACKET_MODEL)

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_GETSELCHNLDATA=0x01000025,//����ѡ���ͨ����������
	PTYPE_GETRUNSTATUS	=0x01000028,//����װ�����й���
	PTYPE_RESETDSP		=0x01000029,//��λǰ�û�
	PTYPE_DISABLERECORD	=0x0100002A,//��ֹ����Խ���о�����¼��
	PTYPE_ENABLERECORD	=0x0100002B,//��������Խ���о�����¼��
	PTYPE_GET_MVER		=0x0100002C,//����ģ��汾��Ϣ
		UPDTYPE_NET		=0x0100002D,//�����������
	PTYPE_REBOOT		=0x0100002E,//��������

//////////////////////////////////////////////////////////////////
	PTYPE_CSS_QUERY		=0x01001000,//��̬��¼��ѯ(����CSS_QUERY_FILTER,����CSS_QUERY_RESULT*N)
	PTYPE_CSS_GETREC	=0x01001001,//������̬��¼����(����CSS_GET_REC,����zyd�ļ�)
	PTYPE_GETALARM		=0x01001002,//����澯�¼�
};

/* ���ĸ�ʽ��
1)ZH-2X/ZH-3X ��ǿ�ͱ��ĸ�ʽ(������TCPͨ��)��
1.1)ZHPACKET_STX��2, 8 bytes
1.2)PACKET_HEADER, 8 bytes
1.3)data, PACKET_HEADER.length bytes
1.4)checksum, 4 bytes, ���ֽ�У���
1.5)ZHPACKET_ETX��2, 8 bytes

2)ZH-2X/ZH-3X ��ͨ���ĸ�ʽ(����TCP/UDPͨ��)��
2.1)PACKET_HEADER, 8 bytes
2.2)data, PACKET_HEADER.length bytes
*/

// ���ݰ�ͷ(����TCP/UDPͨ��)
typedef struct PACKET_HEADER
{
	UINT32	type;
	UINT32	length;	// ���������ṹ��ĳ���
//--------------------------------------------------------------------------
//	inline char* data() { return (char*)this + sizeof(PACKET_HEADER); }
}PACKET_HEADER;//size=8

// ��չ����ͷ(������TCPͨ��)
struct EXTPACKET_HEADER
{
	UINT32    stx1;	// ZHPACKET_STX
	UINT32    stx2;	// ZHPACKET_STX
	struct PACKET_HEADER	hdr;	// �����￪ʼ����checksum
}EXTPACKET_ATTRIB_PACKED;//size=8

struct EXTPACKET_TAIL
{
	UINT32    sum;	// ���ֽ�У���
	UINT32    etx1;	// ZHPACKET_ETX
	UINT32    etx2;	// ZHPACKET_ETX
}EXTPACKET_TAIL_ATTRIB_PACKED;//size=8

////////////////////////////////////////////////
/*
struct SYSTEM_TIME
{
	UINT16 year;
	UINT16 month;
	UINT16 wday;
	UINT16 mday;
	UINT16 hour;
	UINT16 minute;
	UINT16 second;
	UINT16 ms;
}ATTRIB_PACKED;//size=16

struct FILE_INFO
{
	UINT32	dwFrom;			//��Դ
	DATE_TIME		fileTime;		//�ļ�ʱ��
	UINT32	fileSize;		//�ļ���С
	UINT32	fileAttr;		//�ļ�����
	UINT32	fileOffset;		//���͵��ļ�ƫ��
	UINT32	fileLength;		//���͵��ļ�����
	char			fileName[256];	//�ļ���
//--------------------------------------------------------------------------
	inline int BSize()const
	{
		return (sizeof(FILE_INFO)-sizeof(fileName)+strlen(fileName)+1);
	}
	static inline int MinSize()
	{
		return (sizeof(FILE_INFO)-sizeof(((FILE_INFO*)0)->fileName)+1);
	}
}ATTRIB_PACKED;//size=292

//����FILE_GET��(PTYPE_FILE_GET),������PTYPE_FILE_DATA��
struct FILE_GET
{
	UINT16	wFrom;			// ��Դ
	UINT16	wFlag;			// ���
	UINT32	dwFileOffset;	// Ҫ��Է����ļ���dwFileOffset����ʼ����
	int		fid;					// ���ݿ�������ID(dwFrom=FILE_FROM_DBʱ��Ч)
	char	fileName[256];	// �ļ���(dwFrom=FILE_FROM_UNIT��FILE_FROM_DISKʱ��Ч)
//--------------------------------------------------------------------------
	inline int BSize()const;
	static inline int MiniSize();
}ATTRIB_PACKED;//size=268
inline int FILE_GET::BSize()const
{
	return (sizeof(FILE_GET)-sizeof(fileName)+strlen(fileName)+1);
}
inline int FILE_GET::MiniSize()
{
	return sizeof(FILE_GET)-sizeof(((FILE_GET*)0)->fileName)+1;
}

typedef enum
{
	FILE_FLAG_ZIP		=0x0001,
	FILE_FLAG_DEL		=0x0002,//������ɺ�ɾ���ļ�
	FILE_FLAG_BCT		=0x0004,//�ϵ�����
};
*/ 
struct PACKET_RETURN
{	UINT32	nRetCode;
	UINT32	dwSender;
	char	szMessage[256];//������Ϣ
//--------------------------------------------------------------------------
//	inline int BSize()const;
}PACKET_RETURN;//size=264
/*
typedef enum
{
	SENDER_UNKOWN	=0,
	SENDER_RECORDER,
	SENDER_BRANCH,
	SENDER_CENTER,
};

inline int PACKET_RETURN::BSize()const
{
	return (sizeof(PACKET_RETURN)-sizeof(szMessage)+strlen(szMessage)+1);
}
*/ 
//PTYPE_GETNEWSTATUS, PTYPE_RESETNEWSTATUS, PTYPE_SETNEWSTATUS����
struct PACKET_NEWSTATUS
{
	UINT32 	status;
}PACKET_NEWSTATUS;
enum
{
	NEW_STATUS_REC	=0x00000001,	//��¼��
	NEW_STATUS_DZ	=0x00000002,	//�¶�ֵ
	NEW_STATUS_CON	=0x00000004,	//������
	NEW_STATUS_ALARM=0x00000020,	//�¸澯�¼�
};

//����վ˽������
typedef struct CENTER_DATA
{
	UINT32	nFlag;		//���
	UINT32	nAddr;		//��ַ
	char			szUser[64];	//�û�
	unsigned char	nChnl;		//����ͨ����
	unsigned char	Chnls[255];	//ͨ������
}CENTER_DATA;//size=328

struct ZH_CONFIG_HEAD
{
	char	FlagString[16];		/*16*/	/*��־��:"Wyy&FwgDefine", "$ZH2FSCF$"*/
	char	VersionString[8];	/* 8*/	/*�汾��:"3.00"*/
	char	StationName[40];	/*40*/	/*��վ����*/
	char	StationNo[16];		/*16*/	/*��վ���*/
	char	RecorderName[40];	/*40*/	/*¼��������*/
	char	ModelInfo[64];		/*64*/	/*ģ����Ϣ*/
	char	AppVersion[16];		/*16*/	/*Ӧ�ó���汾*/
	char	r1[32];
}ZH_CONFIG_HEAD;//size=232

struct PACKET_MODEL
{
	UINT32	nModel;
	char	szModel[16];	// ��ʾ���ͺţ�����:ZH-3
	char	szSummary[64];	// ��ʾ���ͺ�����������:Ƕ��ʽ��������¼������װ��
};

//װ������״̬��Ϣ
//struct PACKET_RUNSTATUS
//{
//	UINT16	msk[16];
//	UINT16	sta[16];
//};//size=64

enum
{
	// msk[0], sta[0]
	STA0BIT_ADSP0 = 0x0001,	// ADSP 0��msk: 1=����; sta: 1=����
	STA0BIT_ADSP1 = 0x0002,	// ADSP 1
	STA0BIT_ADSP2 = 0x0004,	// ADSP 2
	STA0BIT_ADSP3 = 0x0008,	// ADSP 3
	STA0BIT_ADSP4 = 0x0010,	// ADSP 4
	STA0BIT_ADSP5 = 0x0020,	// ADSP 5

	// msk[1], sta[1], �����ڶ�DSPϵͳ
	STA1BIT_DDSP0 = 0x0001,	// DDSP 0��msk: 1=����; sta: 1=����
	STA1BIT_DDSP1 = 0x0002,	// DDSP 1
	STA1BIT_DDSP2 = 0x0004,	// DDSP 2
	STA1BIT_DDSP3 = 0x0008,	// DDSP 3
	STA1BIT_DDSP4 = 0x0010,	// DDSP 4
	STA1BIT_DDSP5 = 0x0020,	// DDSP 5

	// msk[2], sta[2]������Ƶ�����
	STA2BIT_Z9V		=0x0001, // DC+9V����/����
	STA2BIT_F9V		=0x0002, // DC-9V����/����
	STA2BIT_Z24V	=0x0004, // DC24V����/����
	STA2BIT_REC		=0x0008, // ����¼��
	STA2BIT_TST		=0x0010, // ������״̬
	STA2BIT_CF		=0x0020, // CF����msk: 1=����; sta: 1=����
	STA2BIT_HD0		=0x0040, // HD0��msk: 1=����; sta: 1=����
	STA2BIT_HD1		=0x0080, // HD1��msk: 1=����; sta: 1=����
	STA2BIT_USB		=0x0100, // USB����, msk: 1=������һ��USB����; sta: δ����

	// msk[3], sta[3] ��ˮ���ܵ�վ����Ƶ�����
	STA3BIT_TJ		=0x0001, // ͣ��
	STA3BIT_FD		=0x0002, // ����
	STA3BIT_FDTX	=0x0004, // �������
	STA3BIT_CS		=0x0008, // ��ˮ
	STA3BIT_CSTX	=0x0010, // ��ˮ����

	// msk[4], sta[4]
	STA4BIT_ADSP0 = 0x0001,	// ADSP 0����¼��
	STA4BIT_ADSP1 = 0x0002,	// ADSP 1����¼��
	STA4BIT_ADSP2 = 0x0004,	// ADSP 2����¼��
	STA4BIT_ADSP3 = 0x0008,	// ADSP 3����¼��
	STA4BIT_ADSP4 = 0x0010,	// ADSP 4����¼��
	STA4BIT_ADSP5 = 0x0020,	// ADSP 5����¼��

	// msk[5], sta[5], �����ڶ�DSPϵͳ
	STA5BIT_DDSP0 = 0x0001,	// DDSP 0����¼��
	STA5BIT_DDSP1 = 0x0002,	// DDSP 1z����¼��
	STA5BIT_DDSP2 = 0x0004,	// DDSP 2����¼��
	STA5BIT_DDSP3 = 0x0008,	// DDSP 3����¼��
	STA5BIT_DDSP4 = 0x0010,	// DDSP 4����¼��
	STA5BIT_DDSP5 = 0x0020,	// DDSP 5����¼��

	// msk[6], sta[6], ZH-5����
	STA6BIT_DSPFLASH = 0x0001,	// DSP ���ݴ洢���쳣

	// msk[x], sta[x]�� ����
};

struct MODULE_VER_INFO
{
	char module[32];	//ģ����
	char ver[16];		//�汾��
	char descr[80];		//����������Ϣ(��ѡ)
}MODULE_VER_INFO;

// ****************************************************************
// ����ʵʱ���εı���
//-----------------------------------------------------
//ʵʱ�������ݰ�(080826����)
struct RTCHNLDATAS_080826
{
	INT32	idx;	//ͨ������
	INT32	nUse;	//ʹ�õĸ���
	INT32	lr1[2];	//����

	INT32	dalta;	//��Ư
	INT32	dalta0;	//ͨ����ָ���Ĺ�����Ư
	INT32	lr2[2];	//����

	float	fcval;	//S2����ϵ��(S2=S*fcval)
	float	fr1;	//����

	//����ĵ�ǰֵ
	float	fR;		//ʵ��
	float	fX;		//�鲿

	union
	{
		float fvals[8];
		struct
		{
			float	fvir;	//��Чֵ
			float	fval0;	//��0��˲ʱֵ
			float	fvmax;	//���ֵ
			float	angle;	//���
			float	fvir0;	//ԭʼ��Чֵ(û�г�kkk)
		};
	};

	//����ֵ(ȥ��Ư֮��ģ�����)
	union
	{
		INT32	nVal[100];
		UINT32	wVal[100];
	};
//--------------------------------------------------------------------------
//	static inline int BSize() { return sizeof(RTCHNLDATAS_080826); } // 480
}RTCHNLDATAS_080826;

#define RTW_FREQ_NUM	16

typedef struct RTWDATAS_080826
{
	char	stx[16];		//��ʼ���, RTWDATAS_080826_STX
	UINT32	t;				//ʱ��
	UINT32	flag;			//���
	UINT32	RtwRate;		//ʵʱ���β�����
	UINT32	lr1[5];			//����
	float	fFreq[RTW_FREQ_NUM];	//������Ԫ��Ӳ������Ƶ��
	UINT32	nPPW[RTW_FREQ_NUM];		//������Ԫÿ�ܲ�����
	UINT32	lr2[16];				//����
	struct RTCHNLDATAS_080826	Chnls[1];	//ͨ������
//--------------------------------------------------------------------------
//	static inline int HdrSize()               { return sizeof(RTWDATAS_080826)-RTCHNLDATAS_080826::BSize(); }
//	RTCHNLDATAS_080826* Channel(UINT32 idx) { return (RTCHNLDATAS_080826*)((char*)this+HdrSize()+idx*RTCHNLDATAS_080826::BSize()); }
//	inline UINT RtwRateGet()const { return (RtwRate<800U) ? 1000U : RtwRate;}
}RTWDATAS_080826;

#define RTWDATAS_080826_STX				"RTWDATAS_080826"
enum
{
	RTWDATAS_080826_FLAG_CS=0x00000001,	//��ˮ״̬
};

#if CPU_FAMILY==I80X86
typedef struct RTCHNLDATAS_ZH2X	// *** PowerPC�²��ܵ��ֽڶ��� ***
{
	INT16	idx;	//0 ͨ������*
	INT16	nUse;	//2 ʹ�õĸ���
	double	fcval;	//4 S2����ϵ��(S2=S*fcval)*
	double	fvir;	//12 ��Чֵ
	double	fval0;	//20 ��0��˲ʱֵ
	double	fvmax;	//28 ����*
	double	angle;	//36 ���
	double	fvir0;	//44 ԭʼ��Чֵ(û�г�kkk)
	int		dalta;	//52 ��Ư
	int		dalta0;	//56 ͨ����ָ���Ĺ�����Ư
	union
	{
		int		nVal[100];//60 ����ֵ
		UINT32	wVal[100];
	};
	double	fR;		//460 ʵ��
	double	fX;		//468 �鲿
//--------------------------------------------------------------------------
//	static inline int BSize() { return 476; }
//	inline char* Buff(UINT32 offset) { return (char*)this+offset; }
//	inline RTCHNLDATAS_ZH2X* Next()			{ return (RTCHNLDATAS_ZH2X*)((char*)this+BSize()); }
}RTCHNLDATAS_ZH2X;//size=476	 0x1dc

// ZH-3X ʵʱ���ݽṹ
typedef struct RTWDATAS_ZH2X	// *** PowerPC�²��ܵ��ֽڶ��� ***
{
	UINT32	t;		//0
	unsigned char	bCS;	//4 �Ƿ����ڳ�ˮ
	double		fFreq[16];	//5 Ƶ��
	INT16		nPPW[16];	//133 ÿ�ܲ�����(Points Per Wave)
	struct RTCHNLDATAS_ZH2X	Chnls[1];	//165 ͨ������ 0xa5
//--------------------------------------------------------------------------
//	static inline int HdrSize()               { return 4+1+8*16+2*16; } // 165
//	inline char* HdrBuff(UINT32 offset) { return (char*)this+offset; }
//	RTCHNLDATAS_ZH2X* Channel(UINT32 idx) { return (RTCHNLDATAS_ZH2X*)((char*)this+HdrSize()+idx*RTCHNLDATAS_ZH2X::BSize()); }
}RTWDATAS_ZH2X;
#endif //CPU_FAMILY==I80X86

// ****************************************************************
// ң�⡢ң�š���չң��
struct ZHREC_ASDU10_YC	//ң��, // *** PowerPC�²��ܵ��ֽڶ��� ***
{
	unsigned char	gno;//0 ����(20)*
	unsigned char	ino;//1 ��Ŀ��(1��80)*
	float	fv;			//2 ң��ֵ(��׼ֵ)
	float	fa2;		//6 ���β໻��ϵ��A(���β�ֵ=fv*fa2)*
	float	fa1;		//10 һ�β໻��б��A(һ�β�ֵ=fv*fa1+fb1)*
	float	fb1;		//14 һ�β໻��ؾ�B*
	char	name[48];	//18 ����*
	char	unit1[8];	//66 һ�β�����*
	char	unit2[8];	//74 ���β�����*
//--------------------------------------------------------------------------
//	static inline int BSize()      { return 2+4*4+48+16; } // 82
//	inline char* Buff(int offset)  { return (char*)this+offset; }
//	inline ZHREC_ASDU10_YC* Next() { return (ZHREC_ASDU10_YC*)((char*)this+BSize()); }
}ZHREC_ASDU10_YC;//size=82

struct ZHREC_ASDU10_YX//ң��
{
	unsigned char	gno;//����(30)*
	unsigned char	ino;//��Ŀ��(1��160)*
	unsigned char	dpi;//ң��ֵ(��ǰֵ)
	unsigned char	dpi0;//ң��ֵ(��һ��ֵ)
	char	name[48];	//����
}ZHREC_ASDU10_YX;//size=52

enum
{
	//1. ͨ�Ŵ�����
	ECOMM_NONE	=0,		//�޴���
	ECOMM_USER	=70000,	//�û��Զ��������
	ECOMM_FILEREAD		=(ECOMM_USER+3),	//���ļ�����
	ECOMM_STIMEOUT		=(ECOMM_USER+4),	//ͨ�ŷ������ݳ�ʱ
	ECOMM_RTIMEOUT		=(ECOMM_USER+5),	//ͨ�Ž������ݳ�ʱ

	ECOMM_FILEWRITE		=(ECOMM_USER+7),	//д�ļ�����
	ECOMM_FILENOTFIND	=(ECOMM_USER+12),	//�ļ���Ŀ¼������
	ECOMM_CHECKSUM		=(ECOMM_USER+16),	//��������У���
	ECOMM_CREATESOCKET	=(ECOMM_USER+23),	//���������׽���ʧ��
	ECOMM_FIRST	=(ECOMM_USER+1000),			//�û��Զ���ͨ�Ŵ�����
	ECOMM_REMOTECLOSE	=(ECOMM_FIRST+0),	//Զ���Ѿ��ر�
	ECOMM_CONNTOOMANY	=(ECOMM_FIRST+1),	//����̫��
	ECOMM_NOACK			=(ECOMM_FIRST+2),	//Զ��û��Ӧ��
	ECOMM_PTYPE			=(ECOMM_FIRST+3),	//���ݰ����ʹ���
	ECOMM_REMOTECANCEL	=(ECOMM_FIRST+4),	//Զ��ȡ���˵�ǰ����
	ECOMM_LOCALCANCEL	=(ECOMM_FIRST+5),	//����ȡ���˵�ǰ����
	ECOMM_TRANSBLOCK	=(ECOMM_FIRST+100),	//�豸ͨ�ű�����
	ECOMM_FILECREATE	=(ECOMM_FIRST+200),	//�����ļ�ʧ��
	ECOMM_FILEOPEN		=(ECOMM_FIRST+201),	//���ļ�ʧ��
	ECOMM_MEMORY		=(ECOMM_FIRST+202),	//�ڴ����ʧ��
	ECOMM_COMMAND		=(ECOMM_FIRST+301),	//����ϵͳ��֧��
	ECOMM_NODATA		=(ECOMM_FIRST+302),	//û�������������
	ECOMM_BUSY			=(ECOMM_FIRST+304),	//ϵͳ��æ
	ECOMM_PARAM			=(ECOMM_FIRST+305),	//��������
	ECOMM_FILESEEK		=(ECOMM_FIRST+306),	//�ƶ��ļ�ָ��ʧ��
};

//#include <pop_pack.h>

//Added by JAN 2009-9-14 
typedef struct START_PACKET{
	struct PACKET_HEADER header;
	CENTER_DATA data;
} START_PACKET;




#endif /*__ZH2X_NETWORK_PROTOCOL_DEFINE_6328DDF5_2F9A_4346_9907_11753C1CCE0F_H__ */
