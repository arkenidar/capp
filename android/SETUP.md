# Android Development Environment Setup

This guide walks through setting up your system to build and test capp as an Android APK.

## Quick Start (Recommended)

Use Android Studio (easiest approach):

1. Download Android Studio: https://developer.android.com/studio
2. Install with default options (includes SDK, NDK, Gradle)
3. In Android Studio: **File > Open > [path to android/]**
4. Wait for Gradle sync
5. Click **Build > Build Bundle(s)/APK(s) > Build APK(s)**

Done! APK is in `app/build/outputs/apk/debug/app-debug.apk`

## Manual Setup (Command Line)

### Requirements

- **Java Development Kit (JDK)** — version 11 or later
- **Android SDK** — API level 34 minimum
- **Android NDK** — version 25.2.9519653 (or compatible)
- **Gradle** — version 8.2+

### 1. Install Java

**macOS (Homebrew)**
```bash
brew install openjdk@11
export JAVA_HOME=$(/usr/libexec/java_home -v 11)
```

**Ubuntu/Debian**
```bash
sudo apt update
sudo apt install openjdk-11-jdk-headless
```

**Windows**
- Download from: https://adoptium.net/
- Install and add to PATH

Verify:
```bash
java -version
javac -version
```

### 2. Install Android SDK

**Option A: Using Android Studio (Recommended)**

1. Install Android Studio: https://developer.android.com/studio
2. Open Android Studio
3. **Tools > SDK Manager**
4. **SDK Platforms** tab: Check "Android 14" (API 34)
5. **SDK Tools** tab: Check:
   - Android SDK Build-Tools 34.x
   - Android SDK Platform-Tools
   - CMake 3.22.1+
   - NDK (Side by side) 25.2.9519653

**Option B: Command Line Tools**

```bash
# Download SDK command-line tools from:
# https://developer.android.com/studio/command-line/sdkmanager

# Extract and add to PATH
export ANDROID_SDK_ROOT=~/Android/Sdk
export PATH=$ANDROID_SDK_ROOT/cmdline-tools/latest/bin:$PATH

# Install SDK components
sdkmanager --install "platforms;android-34" "build-tools;34.0.0"
sdkmanager --install "ndk;25.2.9519653"
sdkmanager --install "cmake;3.22.1"
```

### 3. Install Gradle

**Option A: Via Android Studio (Automatic)**
Android Studio includes and manages Gradle automatically.

**Option B: Homebrew (macOS)**
```bash
brew install gradle@8.2
```

**Option C: Manual Download**
1. Visit: https://gradle.org/releases/
2. Download Gradle 8.2
3. Extract to location (e.g., `~/gradle-8.2`)
4. Add to PATH:
   ```bash
   export PATH=~/gradle-8.2/bin:$PATH
   ```

Verify:
```bash
gradle --version
```

### 4. Set Up Environment Variables

Add to your shell profile (`.bashrc`, `.zshrc`, etc.):

**macOS/Linux**
```bash
export JAVA_HOME=$(/usr/libexec/java_home -v 11)  # macOS
# or: export JAVA_HOME=/usr/lib/jvm/java-11-openjdk  # Linux

export ANDROID_SDK_ROOT=~/Android/Sdk
export ANDROID_NDK_ROOT=$ANDROID_SDK_ROOT/ndk/25.2.9519653
export GRADLE_HOME=~/gradle-8.2  # if not using Homebrew

export PATH=$ANDROID_SDK_ROOT/tools:$PATH
export PATH=$ANDROID_SDK_ROOT/platform-tools:$PATH
export PATH=$GRADLE_HOME/bin:$PATH
```

**Windows (Command Prompt)**
```cmd
setx JAVA_HOME "C:\path\to\jdk11"
setx ANDROID_SDK_ROOT "C:\Users\YourName\AppData\Local\Android\Sdk"
setx ANDROID_NDK_ROOT "%ANDROID_SDK_ROOT%\ndk\25.2.9519653"

set PATH=%ANDROID_SDK_ROOT%\tools;%ANDROID_SDK_ROOT%\platform-tools;%PATH%
```

**Windows (PowerShell)**
```powershell
[Environment]::SetEnvironmentVariable("JAVA_HOME", "C:\path\to\jdk11", "User")
[Environment]::SetEnvironmentVariable("ANDROID_SDK_ROOT", "$env:LOCALAPPDATA\Android\Sdk", "User")
```

### 5. Initialize Gradle Wrapper

From the `android/` directory:

```bash
cd android

# If Gradle is installed:
gradle wrapper --gradle-version 8.2

# Or use the setup script:
./setup-gradle.sh
```

This creates:
- `gradle/wrapper/gradle-wrapper.jar`
- `gradle/wrapper/gradle-wrapper.properties`

Now you can use `./gradlew` instead of `gradle`.

### 6. Create local.properties

Copy and edit the example:

```bash
cp local.properties.example local.properties
```

Edit `local.properties` with your actual paths:

```properties
sdk.dir=/home/user/Android/Sdk
ndk.dir=/home/user/Android/Sdk/ndk/25.2.9519653
```

## Verification

Check that everything is set up correctly:

```bash
# Verify Java
java -version
javac -version

# Verify Android SDK
adb --version

# Verify Gradle (from android/ directory)
./gradlew --version

# Test build (will download dependencies first time)
./gradlew assembleDebug
```

Expected: All commands succeed with no errors.

## Building the APK

Once setup is complete:

```bash
cd android
./gradlew assembleDebug
```

APK output: `app/build/outputs/apk/debug/app-debug.apk`

## Troubleshooting

### "JAVA_HOME not set"
```bash
# Find your Java installation
java -XshowSettings:properties -version 2>&1 | grep java.home

# Set JAVA_HOME to that path
export JAVA_HOME=/path/from/above
```

### "Android SDK not found"
```bash
# Create or update local.properties
echo "sdk.dir=$ANDROID_SDK_ROOT" > local.properties
echo "ndk.dir=$ANDROID_NDK_ROOT" >> local.properties
```

### "NDK not found"
```bash
# Check NDK versions available
ls $ANDROID_SDK_ROOT/ndk/

# Update local.properties with available version
echo "ndk.dir=$ANDROID_SDK_ROOT/ndk/[VERSION]" >> local.properties
```

### "Could not determine Java version"
Update Java to JDK 11+:
```bash
# macOS
brew tap adoptopenjdk/openjdk
brew install adoptopenjdk11

# Ubuntu
sudo apt install openjdk-11-jdk
```

### "Gradle wrapper JAR not found"
```bash
# Recreate the wrapper
gradle wrapper --gradle-version 8.2

# Or specify Gradle installation
GRADLE_HOME=/path/to/gradle-8.2 ./gradlew tasks
```

## Next Steps

1. Review [TESTING.md](TESTING.md) for build and test procedures
2. See [README.md](README.md) for build command reference
3. For more info, see [Gradle documentation](https://docs.gradle.org)

## Additional Resources

- [Android Studio Documentation](https://developer.android.com/studio/intro)
- [Android NDK Build Guide](https://developer.android.com/ndk/guides)
- [SDL2 Android Port](https://github.com/libsdl-org/SDL/blob/main/docs/README-android.md)
- [Gradle User Manual](https://docs.gradle.org/8.2/userguide/userguide.html)
