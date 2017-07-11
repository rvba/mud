#!/bin/bash
echo "[BUILD LUAJIT]"
src_dir=/home/milovann/SOURCES/MINUIT/modus/src/stone/lib/luajit
build_dir=/home/milovann/SOURCES/MINUIT/modus-build/luajit
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

