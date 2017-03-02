srec_cat 32VLDisc.bin -bin -exclude 0x1C 0x28 -length-l-e 0x1C 4 -generate 0x20 0x28 -repeat-string 32VLDisc -o 32VLDisc.bin -bin 
srec_cat 32VLDisc.bin -bin -crc32-l-e  -max-addr 32VLDisc.bin -bin -o 32VLDisc.bin -bin
