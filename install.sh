#! /bin/bash
#Star Pi
# please do this first:
#sudo apt-get update
#sudo apt-get dist-upgrade
#sudo rpi-update
#git clone https://github.com/ChrisDick/StarPi
#chmod u+x install.sh
#./install

#remove?
#sudo sed -i -e "\$adeb http://http.debian.net/debian wheezy-backports main" /etc/apt/sources.list
#sudo apt-get install debian-archive-keyring 
#sudo apt-get -t wheezy-backports install scons

sudo apt-get -y install libncurses5-dev python-dev pps-tools git-core scons

#rpicam interface
git clone https://github.com/silvanmelchior/RPi_Cam_Web_Interface.git
cd RPi_Cam_Web_Interface
chmod u+x *.sh
sudo install.sh

cd ../Software

#Gps deamon
wget http://git.savannah.gnu.org/cgit/gpsd.git/snapshot/gpsd-master.tar.gz
tar -zxvf  gpsd-master.tar.gz

rm gpsd-master.tar.gz

cp -r gpsd-master/* StarPi/Software/Src/GPSD 
rm -r gpsd-master

cd ./Src/GPSD 
sudo scons && sudo scons check && sudo scons udev-install

cd ./../..
mkdir Obj
mkdir Out
make
cd ..
sudo cp -r ./Website/* /var/www





