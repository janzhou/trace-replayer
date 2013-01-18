/* ZH2XNetProtoDef.h - ZH-2X/ZH-3X TCP/IP network communication protocol
                       between Device and Manager/SubStation/MainStation
               ZH-2X、ZH-3X 通信协议定义
	1)本协议中所有的结构体都是单字节对齐的；
	2)本协议中所有的数据类型都是小端字节序的；
*/
#ifndef __ZH2X_NETWORK_PROTOCOL_DEFINE_6328DDF5_2F9A_4346_9907_11753C1CCE0F_H__
#define __ZH2X_NETWORK_PROTOCOL_DEFINE_6328DDF5_2F9A_4346_9907_11753C1CCE0F_H__

#include <time.h>
#include <string.h>

//#include "datetime.h"

//#include <push_pack1.h>

#define ZHREC_PORT_TCPTOREC	0x764A//30282, TCP通信(录波器监听端口)
#define ZHREC_PORT_UDPTOREC	0x7643//30275, UDP通信(to ZHREC)(录波器监听端口)

//数据类型
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

	ZHPACKET_SIZE	=4096,	// 默认发送流数据的数据长度
	ZHPACKET_STX	=0x76337692,// 增强型报文的起始字符
	ZHPACKET_ETX	=0x76927633,// 增强型报文的结束字符
};

enum
{
// 两个方向都有的命令
	PTYPE_TICK			=0x00000000,	// 心跳报文
	PTYPE_RETURN		=0x00000001,	// 对上一条命令的响应(PACKET_RETURN)
	PTYPE_TIME			=0x00000002,	// 修改装置本地时钟(SYSTEM_TIME)
	PTYPE_PACKETSIZE	=0x00000003,	// 设置流数据传输时的报文数据大小(不含报文头，248-PACKET_DATA_MAX,0xffff表示请求当前的包大小)
	PTYPE_FRAMEDELAY	=0x00000004,	// 设置发送祯间隔(0ms-30000ms,0xffff表示请求当前的发送祯间隔)
	PTYPE_GETTIME		=0x00000007,	// 请求装置本地时钟(DATE_TIME)

	// 文件、目录传输服务（两个方向都有）
	PTYPE_FILE_INFO		=0x00000010,	// 单个文件的信息(FILE_INFO)
	PTYPE_FILE_DATA		=0x00000011,	// 单个文件的数据(数据)

	PTYPE_FILE_GET		=0x00000012,	// 要求对方传送文件(FILE_GET)

	PTYPE_CANCEL		=0x0000006E,	// 要求停止当前的传输服务(PACKET_RETURN)

// 监视方向的命令

// 控制方向的命令
	PTYPE_GETNEWSTATUS	=0x00000300,	//得到服务的新状态(是否有新变化)
	PTYPE_RESETNEWSTATUS=0x00000301,	//复位服务的新状态
	PTYPE_SETNEWSTATUS	=0x00000302,	//设置服务的新状态
	PTYPE_GETYC			=0x00000303,	//请求遥测
	PTYPE_GETYX			=0x00000304,	//请求遥信
	PTYPE_GETYC21		=0x00000305,	//请求遥测导出量

// 设备自定义
	PTYPE_CHKFRMFMT		=0x76337692,	//检查帧格式

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_RTW		=0x01000004,	//实时波形数据(TCP: ZHREC to COMM)
	PTYPE_RTW_080826=0x01000100,	//实时波形数据(080826后定义的新结构)

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_CFGUPDATE	=0x01000008,	// 配置更新
		UPDTYPE_CON	=0x01000009,	// 配线文件被改变
		UPDTYPE_DZ	=0x0100000A,	// 定值文件被改变

