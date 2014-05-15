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
#include <QFont>
#include <QPair>
#include "vat.h"
#include "entry.h"
#include "invest.h"

ASVat::ASVat(ASTransactionList * transactions, QObject * parent) :
    QObject(parent),
    m_transactions(transactions)
{
}

QString ASVat::getCalculation(int year)
{
    QString report;

    QHash<const ASVatCategory *, QPair<double, double> > category;

    int indent = 15;

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        const ASVatCategory * cat = dynamic_cast<const ASVatCategory*>(*it);
        if (cat)
        {
            category.insert(cat, QPair<double, double>(0.0, 0.0));
            if (cat->getDescription().length() > indent)
            {
                indent = cat->getDescription().length();
            }
        }
    }
    indent += 3;

    it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        const ASAccountEntry * ae = dynamic_cast<const ASAccountEntry*>(*it);
        if (!ae)
            continue;

        if (ae->getOverwrittenBy())
            continue;

        if (ae->getDate().year() == year)
        {
            double charge = ae->getAmount() * ae->getChargePercentage() / 100.0;
            double chargeVat = ae->getVatAmount() * ae->getChargePercentage() / 100.0;
            ASVatCategoryList vl = ae->getVatCategories();
            ASVatCategoryList::const_iterator it2 = vl.constBegin();
            for (; it2 != vl.constEnd(); ++it2)
            {
                category[*it2].first += qAbs((double)(qRound(charge * 100)) / 100.0);
                category[*it2].second += qAbs((double)(qRound(chargeVat * 100)) / 100.0);
                const ASVatCategory * vp = (*it2)->getParent();
                while (vp)
                {
                    category[vp].first += qAbs((double)(qRound(charge * 100)) / 100.0);
                    category[vp].second += qAbs((double)(qRound(chargeVat * 100)) / 100.0);
                    vp = vp->getParent();
                }
            }
        }
    }

    QString line = QString("%1 %2 %3\n")
        .arg("Description", -indent)
        .arg("Base amount", 15)
        .arg("VAT amount", 15);
    report.append(line);
    report.append("\n");

    QHashIterator<const ASVatCategory *, QPair<double, double> > hit(category);
    while (hit.hasNext()) {
        hit.next();
        line = QString("%1 %2 %3\n")
            .arg(hit.key()->getDescription(), -indent)
            .arg(hit.value().first, 15, 'f', 2)
            .arg(hit.value().second, 15, 'f', 2);
        report.append(line);
    }

    return report;
}
