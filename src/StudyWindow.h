#ifndef STUDY_WINDOW_H
#define STUDY_WINDOW_H

#include <qcombobox.h>
#include <qlabel.h>
#include <qmainwindow.h>

class StudyWindow : public QMainWindow
{
  Q_OBJECT
  public:
    StudyWindow (QWidget* parent = 0, const char* name = 0,
                 WFlags f = WType_TopLevel);

  public slots:
    void Open();

  private:
    QComboBox* modeCombo;
    QLabel*    label;
};

#endif // STUDY_WINDOW_H
