#!/bin/bash
# Bootstrap Gradle wrapper for Android build

set -e

GRADLE_VERSION="8.2"
WRAPPER_DIR="gradle/wrapper"

mkdir -p "$WRAPPER_DIR"

echo "Setting up Gradle wrapper version $GRADLE_VERSION..."

# Try downloading from official Gradle services
if command -v gradle &> /dev/null; then
    echo "Found system Gradle installation, using it to generate wrapper..."
    gradle wrapper --gradle-version $GRADLE_VERSION
    exit 0
fi

# Alternative: download pre-built gradle distribution
echo "Gradle not found in PATH. Please install Gradle or Android Studio SDK."
echo ""
echo "Option 1: Install with package manager"
echo "  Ubuntu/Debian: sudo apt install gradle"
echo "  macOS: brew install gradle"
echo ""
echo "Option 2: Download from https://gradle.org/releases/"
echo "  Extract and add to PATH, then run this script again"
echo ""
echo "Option 3: Install Android Studio and use built-in Gradle"
echo "  https://developer.android.com/studio"
