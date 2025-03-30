# Makefile for wrapping PlatformIO commands

# Set the board environment variable
ENV = seeed_xiao_esp32s3

.PHONY: build upload remote-upload remote-monitor help

all: build
	@echo "Building for environment: $(ENV)"
	@echo "Use 'make upload' to upload locally or 'make remote-upload' to upload remotely."
	@echo "Use 'make remote-monitor' to monitor the remote device."

# Build the project
build:
	platformio run --environment $(ENV)

# Upload the project locally
upload:
	platformio run --target upload --environment $(ENV)

# Upload the project remotely
remote-upload:
	platformio remote run --target upload --environment $(ENV)

# Monitor the remote device
remote-monitor:
	platformio remote device monitor --environment $(ENV)

# Display available commands
help:
	@echo "Available targets:"
	@echo "  build           - Build the project"
	@echo "  upload          - Upload locally"
	@echo "  remote-upload   - Upload remotely"
	@echo "  remote-monitor  - Monitor the remote device"
