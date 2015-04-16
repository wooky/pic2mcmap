CC=gcc
CPP=g++
IUP=C:/MinGW/iup/iup
IM=C:/MinGW/iup/im
CNBT=cNBT-master
CFLAGS=-Wall

all: pic2mcmap

pic2mcmap:
	$(CPP) -I$(IUP)/include -I$(IM)/include -L$(IUP) -L$(IM) -L$(CNBT) -o pic2mcmap.exe -liup -liupim -lim -lim_process -lz -lgdi32 -lcomdlg32 -lcomctl32 -luuid -lole32 -lnbt src/*.cpp

clean:
	rm pic2mcmap.exe
