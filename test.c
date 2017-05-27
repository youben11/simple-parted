#include "sparted.h"

void main(){   

    SPedDisk disk=sped_get_disk();
    SPedDiskInfo dinfo=sped_get_disk_info(disk.disk[0]);

    for(int i=0;i<dinfo.partition_info_count;i++){
        printf("%s %s %s %s %lldMo\n",dinfo.partition_info[i].name,dinfo.partition_info[i].path,dinfo.partition_info[i].type,dinfo.partition_info[i].fs,dinfo.partition_info[i].size/1048576);
    }
    printf("%lld\n",dinfo.size/1024/1024/1024);

}
