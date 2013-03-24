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

#ifndef DOCUMENT_H
#define DOCUMENT_H

/**
 * Document (invoice/payment request) class
 */

#include <QDate>
#include "transaction.h"

class ASDocument : public ASTransaction
{
public:
    // Constructors

    ASDocument(ASTransactionList * transactions = NULL,
               ASTransaction * overrides = NULL);

    // Public functions

    bool setDocumentDate(const QDate& date);
    const QDate getDocumentDate() const;
    bool setNumber(const QString& id);
    const QString getNumber() const;
    bool setId(const QString& id);
    const QString getId() const;

    virtual TransactionType getType() {return TRANSACTION_TYPE_DOCUMENT;}
    virtual void writeToXml(QDomDocument * doc,
                            QDomElement * de);

protected:
    QDate m_date;
    QString m_number;
    QString m_id;

    virtual void handleDomElement(QDomElement * de);
};

#endif
