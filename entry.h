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

#ifndef ENTRY_H
#define ENTRY_H

/**
 * Account entry class
 */

#include "transaction.h"
#include "document.h"

class ASAccount : public ASTransaction
{
    // Constructors
    
public:
    ASAccount(ASTransactionList * transactions = NULL,
              ASAccount * overrides = NULL);

    // Public functions

    bool setIban(const QString& iban);
    QString getIban() const;
    bool setBic(const QString& bic);
    QString getBic() const;

    virtual TransactionType getType() {return TRANSACTION_TYPE_ACCOUNT;}
    virtual void writeToXml(QDomDocument * doc,
                            QDomElement * de);

protected:
    QString m_iban;
    QString m_bic;

    virtual void handleDomElement(QDomElement * de);
};

class ASCategory : public ASTransaction
{
public:
    ASCategory(ASTransactionList * transactions = NULL,
               ASCategory * overrides = NULL);

    virtual TransactionType getType() {return TRANSACTION_TYPE_CATEGORY;}
};

class ASAccountEntry : public ASTransaction
{
public:
    // Constructors

    ASAccountEntry(ASTransactionList * transactions = NULL,
               ASTransaction * overrides = NULL);

    // Public functions

    bool setAmount(double amount);
    double getAmount() const;
    bool setVatAmount(double vatAmount);
    double getVatAmount() const;
    bool setChargePercentage(double chargePercentage);
    double getChargePercentage() const;
    bool setVatPercentage(int vatPercentage);
    int getVatPercentage() const;
    bool setAccount(ASAccount * account);
    ASAccount * getAccount() const;
    bool setCategory(ASCategory * category);
    const ASCategory * getCategory() const;
    bool setDocument(ASDocument * document);
    const ASDocument * getDocument() const;
    bool setDate(const QDate& date);
    const QDate getDate() const;
    virtual ASAccountEntry copy();

    virtual TransactionType getType() {return TRANSACTION_TYPE_ACCOUNTENTRY;}
    virtual void writeToXml(QDomDocument * doc,
                            QDomElement * de);

    // Comparison operators
    virtual bool operator>(const ASAccountEntry &other) const;
    virtual bool operator<(const ASAccountEntry &other) const;

protected:
    double m_amount;
    double m_vatAmount;
    double m_chargePercentage;
    int m_vatPercentage;
    ASAccount * m_account;
    ASCategory * m_category;
    ASDocument * m_document;
    QDate m_date;

    virtual void handleDomElement(QDomElement * de);
};

#endif
