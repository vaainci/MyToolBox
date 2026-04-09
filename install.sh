#!/bin/bash

# Terminal Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

APP_NAME="mytoolbox"
ALIAS_NAME="mtb"
INSTALL_DIR="/usr/local/bin"

echo -e "${BLUE}=== MyToolBox Installation (Multi-Command Mode) ===${NC}"

# 1. Update via Git
if [ -d ".git" ]; then
    echo "Checking for updates..."
    git pull origin main
fi

# 2. Compilation with CMake
echo "Configuring and building..."
mkdir -p build
cd build

# Generate build files
cmake ..

# Compile the project
if make -j$(nproc); then
    echo -e "${GREEN}Compilation successful.${NC}"
else
    echo -e "${RED}Compilation failed! Check your C++ code.${NC}"
    exit 1
fi

# 3. Smart Binary Detection
# We look for the binary regardless of capitalization (MyToolBox or mytoolbox)
REAL_BINARY=$(find . -maxdepth 1 -type f -executable -iname "mytoolbox" -print -quit)

if [ -z "$REAL_BINARY" ]; then
    echo -e "${RED}Error: Compiled binary not found in build directory.${NC}"
    exit 1
fi

# 4. Installation
echo "Installing main binary to $INSTALL_DIR/$APP_NAME..."
sudo mv "$REAL_BINARY" "$INSTALL_DIR/$APP_NAME"
sudo chmod +x "$INSTALL_DIR/$APP_NAME"

# 5. Symbolic Link Creation (The magic for 'mtb')
echo "Creating symbolic link: $INSTALL_DIR/$ALIAS_NAME -> $APP_NAME..."
# -s: symbolic, -f: force (overwrites if exists)
sudo ln -sf "$INSTALL_DIR/$APP_NAME" "$INSTALL_DIR/$ALIAS_NAME"

# Go back to root
cd ..

# 6. Verification & Final touch
echo -e "\n${GREEN}Success! MyToolBox is now installed.${NC}"
echo -e "Available commands:"
echo -e "  - ${BLUE}$APP_NAME${NC}"
echo -e "  - ${BLUE}$ALIAS_NAME${NC}"

# Force Zsh/Bash to recognize the new commands immediately
if [[ $SHELL == *"zsh"* ]]; then
    zsh -c "rehash"
elif [[ $SHELL == *"bash"* ]]; then
    hash -r
fi

echo -e "\nTry typing: ${BLUE}$ALIAS_NAME --help${NC}"