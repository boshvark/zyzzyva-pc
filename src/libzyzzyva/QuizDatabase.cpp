//---------------------------------------------------------------------------
// QuizDatabase.cpp
//
// A class for working with the database for a saved quiz.
//
// Copyright 2012 Boshvark Software, LLC.
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

#include "QuizDatabase.h"
#include "Auxil.h"
#include <QDateTime>
#include <QSqlDatabase>

//const QString SQL_CREATE_QUESTIONS_TABLE_CURRENT =
//    "CREATE TABLE questions (question varchar(16), correct integer, "
//    "incorrect integer, streak integer, last_correct integer, "
//    "difficulty integer, cardbox integer, next_scheduled integer)";

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Constructor.  Connect to the database specified by a lexicon and quiz
//! type.
//
//! @param fileName the database file name
//---------------------------------------------------------------------------
QuizDatabase::QuizDatabase(const QString& fileName)
    : db(0), dbFileName(fileName)
{
    // Get random connection name
    rng.srand(QDateTime::currentDateTime().toTime_t(), Auxil::getPid());
    unsigned int r = rng.rand();
    dbConnectionName = "quiz" + QString::number(r);
    db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE",
        dbConnectionName));
    db->setDatabaseName(dbFileName);
    //if (!db->open())
    //    return;
}

//---------------------------------------------------------------------------
//  QuizDatabase
//
//! Destructor.
//---------------------------------------------------------------------------
QuizDatabase::~QuizDatabase()
{
}

//---------------------------------------------------------------------------
//  isValid
//
//! Determine whether the database connection is valid.
//
//! @return true if the database is valid, false otherwise
//---------------------------------------------------------------------------
bool
QuizDatabase::isValid() const
{
    return (db && db->isValid());
}

//---------------------------------------------------------------------------
//  databaseFile
//
//! Return the database file name.
//
//! @return the database file name
//---------------------------------------------------------------------------
QString
QuizDatabase::databaseFile() const
{
    return dbFileName;
}
