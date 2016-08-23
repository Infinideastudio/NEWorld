#!/bin/sh
# Install deps
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo add-apt-repository ppa:george-edison55/precise-backports
sudo apt-get update -y -qq
sudo apt-get install -y \
p7zip \
cmake \
cmake-data \
libgl1-mesa-dev \
libglu1-mesa-dev \
freeglut3-dev \
libglew-dev \
libfreetype6-dev \
libsdl2-dev \
libsdl2-image-dev
sudo apt-get install -y ${CXX} ${CC} # Install the compiler given by .travis.yml

sudo ln -s /usr/include/freetype2/freetype /usr/include/freetype -f

# Install boost
if [ ! -f ./boost_1_61_0/cached_mark ];
then
  wget http://downloads.sourceforge.net/project/boost/boost/1.61.0/boost_1_61_0.7z
  p7zip -d ./boost_1_61_0.7z > 7zlog.txt
  cd ./boost_1_61_0
  touch cached_mark
  ./bootstrap.sh --with-libraries=date_time,filesystem,system
  ./b2
  cd ..
fi
pwd
