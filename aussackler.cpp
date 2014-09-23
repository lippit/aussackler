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
#include <QFile>
#include <stdio.h>
#include "aussackler.h"
#include "entry.h"
#include "mainwindow.h"
#include "transaction.h"

Aussackler::Aussackler()
{
    ASTransaction::TTypeInit();

    ASMainWindow * mw = new ASMainWindow(&m_transactions);

    connect(mw, SIGNAL(signalLoad(const QString)),
            this, SLOT(slotLoadFile(const QString)));
    connect(mw, SIGNAL(signalSave(const QString)),
            this, SLOT(slotSaveFile(const QString)));

    mw->show();
}

Aussackler::~Aussackler()
{
}

void Aussackler::slotLoadFile(const QString fileName)
{
    QDomDocument doc("aussackler");
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly))
        return;
    if (!doc.setContent(&file)) {
        file.close();
        return;
    }
    file.close();

    for (int i = m_transactions.size() - 1; i >= 0; --i)
    {
        delete m_transactions.at(i);
    }
    m_transactions.clear();

    QDomElement docElem = doc.documentElement();

    QDomNode n = docElem.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(!e.isNull() && e.tagName() == "transaction")
        {
            if (!e.hasAttribute("type"))
                continue;
            ASTransaction::TransactionType type =
                (ASTransaction::TransactionType)
                ASTransaction::TransactionTypeStrings.
                indexOf(e.attribute("type"));
            ASTransaction * a = NULL;
            switch(type)
            {
            case ASTransaction::TRANSACTION_TYPE_BASE:
                a = new ASTransaction(&m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_CATEGORY:
                a = new ASCategory(&m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_ACCOUNT:
                a = new ASAccount(&m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_ACCOUNTENTRY:
                a = new ASAccountEntry(&m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_INVESTMENT:
                a = new ASInvestEntry(&m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_DOCUMENT:
                a = new ASDocument(&m_transactions);
                break;
            case ASTransaction::TRANSACTION_TYPE_VATCATEGORY:
                a = new ASVatCategory(&m_transactions);
                break;
            }
            if (a)
            {
                a->readFromXml(&e);
                a->commit();
            }
        }
        n = n.nextSibling();
    }
}

void Aussackler::slotSaveFile(const QString fileName)
{
    QDomDocument doc("aussackler");
    QDomElement root = doc.createElement("aussackler");
    doc.appendChild(root);
    ASTransactionList::const_iterator it = m_transactions.constBegin();
    for(; it != m_transactions.constEnd(); ++it)
    {
        QDomElement de = doc.createElement("transaction");
        (*it)->writeToXml(&doc, &de);
        root.appendChild(de);
    }

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;
    file.write(doc.toByteArray(4));
    file.close();
}

/********************************************************************/

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    QLocale::setDefault(QLocale(QLocale::German, QLocale::Austria));

    Aussackler as;
    if (argc > 1)
        as.slotLoadFile(argv[1]);

    app.exec();

    return 0;
}