		UPDTYPE_RXX	=0x0100000B,	// 保留
		UPDTYPE_INI	=0x0100000C,	// 初始化文件被改变

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_CENTERINFO	=0x0100000D,// 中心站登录到录波器
	PTYPE_CENTERDATA	=0x0100000E,// 中心站注册实时波形信息
	PTYPE_RECORDERINFO	=0x01000010,// 请求录波器信息(CONFIG_HEAD)

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_GETMODEL		=0x01000024,//得到设备型号(PACKET_MODEL)

///////////////////////////////////////////////////////////////////////////////////////
	PTYPE_GETSELCHNLDATA=0x01000025,//根据选择的通道请求数据
	PTYPE_GETRUNSTATUS	=0x01000028,//请求装置运行工况
	PTYPE_RESETDSP		=0x01000029,//复位前置机
	PTYPE_DISABLERECORD	=0x0100002A,//禁止所有越限判据启动录波
	PTYPE_ENABLERECORD	=0x0100002B,//允许所有越限判据启动录波
	PTYPE_GET_MVER		=0x0100002C,//请求模块版本信息
		UPDTYPE_NET		=0x0100002D,//更改网络参数
	PTYPE_REBOOT		=0x0100002E,//重新启动

//////////////////////////////////////////////////////////////////
	PTYPE_CSS_QUERY		=0x01001000,//稳态记录查询(发送CSS_QUERY_FILTER,返回CSS_QUERY_RESULT*N)
	PTYPE_CSS_GETREC	=0x01001001,//请求稳态记录波形(发送CSS_GET_REC,返回zyd文件)
	PTYPE_GETALARM		=0x01001002,//请求告警事件
};

/* 报文格式：
1)ZH-2X/ZH-3X 增强型报文格式(仅用于TCP通信)：
1.1)ZHPACKET_STX×2, 8 bytes
1.2)PACKET_HEADER, 8 bytes
1.3)data, PACKET_HEADER.length bytes
1.4)checksum, 4 bytes, 单字节校验和
1.5)ZHPACKET_ETX×2, 8 bytes

2)ZH-2X/ZH-3X 普通报文格式(用于TCP/UDP通信)：
2.1)PACKET_HEADER, 8 bytes
2.2)data, PACKET_HEADER.length bytes
*/

// 数据包头(用于TCP/UDP通信)
typedef struct PACKET_HEADER
{
	UINT32	type;
	UINT32	length;	// 不包括本结构体的长度
//--------------------------------------------------------------------------
//	inline char* data() { return (char*)this + sizeof(PACKET_HEADER); }
}PACKET_HEADER;//size=8

// 扩展报文头(仅用于TCP通信)
struct EXTPACKET_HEADER
{
	UINT32    stx1;	// ZHPACKET_STX
	UINT32    stx2;	// ZHPACKET_STX
	struct PACKET_HEADER	hdr;	// 从这里开始计算checksum
}EXTPACKET_ATTRIB_PACKED;//size=8

struct EXTPACKET_TAIL
{
	UINT32    sum;	// 单字节校验和
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
	UINT32	dwFrom;			//来源
	DATE_TIME		fileTime;		//文件时间
	UINT32	fileSize;		//文件大小
	UINT32	fileAttr;		//文件属性
	UINT32	fileOffset;		//发送的文件偏移
	UINT32	fileLength;		//发送的文件长度
	char			fileName[256];	//文件名
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

//发送FILE_GET包(PTYPE_FILE_GET),服务送PTYPE_FILE_DATA包
struct FILE_GET
{
	UINT16	wFrom;			// 来源
	UINT16	wFlag;			// 标记
	UINT32	dwFileOffset;	// 要求对方从文件的dwFileOffset处开始传送
	int		fid;					// 数据库中数据ID(dwFrom=FILE_FROM_DB时有效)
	char	fileName[256];	// 文件名(dwFrom=FILE_FROM_UNIT或FILE_FROM_DISK时有效)
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
	FILE_FLAG_DEL		=0x0002,//传输完成后删除文件
	FILE_FLAG_BCT		=0x0004,//断点续传
};
*/ 
struct PACKET_RETURN
{	UINT32	nRetCode;
	UINT32	dwSender;
	char	szMessage[256];//错误信息
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
//PTYPE_GETNEWSTATUS, PTYPE_RESETNEWSTATUS, PTYPE_SETNEWSTATUS报文
struct PACKET_NEWSTATUS
{
	UINT32 	status;
}PACKET_NEWSTATUS;
enum
{
	NEW_STATUS_REC	=0x00000001,	//新录波
	NEW_STATUS_DZ	=0x00000002,	//新定值
	NEW_STATUS_CON	=0x00000004,	//新配线
	NEW_STATUS_ALARM=0x00000020,	//新告警事件
};

//中心站私有数据
typedef struct CENTER_DATA
{
	UINT32	nFlag;		//标记
	UINT32	nAddr;		//地址
	char			szUser[64];	//用户
	unsigned char	nChnl;		//监测的通道数
	unsigned char	Chnls[255];	//通道索引
}CENTER_DATA;//size=328

struct ZH_CONFIG_HEAD
{
	char	FlagString[16];		/*16*/	/*标志串:"Wyy&FwgDefine", "$ZH2FSCF$"*/
	char	VersionString[8];	/* 8*/	/*版本串:"3.00"*/
	char	StationName[40];	/*40*/	/*厂站名称*/
	char	StationNo[16];		/*16*/	/*厂站编号*/
	char	RecorderName[40];	/*40*/	/*录波器名称*/
	char	ModelInfo[64];		/*64*/	/*模块信息*/
	char	AppVersion[16];		/*16*/	/*应用程序版本*/
	char	r1[32];
}ZH_CONFIG_HEAD;//size=232

struct PACKET_MODEL
{
	UINT32	nModel;
	char	szModel[16];	// 显示用型号，比如:ZH-3
	char	szSummary[64];	// 显示用型号描述，比如:嵌入式电力故障录波分析装置
};

//装置运行状态信息
//struct PACKET_RUNSTATUS
//{
//	UINT16	msk[16];
//	UINT16	sta[16];
//};//size=64

enum
{
	// msk[0], sta[0]
	STA0BIT_ADSP0 = 0x0001,	// ADSP 0，msk: 1=存在; sta: 1=故障
	STA0BIT_ADSP1 = 0x0002,	// ADSP 1
	STA0BIT_ADSP2 = 0x0004,	// ADSP 2
	STA0BIT_ADSP3 = 0x0008,	// ADSP 3
	STA0BIT_ADSP4 = 0x0010,	// ADSP 4
	STA0BIT_ADSP5 = 0x0020,	// ADSP 5

