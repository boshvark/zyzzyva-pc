#include "StudyWindow.h"
#include <qframe.h>
#include <qlayout.h>
#include <qpushbutton.h>

const QString ANAGRAM_MODE = "Anagram";
const QString RECALL_LIST_MODE = "Recall List";
const QString CHOOSE_BUTTON_TEXT = "Choose...";

StudyWindow::StudyWindow (QWidget* parent, const char* name, WFlags f)
    : QMainWindow (parent, name, f)
{
    QFrame* frame = new QFrame (this);
    setCentralWidget (frame);

    QVBoxLayout* mainVlay = new QVBoxLayout (frame, 0, 0, "mainVlay");
    QHBoxLayout* modeHlay = new QHBoxLayout (0, "mainHlay");
    mainVlay->addLayout (modeHlay);
    QLabel* modeLabel = new QLabel ("Mode:", frame, "modeLabel");
    modeHlay->addWidget (modeLabel);
    modeCombo = new QComboBox (frame, "modeCombo");
    modeCombo->insertItem (ANAGRAM_MODE);
    modeCombo->insertItem (RECALL_LIST_MODE);
    modeHlay->addWidget (modeCombo);

    QHBoxLayout* fileHlay = new QHBoxLayout (0, "fileHlay");
    mainVlay->addLayout (fileHlay);
    QLabel* fileLabel = new QLabel ("Word List:", frame, "fileLabel");
    fileHlay->addWidget (fileLabel);

    QPushButton* fileButton = new QPushButton (CHOOSE_BUTTON_TEXT, frame,
                                               "fileButton");
    fileHlay->addWidget (fileButton);
    connect (fileButton, SIGNAL (clicked()), SLOT (FileButtonClicked()));
}

void
StudyWindow::FileButtonClicked()
{
    qDebug ("Button pressed!");
}
