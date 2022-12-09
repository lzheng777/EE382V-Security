#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/bpf.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "bpf_defs.h"

#include <bpf/libbpf.h>
#include <errno.h>
#include <string.h>

#define ARRAY_MAP_SIZE 0x1337

__asm__(".symver __libc_start_main,__libc_start_main@GLIBC_2.2.5");

int create_bpf_map(int* map_addr)
{
    int ret = -1;
    int map_fd = -1;
    char vals[ARRAY_MAP_SIZE] = {0};
    union bpf_attr map_attrs =
    {
        .map_type = BPF_MAP_TYPE_ARRAY,
        .key_size = 4,
        .value_size = ARRAY_MAP_SIZE,
        .max_entries = 1,
    };

    map_fd = create_map(&map_attrs);

    if(map_fd < 0)
    {
        printf("[-] failed to create bpf array map!\n");
        goto done;
    }

    if(0 != update_map_element(map_fd, 0, vals, BPF_ANY))
    {
        printf("[-] failed to update map element values!\n");
        goto done;
    }

    *map_addr = map_fd;

    ret = 0;

done:
    return ret;
}


int main(){
    int* map_addr;
    FILE* log_file = fopen("map_pointer.log", "w");

    if (create_bpf_map(map_addr) != 0){
    	fprintf(log_file, "error");
    }
    else {
	fprintf(log_file, "%x\n", *map_addr);
    }

    fclose(log_file);

    while (1){}
}
