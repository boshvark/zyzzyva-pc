#include "StudyWindow.h"
#include "DAWG.h"
#include <qapplication.h>

int main (int argc, char** argv)
{
    DAWG graph;

    graph.addWord ("a");
    graph.addWord ("foobar");
    graph.addWord ("food");
    graph.addWord ("foods");
    graph.addWord ("cat");
    graph.addWord ("folly");
    graph.addWord ("codename");
    graph.addWord ("fool");

    graph.print();

//    QApplication app (argc, argv);
//    StudyWindow* window = new StudyWindow (0);
//    app.setMainWidget (window);
//    window->show();
//    return app.exec();
}
