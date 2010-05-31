#!/bin/sh

#config parameters
FAT_DEVICE=/dev/sdb1
ROOTFS_DEVICE=/dev/sdb2

ROOTFS_MOUNT=/tmp/bbext
FAT_MOUNT=/tmp/bbfat

COMPILER_INSTALL=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-

#helper commands
menuconfig="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm menuconfig CROSS_COMPILE=${COMPILER_INSTALL}"

compileImage="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm uImage CROSS_COMPILE=${COMPILER_INSTALL}"

compileModules="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm modules CROSS_COMPILE=${COMPILER_INSTALL}"

installModules="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm KERNEL_CONFIGS=DEBUG uImage modules modules_install CROSS_COMPILE=${COMPILER_INSTALL}"

if [ "$1" == "menuconfig" ]; then
theCommand=$menuconfig
 elif [ "$1" == "compileImage" ]; then
    theCommand=$compileImage
 elif [ "$1" == "compileModules" ]; then
    theCommand=$compileModules
 elif [ "$1" == "installModules" ]; then
    theCommand=$installModules
 else
    echo "Unknown Command, exiting."
    exit
fi

echo $theCommand

echo "unmounting"
sudo umount $ROOTFS_DEVICE
sudo umount $FAT_DEVICE

echo "mounting"
sudo mount $FAT_DEVICE $FAT_MOUNT
sudo mount $ROOTFS_DEVICE $ROOTFS_MOUNT

echo "Compiling Image"
if $theCommand 
then
        echo "Copying uImage to ${FAT_MOUNT}"    
        sudo cp ./arch/arm/boot/uImage $FAT_MOUNT/uImage

        cd $FAT_MOUNT
        echo "Syncing data"
        sync
        cd ..
        sleep 1       
        

        cd $ROOTFS_MOUNT
        echo "Syncing data"
        sync
        cd ..
        sleep 1

        sudo umount $FAT_DEVICE
        sudo umount $ROOTFS_DEVICE        
        
        echo 'Done.'
        exit
fi

echo 'Error: '
