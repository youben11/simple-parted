#include "sparted.h"


/*
    Return an SPedDevice strcuture which contain all devices detected
*/
SPedDevice sped_get_device(){

        ped_device_probe_all();

        int count=1;
        PedDevice** dev=malloc(sizeof(PedDevice*));
        *dev=ped_device_get_next(NULL);

        PedDevice* p=*dev;

        //Creating device
        while((p=ped_device_get_next(p))!=NULL){
                dev=realloc(dev,sizeof(PedDevice*)*(++count));
                *(dev+count-1)=p;
        }

        //Setting the device structure
        SPedDevice device;
        device.device=dev;
        device.device_count=count;

        return device;
}

/*
    Return an SPedDisk strcuture which contain all disks detected
*/
SPedDisk sped_get_disk(){
        SPedDevice dev=sped_get_device();

        int count=0;
        PedDisk** d=NULL;

        for (int i=0;i<dev.device_count;i++){
                //Creating disk for device having valid partition table
                if(ped_disk_probe(dev.device[i])!=NULL){
                        if(!count){
                                d=malloc(sizeof(PedDisk*));
                                count++;
                        }else{
                                d=realloc(d,sizeof(PedDisk*)*(++count));
                        }
                        *(d+count-1)=ped_disk_new(dev.device[i]);
                }
        }

        //Setting the disk structure
        SPedDisk disk;
        disk.disk=d;
        disk.disk_count=count;

        return disk;
}

/*
    Search for a disk by its name
    return the disk if found
    return NULL on faiure
*/
PedDisk* sped_get_disk_by_name(char* name){

    if (name==NULL){
        errno=EINVAL;
        perror("sped_get_disk_by_name(NULL)");
        return NULL;
    }

    SPedDisk disks=sped_get_disk();

    for(int i=0;i<disks.disk_count;i++){
        if(!strcmp(name,disks.disk[i]->dev->path))
            return disks.disk[i];
    }

    return NULL;

}


/*
    Return an SPedPartition strcuture which contain all partition detected
    in the PedDisk d
    return NULL on failure
*/
SPedPartition sped_get_partition(PedDisk* d){

    if (d==NULL){
        errno=EINVAL;
        perror("sped_get_partition(NULL)");
    }

    PedPartition** p=NULL;
    int count=0;
    int lastp=ped_disk_get_last_partition_num(d);
    
    PedPartition* part=ped_disk_next_partition(d,NULL);

    //Creating partition
    for(int i=1;i<=lastp;i++){
        if((part=ped_disk_get_partition(d,i))!=NULL){
            if(!count){
            p=malloc(sizeof(PedPartition*));
            count++;
            }else{
            p=realloc(p,sizeof(PedPartition*)*(++count));
            }
            *(p+count-1)=part;    
        }
    }

    //Setting the partition structure
    SPedPartition partition;
    partition.partition=p;
    partition.partition_count=count;

    return partition;
}

/*
    Return an SPedDiskInfo structure which describe the disk (disk)
    return NULL on failure
*/
SPedDiskInfo sped_get_disk_info(PedDisk* disk){

    if (disk==NULL){
        errno=EINVAL;
        perror("sped_get_disk_info(NULL)");
    }

    SPedPartition part=sped_get_partition(disk);
    int count;
    count=part.partition_count;


    SPedDiskInfo dinfo;
    dinfo.partition_info_count=count;
    dinfo.partition_info=malloc(dinfo.partition_info_count*sizeof(SPedPartitionInfo));
    dinfo.size=disk->dev->length *512;

    dinfo.feature=disk->type->features;

    PedPartition* p;

    for(int i=0;i<count;i++){
    //Getting partition infos

        p=part.partition[i];

        //features is bitmap of supported feature (0x2 mean support partition name)
        if(dinfo.feature & 2){
            dinfo.partition_info[i].name=malloc(sizeof(char)*(strlen(ped_partition_get_name(p))+1));
            strcpy(dinfo.partition_info[i].name,ped_partition_get_name(p));
        }
        else dinfo.partition_info[i].name="";
        
        if(p->fs_type!=NULL){
            dinfo.partition_info[i].fs=malloc(sizeof(char)*(strlen(p->fs_type->name)+1));
            strcpy(dinfo.partition_info[i].fs,p->fs_type->name);

        }
        else dinfo.partition_info[i].fs="";
        
        dinfo.partition_info[i].type=malloc(sizeof(char)*(strlen(ped_partition_type_get_name(p->type))+1));
        strcpy(dinfo.partition_info[i].type,ped_partition_type_get_name(p->type));

        dinfo.partition_info[i].path=ped_partition_get_path(p);
        dinfo.partition_info[i].size=p->geom.length*512;


    }

    return dinfo;
}
