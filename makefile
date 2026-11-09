ARDUINO_CLI = /home/*/bin/arduino-cli

BOARD = arduino:avr:uno
BUILD_DIR = ./build
PORT ?=

.PHONY: all compile upload clean monitor
all: compile

compile:
	@$(ARDUINO_CLI) compile --fqbn $(BOARD) \
		--build-property compiler.c.extra_flags="-ffunction-sections -fdata-sections" \
		--build-property compiler.cpp.extra_flags="-ffunction-sections -fdata-sections" \
		--build-property compiler.c.elf.extra_flags="-Wl,--gc-sections" \
		--build-property compiler.optimization_flags="-O3" \
		--build-path $(BUILD_DIR) .

	@if [ -f $(BUILD_DIR)/compile_commands.json ]; then \
		cp $(BUILD_DIR)/compile_commands.json ./compile_commands.json; \
	fi

upload: compile
ifneq ($(PORT),)
	@$(ARDUINO_CLI) upload -p "$(PORT)" --fqbn $(BOARD) --input-dir $(BUILD_DIR) .
else
	@$(ARDUINO_CLI) board list
	@read -p "Enter the correct port: " PORT; \
	$(ARDUINO_CLI) upload -p $$PORT --fqbn $(BOARD) --input-dir $(BUILD_DIR) .
endif
	@$(ARDUINO_CLI) monitor -p "$(PORT)" --config baudrate=115200

monitor:
ifneq ($(PORT),)
	@$(ARDUINO_CLI) monitor -p "$(PORT)" --config baudrate=115200 #---timestamp
else
	@$(ARDUINO_CLI) board list
	@read -p "Enter the correct port: " PORT; \
	$(ARDUINO_CLI) monitor -p "$(PORT)" --config baudrate=115200 #--timestamp
endif

clean:
	@rm -rf $(BUILD_DIR) compile_commands.json

# gobble some arguments
%:
	@:
