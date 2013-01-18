#ifndef __DM_H
#define __DM_H

//�˿�����
typedef enum{
	AC_CURRENT = 1,		 //����������Чֵ
	AC_VOLTAGE = 2,		 //������ѹ��Чֵ
	
	AC_CPHASE = 3,		//����������λ
	AC_VPHASE = 4,		//������ѹ��λ
	
	AC_CFREQUENCY = 5,  //��������Ƶ��
	AC_VFREQUENCY = 6,  //������ѹƵ��
	
	TEMPERATURE = 7,	//�¶�ֵ
	
	DC_CURRENT = 8,		//ֱ��������Чֵ
	DC_VOLTAGE = 9		//ֱ����ѹ��Чֵ
}PortType;				//ö�����ͣ���ʾ�˿�����

//�˿���Ϣ
typedef struct PortInfo{
	int id;				//�豸id
	int type;		//�˿�����
	int group;			//�˿��������
	char name[30];		//�˿�����
}PortInfo;				//�ṹ���ͣ���ʾ�˿���Ϣ

typedef struct Device{
	char	ip[20];		//�豸ip
	int  port;		//�豸�˿ں�
	char  type[20];  //�豸����
	char name[20];
}Device;			//�ṹ���ͣ���ʾ�豸��ʼ��Ϣ


//��ʱ����
extern int DeviceConnect(struct Device *);
extern int DeviceDisonnect(int device_id);
extern int GetDevices(Device devices[], int n);	//��ÿ����豸
extern int GetPortsById(int device_id, PortInfo ports[], int n);
extern int StartSample(void);
extern int StopSample(void);
extern int SetPort(int device_id , int port_id , float * pValue);
extern int DeletePort(int device_id , int port_id);
extern int IsPortUsed(int device_id , int port_id);


#endif
