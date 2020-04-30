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

#include "document.h"

ASDocument::ASDocument(ASTransactionList * transactions,
                       ASTransaction * overrides) :
    ASTransaction(transactions, overrides),
    m_recurring(false)
{
}

bool ASDocument::setDocumentDate(const QDate& date)
{
    CHECK_COMMITED;

    m_date = date;
    return true;
}

const QDate ASDocument::getDocumentDate() const
{
    return m_date;
}

bool ASDocument::setNumber(const QString& number)
{
    CHECK_COMMITED;

    m_number = number;
    return true;
}

const QString ASDocument::getNumber() const
{
    return m_number;
}

bool ASDocument::setId(const QString& id)
{
    CHECK_COMMITED;

    m_id = id;
    return true;
}

const QString ASDocument::getId() const
{
    return m_id;
}

bool ASDocument::setRecurring(bool recurring)
{
    CHECK_COMMITED;

    m_recurring = recurring;
    return true;
}

bool ASDocument::getRecurring() const
{
    return m_recurring;
}

void ASDocument::writeToXml(QDomDocument * doc, QDomElement * de)
{
    ASTransaction::writeToXml(doc, de);

    if (!getDocumentDate().isNull())
    {
        QDomElement e = doc->createElement("documentdate");
        QDomText t = doc->createTextNode(getDocumentDate().toString(Qt::ISODate));
        e.appendChild(t);
        de->appendChild(e);
    }
    if (!getNumber().isEmpty())
    {
        QDomElement e = doc->createElement("number");
        QDomText t = doc->createTextNode(getNumber());
        e.appendChild(t);
        de->appendChild(e);
    }
    if (!getId().isEmpty())
    {
        QDomElement e = doc->createElement("id");
        QDomText t = doc->createTextNode(getId());
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getRecurring())
    {
        QDomElement e = doc->createElement("recurring");
        de->appendChild(e);
    }
}

void ASDocument::handleDomElement(QDomElement * de)
{
    QString n = de->tagName();
    if (n == "documentdate")
    {
        setDocumentDate(parseDate(de->text()));
    } else if (n == "number")
    {
        setNumber(de->text());
    } else if (n == "id")
    {
        setId(de->text());
    }
    else if (n == "recurring")
    {
        setRecurring(true);
    }
    else
    {
        ASTransaction::handleDomElement(de);
    }
}
