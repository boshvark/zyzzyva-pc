#include "MainWindow.h"
#include "DAWG.h"
#include <qapplication.h>
#include <qfile.h>

int main (int argc, char** argv)
{
    DAWG graph;

    QFile file ("/home/mthelen/scrabble/svn/ospd3/all-words.txt");
    if (!file.open (IO_ReadOnly))
        qFatal ("Can't open file!");

    int i = 0;
    QString word;
    while (file.readLine (word, 20) > 0) {
        word = word.stripWhiteSpace();
        graph.addWord (word);
        ++i;
        if ((i % 1000) == 0)
            qDebug ("Added " + QString::number (i) + " words to the graph.");
    }

    graph.print();

//    QApplication app (argc, argv);
//    StudyWindow* window = new StudyWindow (0);
//    app.setMainWidget (window);
//    window->show();
//    return app.exec();
}