	// msk[1], sta[1], 适用于多DSP系统
	STA1BIT_DDSP0 = 0x0001,	// DDSP 0，msk: 1=存在; sta: 1=故障
	STA1BIT_DDSP1 = 0x0002,	// DDSP 1
	STA1BIT_DDSP2 = 0x0004,	// DDSP 2
	STA1BIT_DDSP3 = 0x0008,	// DDSP 3
	STA1BIT_DDSP4 = 0x0010,	// DDSP 4
	STA1BIT_DDSP5 = 0x0020,	// DDSP 5

	// msk[2], sta[2]，软件灯的设置
	STA2BIT_Z9V		=0x0001, // DC+9V存在/故障
	STA2BIT_F9V		=0x0002, // DC-9V存在/故障
	STA2BIT_Z24V	=0x0004, // DC24V存在/故障
	STA2BIT_REC		=0x0008, // 正在录波
	STA2BIT_TST		=0x0010, // 在试验状态
	STA2BIT_CF		=0x0020, // CF卡，msk: 1=存在; sta: 1=故障
	STA2BIT_HD0		=0x0040, // HD0，msk: 1=存在; sta: 1=故障
	STA2BIT_HD1		=0x0080, // HD1，msk: 1=存在; sta: 1=故障
	STA2BIT_USB		=0x0100, // USB磁盘, msk: 1=至少有一个USB磁盘; sta: 未定义

	// msk[3], sta[3] 抽水蓄能电站软件灯的设置
	STA3BIT_TJ		=0x0001, // 停机
	STA3BIT_FD		=0x0002, // 发电
	STA3BIT_FDTX	=0x0004, // 发电调相
	STA3BIT_CS		=0x0008, // 抽水
	STA3BIT_CSTX	=0x0010, // 抽水调相

	// msk[4], sta[4]
	STA4BIT_ADSP0 = 0x0001,	// ADSP 0正在录波
	STA4BIT_ADSP1 = 0x0002,	// ADSP 1正在录波
	STA4BIT_ADSP2 = 0x0004,	// ADSP 2正在录波
	STA4BIT_ADSP3 = 0x0008,	// ADSP 3正在录波
	STA4BIT_ADSP4 = 0x0010,	// ADSP 4正在录波
	STA4BIT_ADSP5 = 0x0020,	// ADSP 5正在录波

	// msk[5], sta[5], 适用于多DSP系统
	STA5BIT_DDSP0 = 0x0001,	// DDSP 0正在录波
	STA5BIT_DDSP1 = 0x0002,	// DDSP 1z正在录波
	STA5BIT_DDSP2 = 0x0004,	// DDSP 2正在录波
	STA5BIT_DDSP3 = 0x0008,	// DDSP 3正在录波
	STA5BIT_DDSP4 = 0x0010,	// DDSP 4正在录波
	STA5BIT_DDSP5 = 0x0020,	// DDSP 5正在录波

