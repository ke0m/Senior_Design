#! /bin/bash


cd ../FlopsTestGPULoop

fileGPU="./FlopsTestGPULoop/FlopsTestGPULoop"

if [ ! -f $fileGPU ]
then
	make FlopsTestGPULoop
else
	make clean
	make FlopsTestGPULoop
fi

echo GPU >> ../perflogs/ArraySumLogs/GPULog.txt
for i in {0..99}
do
	
	
	./FlopsTestGPULoop >> ../perflogs/ArraySumLogs/GPULog.txt
	
done

cd ../FlopsTestCPULoop

fileCPU="./FlopsTestCPULoop/FlopsTestCPULoop"


if [ ! -f $fileCPU ]
then
        make FlopsTestCPULoop
else
        make clean
        make FlopsTestCPULoop
fi
echo CPU >> ../perflogs/ArraySumLogs/CPULog.txt
for i in {0..99}
do

        ./FlopsTestCPULoop >> ../perflogs/ArraySumLogs/CPULog.txt

done

paste ../perflogs/ArraySumLogs/GPULog.txt ../perflogs/ArraySumLogs/CPULog.txt | column -s $'\t' -t > ../perflogs/ArraySumLogs/log_`date '+%Y-%m-%d-%H%M'`.txt

echo $HOSTNAME >> ../perflogs/ArraySumLogs/log_`date '+%Y-%m-%d-%H%M'`.txt

rm ../perflogs/ArraySumLogs/GPULog.txt
rm ../perflogs/ArraySumLogs/CPULog.txt

exit 0
