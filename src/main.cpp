#include "MainWindow.h"
#include <qapplication.h>

int main (int argc, char** argv)
{
    QApplication app (argc, argv);
    MainWindow window (0);
    app.setMainWidget (&window);
    window.show();
    return app.exec();
}
