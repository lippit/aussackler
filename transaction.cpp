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

#include "transaction.h"

QStringList ASTransaction::TransactionTypeStrings;

ASTransaction * ASTransactionList::getByUuid(const QUuid& uuid) const
{
    for(const_iterator it = constBegin(); it != constEnd(); ++it)
    {
        if ((*it)->getUuid() == uuid)
        {
            return *it;
        }
    }
    return NULL;
}

void ASTransactionList::append(ASTransaction * value)
{
    if (!value->isCommited())
        return;

    QList::append(value);

    emit newTransaction();
}

/********************************************************************/

ASTransaction::ASTransaction(ASTransactionList * transactions,
                             ASTransaction * overrides) :
    m_overwrittenBy(NULL),
    m_transactions(transactions),
    m_hidden(false),
    m_committed(false)
{
    if (m_transactions)
    {
        m_uuid = QUuid::createUuid();
        m_creationTimestamp = QDateTime::currentDateTime();
        m_overrides = overrides;
        if (overrides)
        {
            overrides->m_overwrittenBy = this;
        }
    }
}

ASTransaction::~ASTransaction()
{
}

bool ASTransaction::setDescription(const QString& description)
{
    CHECK_COMMITED;

    m_description = description;
    return true;
}

bool ASTransaction::setHidden()
{
    CHECK_COMMITED;

    m_hidden = true;
    return true;
}

const QString ASTransaction::getDescription() const
{
    return m_description;
}

const QDateTime ASTransaction::getCreationTimestamp() const
{
    return m_creationTimestamp;
}

const QUuid ASTransaction::getUuid() const
{
    return m_uuid;
}

const ASTransaction * ASTransaction::getOverride() const
{
    return m_overrides;
}

const ASTransaction * ASTransaction::getOverwrittenBy() const
{
    return m_overwrittenBy;
}

const ASTransaction * ASTransaction::getLatest() const
{
    const ASTransaction * latest = this;
    while (latest->m_overwrittenBy)
    {
        latest = latest->m_overwrittenBy;
    }
    return latest;
}

bool ASTransaction::isNull() const
{
    return m_uuid.isNull();
}

bool ASTransaction::isCommited() const
{
    return m_committed;
}

void ASTransaction::commit()
{
    m_committed = true;
    m_transactions->append(this);
}

void ASTransaction::writeToXml(QDomDocument * doc, QDomElement * de)
{
    de->setAttribute("type", TransactionTypeStrings[getType()]);
    de->setAttribute("uuid", getUuid().toString());
    de->setAttribute("timestamp", getCreationTimestamp().toString(Qt::ISODate));
    if (getOverride())
    {
        de->setAttribute("overrides", getOverride()->getUuid().toString());
    }
    if (getHidden())
    {
        de->setAttribute("hidden", "true");
    }
    if (!getDescription().isEmpty())
    {
        QDomElement desc = doc->createElement("description");
        QDomText t = doc->createTextNode(getDescription());
        desc.appendChild(t);
        de->appendChild(desc);
    }
}

void ASTransaction::readFromXml(QDomElement * de)
{
    m_uuid = de->attribute("uuid");
    m_creationTimestamp = parseDateTime(de->attribute("timestamp"));
    if (de->hasAttribute("overrides"))
    {
        QUuid overrides = de->attribute("overrides");
        m_overrides = m_transactions->getByUuid(overrides);
        if (m_overrides)
        {
            m_overrides->m_overwrittenBy = this;
        }
    }
    if (de->hasAttribute("hidden"))
    {
        m_hidden = true;
    }

    QDomNode n = de->firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        handleDomElement(&e);
        n = n.nextSibling();
    }
}

void ASTransaction::handleDomElement(QDomElement * de)
{
    if (de->tagName() == "description")
    {
        setDescription(de->text());
    }
}

bool ASTransaction::operator>(const ASTransaction &other) const
{
    return m_creationTimestamp > other.m_creationTimestamp;
}

bool ASTransaction::operator<(const ASTransaction &other) const
{
    return m_creationTimestamp < other.m_creationTimestamp;
}

QDate ASTransaction::parseDate(const QString& dateString)
{
    QString copy(dateString);
    QDate date = QDate::fromString(copy, Qt::ISODate);
    if (!date.isValid()) {
        QString replaced = copy
                .replace("Mär", "Mar")
                .replace("Mai", "May")
                .replace("Okt", "Oct")
                .replace("Dez", "Dec");
        date = QDate::fromString(replaced);
    }
    if (!date.isValid()) {
        qCritical("Could not parse date in input: %s", qUtf8Printable(dateString));
    }
    return date;
}

QDateTime ASTransaction::parseDateTime(const QString& dateString)
{
    if (dateString.isEmpty()) {
        return QDateTime();
    }

    QString copy(dateString);
    QDateTime timestamp = QDateTime::fromString(copy, Qt::ISODate);
    if (!timestamp.isValid()) {
        QString replaced = copy
                .replace("Mär", "Mar")
                .replace("Mai", "May")
                .replace("Okt", "Oct")
                .replace("Dez", "Dec");
        timestamp = QDateTime::fromString(replaced);
    }
    if (!timestamp.isValid()) {
        qCritical("Could not parse date in input: %s", qUtf8Printable(dateString));
    }
    return timestamp;
}
