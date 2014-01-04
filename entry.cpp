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

#include "entry.h"

ASAccount::ASAccount(ASTransactionList * transactions,
                     ASAccount * overrides) :
    ASTransaction(transactions, overrides)
{
}

bool ASAccount::setIban(const QString& iban)
{
    CHECK_COMMITED;

    m_iban = iban;
    return true;
}

QString ASAccount::getIban() const
{
    return m_iban;
}

bool ASAccount::setBic(const QString& bic)
{
    CHECK_COMMITED;

    m_bic = bic;
    return true;
}

QString ASAccount::getBic() const
{
    return m_bic;
}

void ASAccount::writeToXml(QDomDocument * doc, QDomElement * de)
{
    ASTransaction::writeToXml(doc, de);
    if (!getIban().isEmpty())
    {
        QDomElement desc = doc->createElement("iban");
        QDomText t = doc->createTextNode(getIban());
        desc.appendChild(t);
        de->appendChild(desc);
    }
    if (!getBic().isEmpty())
    {
        QDomElement desc = doc->createElement("bic");
        QDomText t = doc->createTextNode(getBic());
        desc.appendChild(t);
        de->appendChild(desc);
    }
}

void ASAccount::handleDomElement(QDomElement * de)
{
    if (de->tagName() == "iban")
    {
        setIban(de->text());
    }
    else if (de->tagName() == "bic")
    {
        setBic(de->text());
    }
    else
    {
        ASTransaction::handleDomElement(de);
    }
}

/********************************************************************/

ASCategory::ASCategory(ASTransactionList * transactions,
                       ASCategory * overrides) :
    ASTransaction(transactions, overrides)
{
}

/********************************************************************/

ASVatCategory::ASVatCategory(ASTransactionList * transactions,
                             ASVatCategory * overrides) :
    ASTransaction(transactions, overrides),
    m_parent(NULL),
    m_vatPercentage(0.0)
{
}

bool ASVatCategory::setParent(ASVatCategory * parent)
{
    CHECK_COMMITED;

    m_parent = parent;
    return true;
}

const ASVatCategory * ASVatCategory::getParent() const
{
    return m_parent;
}

bool ASVatCategory::setVatPercentage(double vatPercentage)
{
    CHECK_COMMITED;

    m_vatPercentage = vatPercentage;
    return true;
}

double ASVatCategory::getVatPercentage()
{
    return m_vatPercentage;
}

void ASVatCategory::writeToXml(QDomDocument * doc, QDomElement * de)
{
    ASTransaction::writeToXml(doc, de);
    if (getParent())
    {
        QDomElement par = doc->createElement("parent");
        QDomText t = doc->createTextNode(getParent()->getUuid());
        par.appendChild(t);
        de->appendChild(par);
    }
    QDomElement vap = doc->createElement("percentage");
    QDomText t = doc->createTextNode(QString::number(getVatPercentage()));
    vap.appendChild(t);
    de->appendChild(vap);
}

void ASVatCategory::handleDomElement(QDomElement * de)
{
    if (de->tagName() == "parent")
    {
        setParent(dynamic_cast<ASVatCategory *>
                  (m_transactions->getByUuid(de->text())));
    }
    else if (de->tagName() == "percentage")
    {
        setVatPercentage(de->text().toDouble());
    }
    else
    {
        ASTransaction::handleDomElement(de);
    }
}

/********************************************************************/

ASAccountEntry::ASAccountEntry(ASTransactionList * transactions,
                               ASTransaction * overrides) :
    ASTransaction(transactions, overrides),
    m_amount(0.0),
    m_vatAmount(0.0),
    m_chargePercentage(100.0),
    m_vatTaxableBase(false),
    m_account(NULL),
    m_category(NULL),
    m_vatCategory(NULL),
    m_document(NULL)
{
}

bool ASAccountEntry::setAmount(double amount)
{
    CHECK_COMMITED;

    m_amount = amount;
    return true;
}

double ASAccountEntry::getAmount() const
{
    return m_amount;
}

bool ASAccountEntry::setVatAmount(double vatAmount)
{
    CHECK_COMMITED;

    m_vatAmount = vatAmount;
    return true;
}

double ASAccountEntry::getVatAmount() const
{
    return m_vatAmount;
}

bool ASAccountEntry::setChargePercentage(double chargePercentage)
{
    CHECK_COMMITED;

    m_chargePercentage = chargePercentage;
    return true;
}

double ASAccountEntry::getChargePercentage() const
{
    return m_chargePercentage;
}

bool ASAccountEntry::setVatTaxableBase(bool vatTaxableBase)
{
    CHECK_COMMITED;

    m_vatTaxableBase = vatTaxableBase;
    return true;
}

bool ASAccountEntry::getVatTaxableBase() const
{
    return m_vatTaxableBase;
}

int ASAccountEntry::getVatPercentage() const
{
    if (getAmount() == 0.0 || getVatAmount() == 0.0)
        return -1;

    double vp = 100.0 * (getVatAmount() / getAmount());

    return qRound(vp);
}

