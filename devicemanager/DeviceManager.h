#ifndef __DM_H
#define __DM_H

//端口类型
typedef enum{
	AC_CURRENT = 1,		 //交流电流有效值
	AC_VOLTAGE = 2,		 //交流电压有效值
	
	AC_CPHASE = 3,		//交流电流相位
	AC_VPHASE = 4,		//交流电压相位
	
	AC_CFREQUENCY = 5,  //交流电流频率
	AC_VFREQUENCY = 6,  //交流电压频率
	
	TEMPERATURE = 7,	//温度值
	
	DC_CURRENT = 8,		//直流电流有效值
	DC_VOLTAGE = 9		//直流电压有效值
}PortType;				//枚举类型，表示端口类型

//端口信息
typedef struct PortInfo{
	int id;				//设备id
	int type;		//端口类型
	int group;			//端口所在组号
	char name[30];		//端口名称
}PortInfo;				//结构类型，表示端口信息

typedef struct Device{
	char	ip[20];		//设备ip
	int  port;		//设备端口号
	char  type[20];  //设备类型
	char name[20];
}Device;			//结构类型，表示设备初始信息


//暂时不看
extern int DeviceConnect(struct Device *);
extern int DeviceDisonnect(int device_id);
extern int GetDevices(Device devices[], int n);	//获得可用设备
extern int GetPortsById(int device_id, PortInfo ports[], int n);
extern int StartSample(void);
extern int StopSample(void);
extern int SetPort(int device_id , int port_id , float * pValue);
extern int DeletePort(int device_id , int port_id);
extern int IsPortUsed(int device_id , int port_id);


#endif
