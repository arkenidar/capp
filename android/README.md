# Android Build Guide

This directory contains the Android/Gradle project for building a native APK of capp.

## Quick Start

**Using Android Studio (Easiest):**

1. Install Android Studio: https://developer.android.com/studio
2. Open this `android/` directory with Android Studio
3. Wait for Gradle sync
4. Click **Build > Build Bundle(s)/APK(s) > Build APK(s)**

**From Command Line:**

```bash
cd android
./gradlew assembleDebug
```

APK will be at: `app/build/outputs/apk/debug/app-debug.apk`

## Setup Documentation

- **[SETUP.md](SETUP.md)** — Detailed environment setup (JDK, Android SDK, NDK, Gradle)
- **[TESTING.md](TESTING.md)** — Build validation and testing procedures

## Prerequisites

1. **Android Studio** (recommended) or Android SDK tools
2. **Android SDK** (API level 34)
3. **Android NDK** (version 25.2.9519653 or compatible)
4. **Java Development Kit** (JDK 11+)
5. **Gradle** (version 8.2+)

## Building the APK

### Using Android Studio

1. Open Android Studio
2. Select **File > Open** and browse to the `android/` directory
3. Wait for Gradle sync to complete
4. Select **Build > Build Bundle(s)/APK(s) > Build APK(s)**
5. APK will be generated in `android/app/build/outputs/apk/release/`

### Using Command Line (Gradle)

From the `android/` directory:

**First time setup (bootstrap Gradle wrapper):**

```bash
# If you have Gradle installed, initialize the wrapper:
gradle wrapper --gradle-version 8.2

# OR run the setup script (auto-detects Gradle):
./setup-gradle.sh

# OR install Gradle manually:
# - Ubuntu/Debian: sudo apt install gradle
# - macOS: brew install gradle
# - Windows: choco install gradle (via Chocolatey)
```

**Build APK:**

```bash
# Build debug APK
./gradlew assembleDebug

# Build release APK (requires signing configuration)
./gradlew assembleRelease

# Install on connected device
./gradlew installDebug
```

## Signing the APK (Release Build)

For release builds, you need to sign the APK:

1. Create a keystore:
   ```bash
   keytool -genkey -v -keystore capp.keystore -keyalg RSA -keysize 2048 -validity 10000 -alias capp_key
   ```

2. Create `android/app/keystore.properties`:
   ```properties
   storeFile=/path/to/capp.keystore
   storePassword=your_password
   keyAlias=capp_key
   keyPassword=your_key_password
   ```

3. Update `android/app/build.gradle.kts` to use the keystore (add to `buildTypes.release` block):
   ```kotlin
   signingConfig = signingConfigs.release
   ```

4. Build signed release:
   ```bash
   ./gradlew assembleRelease
   ```

## Installing and Running

### Install on Device

```bash
# via adb
adb install android/app/build/outputs/apk/debug/app-debug.apk

# or via gradlew
./gradlew installDebug
```

### Run on Emulator

1. Create/start Android Virtual Device (AVD)
2. Build and install APK:
   ```bash
   ./gradlew installDebug
   ```
3. Run:
   ```bash
   adb shell am start -n com.capp.app/.MainActivity
   ```

## Troubleshooting

### NDK Issues
- Ensure NDK version matches `gradle.properties`
- Download via Android Studio: **Tools > SDK Manager > SDK Tools > NDK (Side by side)**

### Build Fails
- Clean and rebuild:
  ```bash
  ./gradlew clean assembleDebug
  ```
- Check SDK versions match target API (34) in `build.gradle.kts`

### Can't Find SDL2
- SDL2 must be compiled for Android NDK
- The build script assumes SDL2 in NDK's third-party folder
- Otherwise, build SDL2 separately and update `CMakeLists.android.txt`

## Project Structure

```
android/
├── app/
│   ├── build.gradle.kts          # App-level Gradle config
│   ├── src/
│   │   └── main/
│   │       ├── AndroidManifest.xml
│   │       ├── java/com/capp/app/
│   │       │   └── MainActivity.java
│   │       └── res/
│   │           └── values/
│   │               └── strings.xml
│   └── proguard-rules.pro
├── build.gradle.kts               # Top-level Gradle config
├── settings.gradle.kts
├── gradle.properties
└── CMakeLists.android.txt         # Native code build config
```

## Next Steps

- Customize the app icon in `android/app/src/main/res/`
- Update package name from `com.capp.app` if desired (requires refactoring)
- Add additional Android-specific features as needed
