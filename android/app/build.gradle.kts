plugins {
    id("com.android.application") version "8.7.0"
}

android {
    namespace = "com.capp.app"
    compileSdk = 34
    ndkVersion = "30.0.14904198"

    defaultConfig {
        applicationId = "com.capp.app"
        minSdk = 21
        targetSdk = 34
        versionCode = 1
        versionName = "1.0.0"

        ndk {
            abiFilters.addAll(listOf("armeabi-v7a", "arm64-v8a", "x86", "x86_64"))
        }
    }

    buildTypes {
        release {
            isMinifyEnabled = false
            proguardFiles(getDefaultProguardFile("proguard-android-optimize.txt"), "proguard-rules.pro")
        }
    }

    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_11
        targetCompatibility = JavaVersion.VERSION_11
    }

    sourceSets {
        main {
            java.srcDirs += "${project.rootDir}/../SDL2/android-project/app/src/main/java"
        }
    }

    externalNativeBuild {
        cmake {
            path = file("../CMakeLists.txt")
            version = "3.22.1"
            arguments.add("-DANDROID_SDL2_PATH=${project.rootDir}/../SDL2")
        }
    }

    packaging {
        resources {
            excludes += listOf("META-INF/NOTICE.txt", "META-INF/LICENSE.txt")
        }
    }
}

dependencies {
    // Android Support Libraries
    implementation("androidx.appcompat:appcompat:1.6.1")
    implementation("androidx.constraintlayout:constraintlayout:2.1.4")
}

// Keep assets/app.lua and assets/fonts in sync with the toolkit's source of
// truth (src/lua/apps/ and the repo-root assets/ dir) instead of hand
// maintaining a second copy that can go stale. Mirrors the desktop build's
// CMake POST_BUILD copy step in ../../CMakeLists.txt.
val syncToolkitApp = tasks.register<Copy>("syncToolkitApp") {
    from("${project.rootDir}/../src/lua/apps/memory_game.lua")
    rename("memory_game.lua", "app.lua")
    from("${project.rootDir}/../assets/fonts") {
        into("fonts")
    }
    into("src/main/assets")
}

tasks.named("preBuild") {
    dependsOn(syncToolkitApp)
}