	// msk[6], sta[6], ZH-5增加
	STA6BIT_DSPFLASH = 0x0001,	// DSP 数据存储器异常

	// msk[x], sta[x]， 保留
};

struct MODULE_VER_INFO
{
	char module[32];	//模块名
	char ver[16];		//版本号
	char descr[80];		//其它描述信息(可选)
}MODULE_VER_INFO;

// ****************************************************************
// 上送实时波形的报文
//-----------------------------------------------------
//实时波形数据包(080826定义)
struct RTCHNLDATAS_080826
{
	INT32	idx;	//通道索引
	INT32	nUse;	//使用的个数
	INT32	lr1[2];	//保留

	INT32	dalta;	//零漂
	INT32	dalta0;	//通道中指定的固有零漂
	INT32	lr2[2];	//保留

	float	fcval;	//S2计算系数(S2=S*fcval)
	float	fr1;	//保留

	//计算的当前值
	float	fR;		//实部
	float	fX;		//虚部

	union
	{
		float fvals[8];
		struct
		{
			float	fvir;	//有效值
			float	fval0;	//第0点瞬时值
			float	fvmax;	//最大值
			float	angle;	//相角
			float	fvir0;	//原始有效值(没有乘kkk)
		};
	};

	//采样值(去零漂之后的！！！)
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
	char	stx[16];		//起始标记, RTWDATAS_080826_STX
	UINT32	t;				//时间
	UINT32	flag;			//标记
	UINT32	RtwRate;		//实时波形采样率
	UINT32	lr1[5];			//保留
	float	fFreq[RTW_FREQ_NUM];	//各个单元的硬件测量频率
	UINT32	nPPW[RTW_FREQ_NUM];		//各个单元每周波点数
	UINT32	lr2[16];				//保留
	struct RTCHNLDATAS_080826	Chnls[1];	//通道参数
//--------------------------------------------------------------------------
//	static inline int HdrSize()               { return sizeof(RTWDATAS_080826)-RTCHNLDATAS_080826::BSize(); }
//	RTCHNLDATAS_080826* Channel(UINT32 idx) { return (RTCHNLDATAS_080826*)((char*)this+HdrSize()+idx*RTCHNLDATAS_080826::BSize()); }
//	inline UINT RtwRateGet()const { return (RtwRate<800U) ? 1000U : RtwRate;}
}RTWDATAS_080826;

#define RTWDATAS_080826_STX				"RTWDATAS_080826"
enum
{
	RTWDATAS_080826_FLAG_CS=0x00000001,	//抽水状态
};

#if CPU_FAMILY==I80X86
typedef struct RTCHNLDATAS_ZH2X	// *** PowerPC下不能单字节对齐 ***
{
	INT16	idx;	//0 通道索引*
	INT16	nUse;	//2 使用的个数
	double	fcval;	//4 S2计算系数(S2=S*fcval)*
	double	fvir;	//12 有效值
	double	fval0;	//20 第0点瞬时值
	double	fvmax;	//28 量程*
	double	angle;	//36 相角
	double	fvir0;	//44 原始有效值(没有乘kkk)
	int		dalta;	//52 零漂
	int		dalta0;	//56 通道中指定的固有零漂
	union
	{
		int		nVal[100];//60 采样值
		UINT32	wVal[100];
	};
	double	fR;		//460 实部
	double	fX;		//468 虚部
//--------------------------------------------------------------------------
//	static inline int BSize() { return 476; }
//	inline char* Buff(UINT32 offset) { return (char*)this+offset; }
//	inline RTCHNLDATAS_ZH2X* Next()			{ return (RTCHNLDATAS_ZH2X*)((char*)this+BSize()); }
}RTCHNLDATAS_ZH2X;//size=476	 0x1dc

// ZH-3X 实时数据结构
typedef struct RTWDATAS_ZH2X	// *** PowerPC下不能单字节对齐 ***
{
	UINT32	t;		//0
	unsigned char	bCS;	//4 是否正在抽水
	double		fFreq[16];	//5 频率
	INT16		nPPW[16];	//133 每周波点数(Points Per Wave)
	struct RTCHNLDATAS_ZH2X	Chnls[1];	//165 通道参数 0xa5
//--------------------------------------------------------------------------
//	static inline int HdrSize()               { return 4+1+8*16+2*16; } // 165
//	inline char* HdrBuff(UINT32 offset) { return (char*)this+offset; }
//	RTCHNLDATAS_ZH2X* Channel(UINT32 idx) { return (RTCHNLDATAS_ZH2X*)((char*)this+HdrSize()+idx*RTCHNLDATAS_ZH2X::BSize()); }
}RTWDATAS_ZH2X;
#endif //CPU_FAMILY==I80X86

// ****************************************************************
// 遥测、遥信、扩展遥信
struct ZHREC_ASDU10_YC	//遥测, // *** PowerPC下不能单字节对齐 ***
{
	unsigned char	gno;//0 组编号(20)*
	unsigned char	ino;//1 条目号(1～80)*
	float	fv;			//2 遥测值(标准值)
	float	fa2;		//6 二次侧换算系数A(二次侧值=fv*fa2)*
	float	fa1;		//10 一次侧换算斜率A(一次侧值=fv*fa1+fb1)*
	float	fb1;		//14 一次侧换算截距B*
	char	name[48];	//18 描述*
	char	unit1[8];	//66 一次侧量纲*
	char	unit2[8];	//74 二次侧量纲*
//--------------------------------------------------------------------------
//	static inline int BSize()      { return 2+4*4+48+16; } // 82
//	inline char* Buff(int offset)  { return (char*)this+offset; }
//	inline ZHREC_ASDU10_YC* Next() { return (ZHREC_ASDU10_YC*)((char*)this+BSize()); }
}ZHREC_ASDU10_YC;//size=82

struct ZHREC_ASDU10_YX//遥信
{
	unsigned char	gno;//组编号(30)*
	unsigned char	ino;//条目号(1～160)*
	unsigned char	dpi;//遥信值(当前值)
	unsigned char	dpi0;//遥信值(上一次值)
	char	name[48];	//描述
}ZHREC_ASDU10_YX;//size=52

enum
{
	//1. 通信错误码
	ECOMM_NONE	=0,		//无错误
	ECOMM_USER	=70000,	//用户自定义错误码
	ECOMM_FILEREAD		=(ECOMM_USER+3),	//读文件出错
	ECOMM_STIMEOUT		=(ECOMM_USER+4),	//通信发送数据超时
	ECOMM_RTIMEOUT		=(ECOMM_USER+5),	//通信接收数据超时

