# QStyleHelper
A Helper class for managing QStyle, QPalette, TitleBar Color on Windows and auto detect color scheme changes.



https://user-images.githubusercontent.com/12413639/216264921-933c2670-b1d5-4b67-8624-c5c8aaf39ecd.mp4

https://user-images.githubusercontent.com/12413639/216384897-b8f63c3a-2658-4637-acaa-d85e74e75869.mp4




## Usage
### main.cpp
```C++
#include "mainwindow.h"
#include "qstylehelper.hpp"
#include <QApplication>


int main(int argc, char *argv[])
{
    // this function uses new Qt >= 5.15 win32 dark titlebar environment 
    QStyleHelper::setTitleBarDarkColor();

    QApplication a(argc, argv);
    MainWindow w;
    
    // if your Qt version older than 5.15 use it like this for win32 dark titlebar environment and also you have to call once for any subwindows 
    QStyleHelper::setTitleBarDarkColor(w);
    
    // initialize the instance and set desired look you want, also you can set any custom QPalette for dark and light scheme separately 
    QStyleHelper::instance().setDarkPalette().setWidgetStyle("fusion").setAutoChangePalette(true);
    

    // this connection and QStyleHelper::colorSchemeChanged signal for monitor windows dark/light mode changes
    QObject::connect(&QStyleHelper::instance(), &QStyleHelper::colorSchemeChanged, [&w](bool dark) { QStyleHelper::setTitleBarDarkColor(w, dark); });

    w.show();

    return a.exec();
}
```
