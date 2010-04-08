#!/bin/sh

MOD_PATH=/media/239db643-c750-4ee7-95cb-a50e260b2873

compileImage='sudo make -j8 INSTALL_MOD_PATH=${MOD_PATH} ARCH=arm uImage CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-'

compileModules='sudo make -j8 INSTALL_MOD_PATH=${MOD_PATH} ARCH=arm modules CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-'

installModules='sudo make -j8 INSTALL_MOD_PATH=${MOD_PATH} ARCH=arm modules_install CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-'

if $compileImage; then
    echo 'Copying Image to /media/bbfat'    
    cp /home/eye/linux-omap/linux-omap-2.6-OEwork/arch/arm/boot/uImage /media/bbfat/uImage
    cd /media/bbfat
    sync
    echo 'Done.'
    exit
fi

echo 'Error: '

#sudo make -j8 INSTALL_MOD_PATH=/media/239db643-c750-4ee7-95cb-a50e260b2873 ARCH=arm menuconfig CROSS_COMPILE=/home/eye/CodeSourcery/Sourcery_G++_Lite/bin/arm-none-linux-gnueabi-