bool ASAccountEntry::setAccount(const ASAccount * account)
{
    CHECK_COMMITED;
    CHECK_NOT_COMMITED(account);

    m_account = account;
    return true;
}

const ASAccount * ASAccountEntry::getAccount() const
{
    return m_account;
}

bool ASAccountEntry::setCategory(const ASCategory * category)
{
    CHECK_COMMITED;
    CHECK_NOT_COMMITED(category);

    m_category = category;
    return true;
}

const ASCategory * ASAccountEntry::getCategory() const
{
    return m_category;
}

bool ASAccountEntry::setVatCategory(const ASVatCategory * category)
{
    if (!category)
        return false;

    CHECK_COMMITED;
    CHECK_NOT_COMMITED(category);

    m_vatCategory = category;
    return true;
}

const ASVatCategory * ASAccountEntry::getVatCategory() const
{
    return m_vatCategory;
}

bool ASAccountEntry::setDocument(const ASDocument * document)
{
    CHECK_COMMITED;
    CHECK_NOT_COMMITED(document);

    m_document = document;
    return true;
}

const ASDocument * ASAccountEntry::getDocument() const
{
    return m_document;
}

bool ASAccountEntry::setDate(const QDate& date)
{
    CHECK_COMMITED;

    m_date = date;
    return true;
}

const QDate ASAccountEntry::getDate() const
{
    return m_date;
}

ASAccountEntry ASAccountEntry::copy()
{
    if (!m_overrides)
    {
        ASAccountEntry e;
        return e;
    }

    ASAccountEntry e(m_transactions);

    e.m_amount = m_amount;
    e.m_chargePercentage = m_chargePercentage;
    e.m_account = m_account;
    e.m_category = m_category;
    e.m_document = m_document;
    e.m_date = m_date;

    return e;
}

void ASAccountEntry::writeToXml(QDomDocument * doc, QDomElement * de)
{
    ASTransaction::writeToXml(doc, de);
    if (getAmount() != 0.0)
    {
        QDomElement e = doc->createElement("amount");
        QDomText t = doc->createTextNode(QString::number(getAmount()));
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getVatAmount() != 0.0)
    {
        QDomElement e = doc->createElement("vatamount");
        QDomText t = doc->createTextNode(QString::number(getVatAmount()));
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getChargePercentage() != 0.0)
    {
        QDomElement e = doc->createElement("chargepercentage");
        QDomText t = doc->createTextNode(QString::number(getChargePercentage()));
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getVatTaxableBase())
    {
        QDomElement e = doc->createElement("vattaxablebase");
        de->appendChild(e); 
    }
    if (getAccount() != NULL)
    {
        QDomElement e = doc->createElement("account");
        QDomText t = doc->createTextNode(getAccount()->getUuid());
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getCategory() != NULL)
    {
        QDomElement e = doc->createElement("category");
        QDomText t = doc->createTextNode(getCategory()->getUuid());
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getVatCategory() != NULL)
    {
        QDomElement e = doc->createElement("vatcategory");
        QDomText t = doc->createTextNode(getVatCategory()->getUuid());
        e.appendChild(t);
        de->appendChild(e);
    }
    if (getDocument() != NULL)
    {
        QDomElement e = doc->createElement("document");
        QDomText t = doc->createTextNode(getDocument()->getUuid());
        e.appendChild(t);
        de->appendChild(e);
    }
    if (!getDate().isNull())
    {
        QDomElement e = doc->createElement("date");
        QDomText t = doc->createTextNode(getDate().toString());
        e.appendChild(t);
        de->appendChild(e);
    }
}

void ASAccountEntry::handleDomElement(QDomElement * de)
{
    QString n = de->tagName();
    if (n == "amount")
    {
        setAmount(de->text().toDouble());
    }
    else if (n == "vatamount")
    {
        setVatAmount(de->text().toDouble());
    }
    else if (n == "chargepercentage")
    {
        setChargePercentage(de->text().toDouble());
    }
    else if (n == "vattaxablebase")
    {
        setVatTaxableBase(true);
    }
    else if (n == "account")
    {
        setAccount(dynamic_cast<ASAccount*>
                   (m_transactions->getByUuid(de->text())));
    }
    else if (n == "category")
    {
        setCategory(dynamic_cast<ASCategory*>
                    (m_transactions->getByUuid(de->text())));
    }
    else if (n == "vatcategory")
    {
        setVatCategory(dynamic_cast<ASVatCategory*>
                       (m_transactions->getByUuid(de->text())));
    }
    else if (n == "document")
    {
        setDocument(dynamic_cast<ASDocument*>
                    (m_transactions->getByUuid(de->text())));
    }
    else if (n == "date")
    {
        setDate(QDate::fromString(de->text()));
    }
    else
    {
        ASTransaction::handleDomElement(de);
    }
}

bool ASAccountEntry::operator>(const ASAccountEntry &other) const
{
    return m_date > other.m_date;
}

bool ASAccountEntry::operator<(const ASAccountEntry &other) const
{
    return m_date < other.m_date;
}
