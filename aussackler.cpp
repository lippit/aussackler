/*
 * Copyright (c) 2013 Georg Lippitsch
 *
 * This file is part of Aussackler.
 *
 * Aussackler is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aussackler is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Aussackler.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <QApplication>
#include <QTextCodec>
#include <QDate>
#include <stdio.h>
#include "aussackler.h"
#include "entry.h"
#include "mainwindow.h"
#include "transaction.h"

Aussackler::Aussackler()
{
    ASTransaction::TTypeInit();

    ASMainWindow * mw = new ASMainWindow(&m_transactions);
    mw->show();
}

Aussackler::~Aussackler()
{
}

/********************************************************************/

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    Aussackler as;

    app.exec();

    return 0;
}
