#!/bin/bash

if [ $# -eq 0 ];then
        echo "usage: please input version"
        exit
fi

if [ $HRP_HOME ];then
        echo "HRP_HOME="$HRP_HOME
else
        echo "Invalid environment HRP_HOME"
        exit -1
fi

Version=$1
ModelName=BaseRFMCard
TagName=ST-ESP-$ModelName$1
ReleaseDir=$HRP_HOME/ReleasePack/$TagName/$ModelName/Redhawk6.5/lib

#check whether so file is exist
if [ ! -f $HRP_HOME/lib/lib$ModelName.a ];then
	echo "Error:$HRP_HOME/lib/lib$ModelName.so is not exist."
	exit -1
fi

#delete release folder if exist
if [ -d $ReleaseDir ];then
        rm $ReleaseDir -rf
fi
cd $HRP_HOME/$ModelName

mkdir -p $ReleaseDir

cp $HRP_HOME/lib/lib$ModelName.a $ReleaseDir
cp ./bin/* $ReleaseDir
cd $HRP_HOME/ReleasePack
tar czf ST-ESP-$ModelName$Version.tar.gz $HRP_HOME/ReleasePack/ST-ESP-$ModelName$Model$Version
cd -
