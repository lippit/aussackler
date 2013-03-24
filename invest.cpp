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

#include "invest.h"

ASInvestEntry::ASInvestEntry(ASTransactionList * transactions,
                             ASTransaction * overrides) :
    ASAccountEntry(transactions, overrides),
    m_years(0)
{
}

bool ASInvestEntry::setDepreciationPeriod(int years)
{
    CHECK_COMMITED;

    m_years = years;
    return true;
}

int ASInvestEntry::getDepreciationPeriod() const
{
    return m_years;
}

double ASInvestEntry::getDepreciation(int year) const
{
    // Calculate linear depreciation for year according to
    // Austrian law (as of 2013).

    if (m_date.isNull())
        return 0.0;

    int hy = m_date.month() <= 6 ? 0 : 1;
    double am = m_amount;
    double dphy = m_amount / (m_years * 2);
    double d = 0.0;
    while(am > 0)
    {
        if (year == m_date.year() + hy / 2)
        {
            d += dphy;
        }
        am -= dphy;
        hy++;
    }

    return d;
}

void ASInvestEntry::writeToXml(QDomDocument * doc, QDomElement * de)
{
    ASAccountEntry::writeToXml(doc, de);
    QDomElement e = doc->createElement("depreciationperiod");
    QDomText t = doc->createTextNode(QString::number(getDepreciationPeriod()));
    e.appendChild(t);
    de->appendChild(e);
}

void ASInvestEntry::handleDomElement(QDomElement * de)
{
    if (de->tagName() == "depreciationperiod")
    {
        setDepreciationPeriod(de->text().toInt());
    }
    else
    {
        ASAccountEntry::handleDomElement(de);
    }
}
