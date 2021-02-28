#!/bin/bash

src=$1
target=$2
target_dir=$3

__targets_rel/cli/texporter --supersampling=8 "$src" "$target_dir"/temp.exr
convert "$target_dir"/temp.exr $target