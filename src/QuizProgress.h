//---------------------------------------------------------------------------
// QuizProgress.h
//
// A class to represent quiz progress.
//
// Copyright 2005 Michael W Thelen <mike@pietdepsi.com>.
//
// This file is part of Zyzzyva.
//
// Zyzzyva is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// Zyzzyva is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//---------------------------------------------------------------------------

#ifndef QUIZ_PROGRESS_H
#define QUIZ_PROGRESS_H

#include <qdom.h>
#include <qmap.h>

class QuizProgress
{
    public:
    QuizProgress (int q = 1, int c = 0, int i = 0, int m = 0);
    ~QuizProgress() { }

    void addIncorrect (const QString& word, int count);
    void addMissed (const QString& word, int count);

    QDomElement asDomElement() const;
    bool fromDomElement (const QDomElement& element);

    private:
    int question;
    int correct;
    int incorrect;
    int missed;
    QMap<QString, int> incorrectWords;
    QMap<QString, int> missedWords;
};

#endif // QUIZ_PROGRESS_H

