#include <QtWidgets/QApplication>
#include "MyApp.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MyApp w;
    w.show();
    return a.exec();
}

//windeployqt.exe --quick .