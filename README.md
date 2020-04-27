# Projects

Arduino - Wemos RFID and Painless Mesh An attempt was made to create a rudimentary blockchain using a wemos d1 mini clone and an MRC522 sensor. Please see the esp8666R1D1 project folder for details. The Hashing and RFID Implementation folders were just for experimentation. Atom and the platformIO plugin were utilized.


Coding Environment
All code was run on Ubuntu 18.04 LTS. Atom IDE was installed and PlatformIO was utilized. 

All code was written in PlatformIO’s version of C++ for embedded systems. 

The Arduino environment can be used as well if the proper libraries are installed

// On a Debian based system, download the Atom .deb file from https://atom.io/ 

Run in terminal:sudo dpkg -i atom.deb
sudo apt install ./atom-amd64.deb
// On Ubuntu 20.04 and above you may have to use the following: sudo snap install atom --classic
// From a terminal install clang, git, and download project files:
sudo apt-get install clang gitgit 
clone https://github.com/cthulhu1988/Projects

// Open atom and navigate to Edit > Preferences > Install
// Install packages: platformio-ide and platformio-ide-terminal.
// Restart atom and install the following libraries through PlatformIO: painlessMesh MFRC522
// Through PlatformIO, click Open Project, select esp8266R1D1 folder.
// Main.cpp file is in src directory. The Code should compile now.

