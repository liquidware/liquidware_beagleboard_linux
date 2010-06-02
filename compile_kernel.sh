#!/bin/sh

# Description:
#  Compiles the kernel and copies uImage and/or modules 
#  to the SD card formatted for the BeagelBoard
#
# Syntax:
#  ./compile_kernel.sh [action] [SD card device]
#
# Example:
#  ./compile_kernel.sh image /dev/sdb #copies only the uImage to the SD card
#

#config parameters
FAT_DEVICE=${2}1
ROOTFS_DEVICE=${2}2

ROOTFS_MOUNT=/tmp/bbext
FAT_MOUNT=/tmp/bbfat

COMPILER_INSTALL=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-


#existance check the device
if [ -b "$FAT_DEVICE" ] && [ -b "$ROOTFS_DEVICE" ]; then
    echo "Using ${FAT_DEVICE} for uImage and ${ROOTFS_DEVICE} for rootfs"
else
    echo "Error: Invalid device"
    exit
fi


#existance check the compiler 
if [ -e ${COMPILER_INSTALL}gcc ]; then
    echo "Using $COMPILER_INSTALL for gcc"
else
    echo "Error: arm-none-linux-gnueabi compiler not found"
    exit
fi

#helper commands
menuconfig="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm menuconfig CROSS_COMPILE=${COMPILER_INSTALL}"

image="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm uImage CROSS_COMPILE=${COMPILER_INSTALL}"

modules="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm modules CROSS_COMPILE=${COMPILER_INSTALL}"

modules_install="sudo make -j8 INSTALL_ROOTFS_MOUNT=${ROOTFS_MOUNT} ARCH=arm uImage modules modules_install CROSS_COMPILE=${COMPILER_INSTALL}"

if [ "$1" == "menuconfig" ]; then
theCommand=$menuconfig
 elif [ "$1" == "image" ]; then
    theCommand=$image
 elif [ "$1" == "modules" ]; then
    theCommand=$modules
 elif [ "$1" == "modules_install" ]; then
    theCommand=$modules_install
 else
    echo "Unknown Command, exiting."
    exit
fi

echo "unmounting"
sudo umount $ROOTFS_DEVICE
sudo umount $FAT_DEVICE

echo "mounting"
sudo mount $FAT_DEVICE $FAT_MOUNT
sudo mount $ROOTFS_DEVICE $ROOTFS_MOUNT

echo $theCommand
if $theCommand 
then
        echo "Copying uImage to ${FAT_MOUNT}"    
        sudo cp ./arch/arm/boot/uImage $FAT_MOUNT/uImage

        echo "Syncing data"
        sync
        sleep 1
        sync
        sleep 1
        
        #echo "Unmounting ${2}"
        #sudo umount $FAT_DEVICE
        #sudo umount $ROOTFS_DEVICE        
        
        echo 'Done.'
        exit
fi

echo 'Error: '