	ECOMM_FILEWRITE		=(ECOMM_USER+7),	//写文件出错
	ECOMM_FILENOTFIND	=(ECOMM_USER+12),	//文件或目录不存在
	ECOMM_CHECKSUM		=(ECOMM_USER+16),	//接收数据校验错
	ECOMM_CREATESOCKET	=(ECOMM_USER+23),	//创建连接套接字失败
	ECOMM_FIRST	=(ECOMM_USER+1000),			//用户自定义通信错误码
	ECOMM_REMOTECLOSE	=(ECOMM_FIRST+0),	//远方已经关闭
	ECOMM_CONNTOOMANY	=(ECOMM_FIRST+1),	//连接太多
	ECOMM_NOACK			=(ECOMM_FIRST+2),	//远方没有应答
	ECOMM_PTYPE			=(ECOMM_FIRST+3),	//数据包类型错误
	ECOMM_REMOTECANCEL	=(ECOMM_FIRST+4),	//远方取消了当前操作
	ECOMM_LOCALCANCEL	=(ECOMM_FIRST+5),	//本地取消了当前操作
	ECOMM_TRANSBLOCK	=(ECOMM_FIRST+100),	//设备通信被阻塞
	ECOMM_FILECREATE	=(ECOMM_FIRST+200),	//创建文件失败
	ECOMM_FILEOPEN		=(ECOMM_FIRST+201),	//打开文件失败
	ECOMM_MEMORY		=(ECOMM_FIRST+202),	//内存分配失败
	ECOMM_COMMAND		=(ECOMM_FIRST+301),	//命令系统不支持
	ECOMM_NODATA		=(ECOMM_FIRST+302),	//没有所请求的数据
	ECOMM_BUSY			=(ECOMM_FIRST+304),	//系统繁忙
	ECOMM_PARAM			=(ECOMM_FIRST+305),	//参数错误
	ECOMM_FILESEEK		=(ECOMM_FIRST+306),	//移动文件指针失败
};

//#include <pop_pack.h>

//Added by JAN 2009-9-14 
typedef struct START_PACKET{
	struct PACKET_HEADER header;
	CENTER_DATA data;
} START_PACKET;




#endif /*__ZH2X_NETWORK_PROTOCOL_DEFINE_6328DDF5_2F9A_4346_9907_11753C1CCE0F_H__ */
