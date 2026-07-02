# Testing Android APK Build

This document outlines how to test and validate the Android APK build system.

## Prerequisites Checklist

Before testing, ensure you have:

- [ ] Android Studio installed OR Android SDK command-line tools
- [ ] Android NDK (via Android Studio SDK Manager or standalone)
- [ ] JDK 11 or later
- [ ] Gradle installed or available via Android Studio

## Testing Workflow

### Step 1: Set Up Gradle Wrapper

If this is your first time, initialize the Gradle wrapper:

```bash
cd android

# Option A: If you have Gradle installed
gradle wrapper --gradle-version 8.2

# Option B: Run the setup script
./setup-gradle.sh

# Option C: Use Android Studio's embedded Gradle
# (Android Studio automatically manages the wrapper)
```

### Step 2: Verify Build Configuration

Check that the build files are properly formatted:

```bash
# From android/ directory
./gradlew tasks
```

This should list all available tasks without errors.

### Step 3: Build Debug APK

```bash
./gradlew assembleDebug
```

**Expected output:**
- Build completes successfully
- APK generated at: `app/build/outputs/apk/debug/app-debug.apk`
- Size should be ~5-50 MB depending on dependencies

### Step 4: Verify APK

```bash
# Check APK exists and is a valid ZIP
unzip -t app/build/outputs/apk/debug/app-debug.apk | head -20

# List native libraries included
unzip -l app/build/outputs/apk/debug/app-debug.apk | grep "lib.*\.so"
```

**Expected native libraries:**
- `lib/armeabi-v7a/libc++_shared.so` (or similar STL)
- `lib/armeabi-v7a/libcapp_native.so` (your app code)
- `lib/armeabi-v7a/libSDL2.so` (SDL2 library)
- Similar entries for arm64-v8a, x86, x86_64 if configured

### Step 5: Install and Run on Device (Optional)

If you have an Android device or emulator:

```bash
# Start Android emulator or connect device
adb devices

# Install APK
./gradlew installDebug

# Launch app
adb shell am start -n com.capp.app/.MainActivity

# Monitor logs
adb logcat | grep capp
```

**Expected behavior:**
- App launches and displays checkerboard animation
- No native crashes (check logcat)
- Window resizes properly on Termux-X11 (if applicable)

### Step 6: Build Release APK (Optional)

```bash
./gradlew assembleRelease
```

Note: Requires signing configuration (see android/README.md)

## Troubleshooting

### "gradle-wrapper.jar not found"
```bash
# Initialize wrapper:
gradle wrapper --gradle-version 8.2
# or use setup-gradle.sh
```

### "SDK not found" or "NDK not found"
```bash
# Create local.properties with correct paths:
cp local.properties.example local.properties
# Edit with your Android SDK and NDK locations
```

### Build fails with native compilation errors
- Check `CMakeLists.android.txt` for correct paths
- Verify SDL2 is available in NDK or bundled correctly
- Check NDK version matches (8.2 configured for API 34)

### APK installs but app crashes
- Check logcat: `adb logcat | grep -i "capp\|SDL"`
- Verify all native libraries compiled successfully
- Check that SDL2 is properly linked

## File Validation

### Key Files Checklist

Verify these files are present:

```bash
# Configuration
✓ build.gradle              (top-level config)
✓ build.gradle.kts          (alternative Kotlin config)
✓ app/build.gradle.kts      (app-level config)
✓ settings.gradle.kts       (project structure)
✓ gradle.properties         (Gradle settings)

# Android
✓ app/src/main/AndroidManifest.xml        (app manifest)
✓ app/src/main/java/com/capp/app/MainActivity.java

# Native build
✓ CMakeLists.android.txt    (native code build)
✓ ../src/lua/main_embed_lua.c
✓ ../src/lua/lua_sdl.c
✓ ../src/lua/minilua/minilua.c

# Gradle wrapper
✓ gradlew                   (Unix launcher script)
✓ gradlew.bat               (Windows launcher)
✓ gradle/wrapper/gradle-wrapper.properties
✓ gradle/wrapper/gradle-wrapper.jar       (generated, not in repo)
```

## Success Criteria

The Android build setup is working correctly when:

1. ✅ `./gradlew tasks` runs without errors
2. ✅ `./gradlew assembleDebug` produces an APK
3. ✅ APK contains native libraries (`lib/*/libcapp_native.so`)
4. ✅ APK installs on device/emulator without errors
5. ✅ App launches and displays graphics (if device available)

## Next Steps

After successful APK build:
1. Test on different Android versions (min API 21 is configured)
2. Test on different architectures (armeabi-v7a, arm64-v8a, x86, x86_64)
3. Set up CI/CD pipeline (GitHub Actions, GitLab CI, etc.)
4. Create release signing configuration for Google Play
5. Customize app icon and branding
