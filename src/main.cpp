#include "StudyWindow.h"
#include <qapplication.h>

int main (int argc, char** argv)
{
    QApplication app (argc, argv);
    StudyWindow* window = new StudyWindow (0);
    app.setMainWidget (window);
    window->show();
    return app.exec();
}
