TARGET_LIB=libshare.so  
TARGET_TEST=libshare_test
 
DEV_DIR = ../dev
LIB_DIR = $(DEV_DIR)/lib
INC_DIR = $(DEV_DIR)/inc

CFLAGS  = -march=armv4t -mtune=arm920t -Wall -O2 -shared -fPIC -I. $(LIBS)  
CC      = arm-none-linux-gnueabi-gcc  
STRIP   = arm-none-linux-gnueabi-strip  
SRC     = file.c crc16.c pid.c ini.c
INC     = file.h crc16.h pid.h ini.h
  
all: $(TARGET_LIB) $(TARGET_TEST)
$(TARGET_LIB): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

test:$(TARGET_TEST)	

$(TARGET_TEST):libshare_test.c
	$(CC) -march=armv4t -mtune=arm920t -L. -lshare -o $@ $^

install:$(scp)
	mkdir -p $(INC_DIR)/libshare
	cp -f $(TARGET_LIB)  $(LIB_DIR)
	cp -f $(INC)  $(INC_DIR)/libshare
	chmod 777 -R  $(INC_DIR)/libshare

clean:
	rm -rf $(TARGET_TEST)

cleanall:  
	rm -rf *.o $(TARGET_LIB) $(TARGET_TEST)

  
.PHONY: all clean install cleanall test
