#!/bin/bash
RELEASE_DIR=("matlab" "R2011b"  "R2014" "R2015b" "R2016b" "R2017b" "R2018a" "R2018b" "R2019b")

#record version
basePath=$(pwd)
tagName=$(echo ${basePath##*/})
serverName=$(echo $tagName|cut -d "-" -f 2)
version=$(echo $tagName|cut -d "-" -f 3)
dateNow=$(date "+%Y-%m-%d-%H:%M:%S")
text="$serverName $version $dateNow"
versionFile="$AUTO_HOME/versionFile"
if [[ -f $versionFile ]];then
    if cat $versionFile | grep $serverName; then
        sed -i "/^$serverName/c$text" $versionFile
        echo $text
    else
        echo "$text">>$versionFile
    fi  
else
    touch $versionFile
    echo "serverName version date
$text">>$versionFile
fi

#backup
DATE=$(date +%Y%m%d_%H%M%S)
for FOLDER in ${RELEASE_DIR[*]}
do
    if [ -d "$AUTO_HOME/$FOLDER" ];then
	if [ ! -d $AUTO_HOME/backup ];then
            mkdir $AUTO_HOME/backup
        fi
        mv  $AUTO_HOME/$FOLDER $AUTO_HOME/backup/$FOLDER'_'$DATE -f 
    fi 
    
    cp $FOLDER $AUTO_HOME -rf
done
