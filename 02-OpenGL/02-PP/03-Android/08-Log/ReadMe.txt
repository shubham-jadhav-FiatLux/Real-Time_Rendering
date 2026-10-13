
gradlew.bat clean

gradlew.bat build

adb -d install -r app\build\outputs\apk\debug\app-debug.apk


// Log related commands

adb logcat

adb logcat -c		// For cleaning

adb logcat | findstr /i "SBJ:"	// Finding string