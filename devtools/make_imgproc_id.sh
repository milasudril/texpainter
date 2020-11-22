#!/bin/bash

invalid_img_proc_id="00000000000000000000000000000000"
img_proc_id=$invalid_img_proc_id
while [ "$img_proc_id" == "$invalid_img_proc_id" ]; do
	img_proc_id=$(hexdump -n 16 -e '4/4 "%08x" 1 "\n"' /dev/random)
done
echo $img_proc_id