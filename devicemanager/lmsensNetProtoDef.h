#ifndef LM_SENSORS_H
#define LM_SENSORS_H

#ifndef LIB_SENSORS_DATA_H
typedef struct sensors_socket_data {
	char  label[20];
	float  data;
	char sulable[10];
} sensors_socket_data;
#endif

//命令格式
enum{
//发往LM方向命令
	LM_GET_PORTS		= 0x00000001,
	LM_GET_DATA 		= 0x00000002,
	LM_STOP     		= 0x00000003,
//LM返回命令
	LM_PORTS_RETURN		= 0x10000001,
	LM_DATA_RETURN		= 0x10000002,
};

struct lm_pkt_head{
	int pkt_type;
};
//发往LM数据包
struct lm_pkt_get_ports{
	struct lm_pkt_head pkt_head;
};

struct lm_pkt_get_data{
	struct lm_pkt_head pkt_head;
};

struct lm_pkt_stop{
	struct lm_pkt_head pkt_head;
};
//LM返回数据包
struct lm_pkt_data_return{
	struct lm_pkt_head pkt_head;
	float data[1];
};

struct lm_pkt_ports_return{
	struct lm_pkt_head pkt_head;
	int count;
	sensors_socket_data ports[1];
};
#endif