#include "MainWindow.h"
#include <qapplication.h>

int main (int argc, char** argv)
{
    QApplication app (argc, argv);
    MainWindow* window = new MainWindow (0);
    app.setMainWidget (window);
    window->show();
    return app.exec();
}
