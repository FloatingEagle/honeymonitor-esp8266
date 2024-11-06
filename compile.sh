#!/bin/bash

# Requires:
# arduino-cli
# mklittlefs - https://github.com/earlephilhower/mklittlefs

function config {
    TARGET_BOARD="esp8266:esp8266:generic:eesz=4M1M,..." # UPDATE WITH YOUR OPTIONS
    SERIAL="/dev/tty.usbserial-SERIALPATH" # UPDATE WITH YOUR SERIAL DEVICE PATH
    BASE_DIRECTORY=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
    BUILD_DIRECTORY="$BASE_DIRECTORY/.build"
    DIST_DIRECTORY="$BASE_DIRECTORY/dist"
    SKETCH_FILE="$BASE_DIRECTORY/firmware.ino"
}

# PREPARE
function prepare {
    rm -Rf "$DIST_DIRECTORY";
    mkdir "$DIST_DIRECTORY";
}

# BUILD
function build {
    read -r -p "Would you like to upload the binary to serial after build? [y/N] " response
    echo "";
    echo "----------";
    echo "";
    echo "Building..."
    case "$response" in
        [yY][eE][sS]|[yY]) 
    		arduino-cli compile -b "$TARGET_BOARD" --build-path "$BUILD_DIRECTORY" --upload --port "$SERIAL" "$SKETCH_FILE"

            ;;
        *)
    		arduino-cli compile -b "$TARGET_BOARD" --build-path "$BUILD_DIRECTORY" "$SKETCH_FILE"
            ;;
    esac

    BUILD_STATUS=$?
    case "$BUILD_STATUS" in
        0)
            echo "Build: [OK]"
            ;;
        *) 
            echo "Build: [Error] ($BUILD_STATUS) - Exiting."
            exit 1
            ;;
    esac
}

# COPY BUILD
function copy_build {
    echo "";
    echo "----------";
    echo "";
    echo "Compiled, copying firmware to $DIST_DIRECTORY/honeymonitor.bin"
    cp -Rf "$BUILD_DIRECTORY/firmware.ino.bin" "$DIST_DIRECTORY/honeymonitor.bin"
}

# BUILD FILESYSTEM
function build_filesystem {
    echo "";
    echo "----------";
    echo "";
    echo "Compiling FileSystem..."

    PROPS=`arduino-cli compile --fqbn $TARGET_BOARD --show-properties`
    BUILD_SPIFFS_BLOCKSIZE=`echo "$PROPS"|grep "^build.spiffs_blocksize"|cut -d= -f2`
    BUILD_SPIFFS_END=`echo "$PROPS"|grep "^build.spiffs_end"|cut -d= -f2`
    BUILD_SPIFFS_PAGESIZE=`echo "$PROPS"|grep "^build.spiffs_pagesize"|cut -d= -f2`
    BUILD_SPIFFS_START=`echo "$PROPS"|grep "^build.spiffs_start"|cut -d= -f2`
    BUILD_SPIFFS_SIZE=`echo "ibase=16;${BUILD_SPIFFS_END:2}-${BUILD_SPIFFS_START:2}"|bc -q`
    echo BUILD_SPIFFS_SIZE "$BUILD_SPIFFS_SIZE"
    mklittlefs -c data -p "$BUILD_SPIFFS_PAGESIZE" -b "$BUILD_SPIFFS_BLOCKSIZE" -s "$BUILD_SPIFFS_SIZE" "$DIST_DIRECTORY/filesystem.bin"

    BUILD_STATUS=$?
    case "$BUILD_STATUS" in
        0)
            echo "Build: [OK]"
            ;;
        *) 
            echo "Build: [Error] ($BUILD_STATUS) - Exiting."
            exit 1
            ;;
    esac
}

# FLASH FILESYSTEM
function flash_filesystem {
    echo "";
    echo "----------";
    echo "";
    case "$response" in
        [yY][eE][sS]|[yY]) 
            echo "Flashing FileSystem..."
            esptool.py --baud 115200 --port $SERIAL write_flash $BUILD_SPIFFS_START $DIST_DIRECTORY/filesystem.bin
            ;;
        *)
            echo "Skipping Flash FileSystem as we're not flashing per user decision."
            ;;
    esac
}

# CLEAN
function clean {
    echo "";
    echo "----------";
    echo "";
    echo "Cleaning..."
    rm -Rf "$BUILD_DIRECTORY"
}

# FINISH
function finish {
    echo "";
    echo "----------";
    echo "";
    echo "Finished";
    echo "";
    echo "----------";
    echo "";
}

# SERIAL ATTACH
function serial_attach {
    case "$response" in
        [yY][eE][sS]|[yY]) 
            read -r -p "Would you like to attach the serial to the device (with picocom)? [y/N] " response

            case "$response" in
                [yY][eE][sS]|[yY]) 
                    echo "Entering picocom... (Use CTRL + A -> CTRL + X to exit)"
                    picocom -b 115200 "$SERIAL"
            esac
            ;;
        *)
            # Did not flash, will not even ask to attach.
            ;;
    esac

    
}


# Run
    echo "";
    echo "";
    echo "----------";
    echo "";
    echo "HoneyMonitor Proxy Compiler, Starting...";
    echo "";
    echo "----------";
    echo "";


config;
prepare;
build;
copy_build;
build_filesystem;
flash_filesystem;
clean;
finish;
serial_attach;