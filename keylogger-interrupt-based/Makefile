obj-m := ./src/keylogger.o

KDIR := /lib/modules/$(shell uname -r)/build
BUILD_DIR := $(pwd)/build
EXTRA_CFLAGS := -I($pwd)/include

all: $(BUILD_DIR)
	@echo "Building the kernel module..."
	make -C $(KDIR) M=$(pwd) modules
	@mkdir -p $(BUILD_DIR)
	@mv *.order *.symvers *.ko *.o *.mod.c $(BUILD_DIR)/ || true
	@mv ./src/keylogger.mod $(BUILD_DIR)/ || true
	@find ./src -maxdepth 1 -type f \( -name '*.ko' -o -name '*.o' -o -name '*.mod.*' -o -name '*.cmd' \) -exec mv -t $(BUILD_DIR) {} + || true

clean: 
	@echo "Cleaning the kernel module..."
	make -C $(KDIR) M=$(pwd) clean
	rm -rf $(BUILD_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)