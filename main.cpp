
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("MovieMatch");
    window.resize(1000, 700);
    window.show();

    return app.exec();
}
