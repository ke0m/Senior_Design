#! /bin/bash

cd ../flopsTestGPULoop

fileGPU="./flopsTestGPULoop/flopsTestGPU"

if [ ! -f $fileGPU ]
then
	make flopsTestGPULoop
else
	make clean
	make flopsTestGPULoop
fi

rm GPULog.txt


for i in {0..999}
do
	
	
	./flopsTestGPULoop >> GPULog.txt
	
done

cd ../flopsTestCPULoop

fileCPU="./flopsTestCPULoop/flopsTestCPU"


if [ ! -f $fileCPU ]
then
        make flopsTestCPULoop
else
        make clean
        make flopsTestCPULoop
fi

rm CPULog.txt

for i in {0..999}
do


        ./flopsTestCPULoop >> CPULog.txt

done


exit 0
