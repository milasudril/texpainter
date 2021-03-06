#!/bin/bash

src=$1
target=$2
target_dir=$3

PATH=__targets_rel/cli:__targets_dbg/cli:__targets_gcov/cli:"$PATH"

texporter --supersampling=8 "$src" "$target_dir"/temp.exr
convert "$target_dir"/temp.exr $target