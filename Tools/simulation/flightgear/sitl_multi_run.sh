#!/bin/bash

#developement/testing version for running multiple instancees of PX4 one by one

set -e

if [ "$#" -lt 4 ]; then
	echo usage: sitl_run.sh sitl_bin model src_path build_path
	exit 1
fi

sitl_bin="$1"
model="$2"
src_path="$3"
build_path="$4"

echo SITL ARGS

echo sitl_bin: $sitl_bin
echo model: $model
echo src_path: $src_path
echo build_path: $build_path

if [ -z "$PX4_ID" ];
then
    PX4_ID=0
fi
echo "========================"
echo $PX4_ID
echo "========================"

rootfs="$build_path/rootfs_"$PX4_ID # this is the working directory
mkdir -p "$rootfs"

export PX4_SIM_MODEL=flightgear_${model}

echo "FG setup"
cd "${src_path}/Tools/simulation/flightgear/flightgear_bridge/"
# 运行fg 并写入模型参数、创建通信协议
./FG_run.py models/${model}.json $PX4_ID
"${build_path}/build_flightgear_bridge/flightgear_bridge" $PX4_ID `./get_FGbridge_params.py "models/"${model}".json"` &
FG_BRIDGE_PID=`echo $!`



pushd "$rootfs" >/dev/null

# Do not exit on failure now from here on because we want the complete cleanup
set +e


sitl_command="\"$sitl_bin\" -i $PX4_ID \"$build_path\"/etc"

echo SITL COMMAND: $sitl_command

eval $sitl_command

popd >/dev/null

kill $FG_BRIDGE_PID
kill -9 `cat /tmp/px4fgfspid_${PX4_ID}`
