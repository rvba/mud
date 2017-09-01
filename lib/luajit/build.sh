#!/bin/bash
echo "[BUILD LUAJIT]"
ls
src_dir="$1"/src/mud/lib/luajit
build_dir="$2"/luajit
echo $src_dir
echo $build_dir
if [[ ! -d $build_dir ]]
then
	echo "MAKE DIR"
	mkdir $build_dir
	echo "COPY SRC"
	cp -R $src_dir/src $build_dir
	cp -R $src_dir/dynasm $build_dir
	cp $src_dir/Makefile $build_dir
fi

cd $build_dir
make

