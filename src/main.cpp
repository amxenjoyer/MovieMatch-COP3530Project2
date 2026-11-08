
#include <QApplication>
#include "../include/MovieWindow.h"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MovieWindow window;
    window.show();

    return app.exec();
}
