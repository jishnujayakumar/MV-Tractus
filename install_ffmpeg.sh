#go to opt folder, make directories, make directories if not exits
sudo mkdir -p /opt/dev-tools-sources
cd /opt/dev-tools-sources
sudo mkdir -p ffmpeg
cd ffmpeg

#make build and bin directories
sudo mkdir -p build bin

#clone ffmpeg repo as source 
sudo git clone https://github.com/FFmpeg/FFmpeg.git source

#go to the source dir
cd source

#install required packages
sudo apt-get -y install autoconf automake build-essential libass-dev libfreetype6-dev \
	libsdl1.2-dev libtheora-dev libtool libva-dev libvdpau-dev libvorbis-dev libxcb1-dev libxcb-shm0-dev \
	libxcb-xfixes0-dev pkg-config texinfo zlib1g-dev

#install yasm compiler
sudo apt install yasm

#instal h264 lib
sudo apt install libx264-dev

#configure
sudo ./configure --prefix="/opt/dev-tools-sources/ffmpeg/build" --bindir="/opt/dev-tools-sources/ffmpeg/bin" --enable-gpl --enable-libx264 --enable-static --enable-shared

#make (depending on your computer, it might be better to use less than all available processing units)
sudo make -j${nproc}

#install (depending on your computer, it might be better to use less than all available processing units)
sudo make install -j${nproc}

#Copying ffmpeg.config to a new config file for making it available to ldconfig
#sudo cp ffmpeg.conf /etc/ld.so.conf.d/ffmpeg.conf

#now update the ldconfig by just calling it
sudo ldconfig

#Bingo installation complete
echo "Bingo! FFmpeg Installation complete"
echo "Now call ./extract_mvs <video-source>"

