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

#ifndef TRANSACTION_H
#define TRANSACTION_H

/**
 * Transaction base class
 */

#include <QObject>
#include <QString>
#include <QHash>
#include <QList>
#include <QStringList>
#include <QUuid>
#include <QDateTime>
#include <QDomElement>

// Some macros

#define CHECK_COMMITED \
    if (m_committed) return false
#define CHECK_NOT_COMMITED(Object) \
    if (!(Object)->isCommited()) return false

// Classes

class ASTransaction;
//typedef QList<ASTransaction *> ASTransactionList;

class ASTransactionList : public QObject, public QList<ASTransaction *>
{
    Q_OBJECT

public:
    ASTransaction * getByUuid(const QUuid& uuid) const;
    void append(ASTransaction * value);

signals:
    void newTransaction();
};

class ASTransaction
{
public:

    // All transaction types

    enum TransactionType
    {
        TRANSACTION_TYPE_BASE,
        TRANSACTION_TYPE_CATEGORY,
        TRANSACTION_TYPE_ACCOUNT,
        TRANSACTION_TYPE_ACCOUNTENTRY,
        TRANSACTION_TYPE_INVESTMENT,
        TRANSACTION_TYPE_DOCUMENT,
        TRANSACTION_TYPE_VATCATEGORY
    };

    static QStringList TransactionTypeStrings;

    static inline void TTypeInit()
    {
        TransactionTypeStrings << "base";
        TransactionTypeStrings << "category";
        TransactionTypeStrings << "account";
        TransactionTypeStrings << "accountentry";
        TransactionTypeStrings << "investment";
        TransactionTypeStrings << "document";
        TransactionTypeStrings << "vatcategory";
    };

    // Constructors

    ASTransaction(ASTransactionList * transactions = NULL,
                  ASTransaction * overrides = NULL);

    // Destructor

    virtual ~ASTransaction();

    // Public functions

    bool setDescription(const QString& description);
    const QString getDescription() const;
    bool setHidden();
    bool getHidden() const {return m_hidden;}

    const QDateTime getCreationTimestamp() const;
    const QUuid getUuid() const;
    const ASTransaction * getOverride() const;
    const ASTransaction * getOverwrittenBy() const;
    const ASTransaction * getLatest() const;
    bool isNull() const;
    bool isCommited() const;

    void commit();

    virtual TransactionType getType() {return TRANSACTION_TYPE_BASE;}
    virtual void writeToXml(QDomDocument * doc,
                            QDomElement * de);
    void readFromXml(QDomElement * de);

    // Comparison operators
    virtual bool operator>(const ASTransaction &other) const;
    virtual bool operator<(const ASTransaction &other) const;

    static QDate parseDate(const QString& dateString);

    // Public members

protected:
    QString m_description;
    QDateTime m_creationTimestamp;
    QUuid m_uuid;
    ASTransaction * m_overrides;
    ASTransaction * m_overwrittenBy;
    ASTransactionList * m_transactions;
    bool m_hidden;
    bool m_committed;

    virtual void handleDomElement(QDomElement * de);
};

#endif
