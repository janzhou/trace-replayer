version 1.0
	最大强度下发

version	2.0
	实现iops的控制，每秒钟下发多少条，队列长度参数输入

version	3.0
	增加输出，将每秒钟的iops，mbps输出到文件

version 4.0
	增加feedback，先以最大强度回放一遍，得到每一秒的iops；
	第二遍回放时读取前一次回放的iops再乘以强度，得到现在要下发的trace数量，从而获得精确的强度。

version 5.1
	回放功能改成库的形式，速度控制删除，可启动多个回放线程。trace格式改成库的形式，可增加格式。

version 6
	重写trace引擎

version 7
	重写feedback，完成性能反馈(调了一个重放引擎bug)
	
version 8
	温度模块
	trace.c无警告
	replayer.c无警告
	回调一个version 7中==写成=号的bug
	修改select_disk中硬盘数不够时的一个bug