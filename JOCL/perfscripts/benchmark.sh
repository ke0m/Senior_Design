#! /bin/bash


cd ../FlopsTestGPULoop/src
javac -classpath ../lib/JOCL-0.1.9.jar FlopsTestGPULoop.java

echo GPU >> ../../perflogs/ArraySumLogs/JOCLlog.txt

for i in {0..99}
do
	java -classpath .:../lib/JOCL-0.1.9.jar FlopsTestGPULoop >> ../../perflogs/ArraySumLogs/JOCLlog.txt
done

cd ../../FlopsTestCPULoop/src
javac -classpath . FlopsTestCPULoop.java

echo CPU >> ../../perflogs/ArraySumLogs/Javalog.txt

for i in {0..99}
do
	java -classpath . FlopsTestCPULoop >> ../../perflogs/ArraySumLogs/Javalog.txt
done

paste ../../perflogs/ArraySumLogs/JOCLlog.txt ../../perflogs/ArraySumLogs/Javalog.txt | column -s $'\t' -t > ../../perflogs/ArraySumLogs/log_`date '+%Y-%m-%d-%H%M'`.txt

echo $HOSTNAME >> ../../perflogs/ArraySumLogs/log_`date '+%Y-%m-%d-%H%M'`.txt

rm ../../perflogs/ArraySumLogs/JOCLlog.txt
rm ../../perflogs/ArraySumLogs/Javalog.txt

exit 0
