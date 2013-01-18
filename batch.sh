#0 不迁移				00000		0
#1 迁移，不关盘			00001		1
#2 迁移，假关盘			00011		3
#3 迁移，真关盘			00111		7
#4 温度反馈				01111		15
#5 温度反馈，预测		11111		31
make pclean
make
hdparm -S 253 /dev/sd*
#./main ../trace/Financial2.spc spc /dev/sda /dev/sdb /dev/sdc 0 31 6 out5.txt
./main ../trace/Financial2.spc spc /dev/sda /dev/sdb /dev/sdc 0 31 6 out5.txt > log/log5.txt
hdparm -S 253 /dev/sd*
./main ../trace/Financial2.spc spc /dev/sda /dev/sdb /dev/sdc 0 7 6 out4.txt > log/log4.txt
hdparm -S 253 /dev/sd*
./main ../trace/WebSearch1.spc spc /dev/sda /dev/sdb /dev/sdc 0 31 6 out2.txt > log/log2.txt
hdparm -S 253 /dev/sd*
./main ../trace/WebSearch1.spc spc /dev/sda /dev/sdb /dev/sdc 0 7 6 out1.txt > log/log1.txt
