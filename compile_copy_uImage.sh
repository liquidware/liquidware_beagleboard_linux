#!/bin/sh

MOD_PATH=/tmp/bbext
MOD_DEV=/dev/sdb2

FAT_PATH=/tmp/bbfat
FAT_DEV=/dev/sdb1

compileImage="sudo make -j8 INSTALL_MOD_PATH=${MOD_PATH} ARCH=arm uImage CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-"

compileModules="sudo make -j8 INSTALL_MOD_PATH=${MOD_PATH} ARCH=arm modules CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-"

installModules="sudo make -j8 INSTALL_MOD_PATH=${MOD_PATH} ARCH=arm KERNEL_CONFIGS=DEBUG uImage modules modules_install CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-"

#echo "unmounting"
#sudo umount $MOD_DEV
#sudo umount $FAT_DEV
sleep 1
echo "mounting"
#sudo mount $FAT_DEV $FAT_PATH
#sudo mount $MOD_DEV $MOD_PATH

echo "Compiling Image"
if  $installModules; then
        echo "Copying uImage to ${FAT_PATH}"    
        sudo cp ./arch/arm/boot/uImage $FAT_PATH/uImage

        cd $FAT_PATH
        echo "Syncing data"
        sync
        cd ..
        sleep 2       
        

        cd $MOD_PATH
        echo "Syncing data"
        sync
        cd ..
        sleep 2

        #sudo umount $FAT_DEV
        #sudo umount $MOD_DEV        
        
        echo 'Done.'
        exit
fi

echo 'Error: '
