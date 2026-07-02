# SDL2 rules
-keep class org.libsdl.app.** { *; }

# Keep JNI methods
-keepclasseswithmembernames class * {
    native <methods>;
}

# Keep main activity
-keep class com.capp.app.MainActivity { *; }
