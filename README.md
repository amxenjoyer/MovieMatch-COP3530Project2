## Qt 6 Setup Instructions

1. Installing Qt

1. Go to https://doc.qt.io/qt-6/qt-online-installation.html
2. Download the Qt Online Installer.
3. Create a Qt account or sign in.
4. Select the open-source installation option.
5. Install Qt 6 with the MinGW 64-bit desktop kit

2. Setting Up Qt in CLion

1. Open the MovieMatch project in CLion.
2. Click File then Settings.
3. Select Build, Execution, Deployment, then CMake.
4. Find the CMake options field.
5. Enter the following:

-DCMAKE_PREFIX_PATH=C:/Qt/6.12.0/mingw_64

6. If Qt was installed somewhere else change the PATH
7. Click Apply then OK.
8. Click Tools then CMake then Reload CMake Project.

### 3. Setting Up Environment Variables

If the project builds but the window does not open, the Qt DLL files may not be found.

1. Click Run then Edit Configurations.
2. Select the MovieMatch configuration.
3. Find Environment variables.
4. Add the Qt bin directory to the PATH variable:

C:\Qt\6.12.0\mingw_64\bin;%PATH%

5. Click Apply, then OK.

Make sure CLion is using a compatible 64-bit MinGW compiler.

### 4. Running MovieMatch

Due to github file size limits the data used by this project cannot be included in the repository.
Please download and extract the ml-32m.zip file from https://grouplens.org/datasets/movielens/32m/

1. Place the movies.csv and ratings.csv files into the provided /data directory.
2. Reload CMake.
3. Build the project using the hammer icon.
4. Select MovieMatch and click the green Run button.
5. The graphical interface should open and load the movie data.

### 5. Common Errors

If CMake cannot find Qt6Config.cmake, check that Qt is installed correctly and that CMAKE_PREFIX_PATH points to the correct folder.

If the program exits with code -1073741515, check that the Qt bin directory is included in the PATH environment variable.

If the project reports compiler compatibility errors, make sure the MinGW compiler in CLion is compatible with the Qt version installed.

### Additional Setup Resources

Qt Installation Guide:
https://doc.qt.io/qt-6/qt-online-installation.html

CLion Qt Tutorial:
https://www.jetbrains.com/help/clion/qt-tutorial.html
