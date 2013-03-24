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

#include "entrywindow.h"
#include "entry.h"
#include "invest.h"
#include "docmodel.h"

ASEntryWindow::ASEntryWindow(ASTransactionList * transactions,
                             Ui::DocumentsWindow * docUi,
                             QDialog * docDialog,
                             QWidget * parent) :
    QDialog(parent),
    m_transactions(transactions),
    m_docUi(docUi),
    m_docDialog(docDialog),
    m_selectedDocument(NULL),
    m_override(NULL)
{
    ui.setupUi(this);
    ui.transactionDate->setDate(QDate::currentDate());
    ui.documentDate->setDate(QDate::currentDate());

    QDoubleValidator * val =
        new QDoubleValidator(-1000000.0, 1000000.0, 2, this);
    ui.amount->setValidator(val);
    ui.vatAmount->setValidator(val);

    // VAT Entries

    ui.vatPercentage->addItem("20 %", 20);
    ui.vatPercentage->addItem("10 %", 10);
    ui.vatPercentage->addItem("Anderer", -1);

    connect(ui.chooseDocument, SIGNAL(toggled(bool)),
            this, SLOT(chooseDocument(bool)));
}

void ASEntryWindow::populateLists()
{
    ui.account->clear();
    ui.category->clear();

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASAccount * a = dynamic_cast<ASAccount*>(*it);
        if (a)
        {
            ui.account->addItem(a->getDescription(),
                                QVariant::fromValue((void*)a));
        }
        ASCategory * c = dynamic_cast<ASCategory*>(*it);
        if (c)
        {
            ui.category->addItem(c->getDescription(),
                                 QVariant::fromValue((void*)c));
        }
    }
}

void ASEntryWindow::on_expense_toggled(bool checked)
{
    if (checked)
    {
        double val = ui.amount->text().toDouble();
        if (val > 0.0)
        {
            ui.amount->setText(QString::number(-val));
        }
    }
}

void ASEntryWindow::on_income_toggled(bool checked)
{
if (checked)
    {
        double val = ui.amount->text().toDouble();
        if (val < 0.0)
        {
            ui.amount->setText(QString::number(-val));
        }
    }
}

void ASEntryWindow::on_invest_toggled(bool checked)
{
    ui.category->setEnabled(!checked);
    ui.investGroup->setEnabled(checked);
}

void ASEntryWindow::on_amount_textChanged()
{
    if (!ui.invest->isChecked())
    {
        double val = ui.amount->text().toDouble();
        if (val < 0.0)
        {
            ui.expense->setChecked(true);
        }
        else if (val > 0.0)
        {
            ui.income->setChecked(true);
        }
    }
        
    calculateVat();
}

void ASEntryWindow::on_vatPercentage_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    calculateVat();
}

void ASEntryWindow::on_buttonBox_accepted()
{
    ASAccountEntry * e;
    ASInvestEntry * ie = NULL;
    if (ui.invest->isChecked())
    {
        ie = new ASInvestEntry(m_transactions, m_override);
        ie->setDepreciationPeriod(ui.depreciationPeriod->value());
        e = ie;
    }
    else
    {
        e = new ASAccountEntry(m_transactions, m_override);
        e->setCategory((ASCategory*)ui.category->itemData(
                           ui.category->currentIndex()).value<void *>());
    }

    e->setDescription(ui.entryDescription->text());
    e->setAmount(ui.amount->text().toDouble());
    e->setVatAmount(ui.vatAmount->text().toDouble());
    e->setChargePercentage(ui.chargePercentage->value());
    e->setVatPercentage(
        ui.vatPercentage->itemData(
            ui.vatPercentage->currentIndex()).value<int>());
    e->setAccount((ASAccount*)ui.account->itemData(
                      ui.account->currentIndex()).value<void *>());
    e->setDate(ui.transactionDate->date());

    if (m_selectedDocument)
    {
        e->setDocument(m_selectedDocument);
    }
    else if (!ui.documentDescription->text().isEmpty())
    {
        ASDocument * d = new ASDocument(m_transactions);

        d->setDescription(ui.documentDescription->text());
        d->setDocumentDate(ui.documentDate->date());
        d->setNumber(ui.documentNumber->text());
        d->setId(ui.documentId->text());

        d->commit();

        e->setDocument(d);
    }

    e->commit();

    documentReset();
    ui.entryDescription->setText("");
    ui.amount->setText("");
    ui.vatAmount->setText("");
    ui.vatAmount->setModified(false);
    ui.entryDescription->setFocus();
}

void ASEntryWindow::setOverride(ASTransaction * override)
{
    if (!override)
        return;

    ASAccountEntry * ae = dynamic_cast<ASAccountEntry*>(override);
    if (!ae)
        return;

    for (int i=0; i<ui.category->count(); ++i)
    {
        if (ae->getCategory() &&
            ui.category->itemText(i) == ae->getCategory()->getDescription())
        {
            ui.category->setCurrentIndex(i);
            break;
        }
    }
    ui.entryDescription->setText(ae->getDescription());
    ui.transactionDate->setDate(ae->getDate());
    ui.amount->setText(QString::number(ae->getAmount()));
    ui.vatAmount->setText(QString::number(ae->getVatAmount()));
    for (int i=0; i<ui.vatPercentage->count(); ++i)
    {
        if(ae->getVatPercentage() ==
           ui.vatPercentage->itemData(i).value<int>())
        {
            ui.vatPercentage->setCurrentIndex(i);
        }
    }
    ui.chargePercentage->setValue(ae->getChargePercentage());

    ASInvestEntry * ie = dynamic_cast<ASInvestEntry*>(ae);
    if (ie)
    {
        ui.invest->click();
        ui.depreciationPeriod->setValue(ie->getDepreciationPeriod());
    }

    m_selectedDocument = (ASDocument*)ae->getDocument();

    documentSetup();

    m_override = override;
}

void ASEntryWindow::chooseDocument(bool buttonOn)
{
    if (buttonOn)
    {
        connect(m_docDialog, SIGNAL(accepted()),
                this, SLOT(documentSelected()));
        connect(m_docDialog, SIGNAL(rejected()),
                this, SLOT(documentReset()));
        m_docDialog->exec();
        disconnect(m_docDialog, SIGNAL(accepted()),
                   this, SLOT(documentSelected()));
        disconnect(m_docDialog, SIGNAL(rejected()),
                   this, SLOT(documentReset()));
    }
    else
    {
        documentReset();
    }
}

void ASEntryWindow::documentSelected()
{
    QItemSelectionModel * sm = m_docUi->tableView->selectionModel();

    if (sm && sm->hasSelection())
    {
        QModelIndex mi = sm->currentIndex();
        if (mi.isValid())
        {
            DocModel * model = dynamic_cast<DocModel*>(m_docUi->tableView->model());
            if (model)
            {
                m_selectedDocument = model->getTransactionByRow(mi.row());
                if (m_selectedDocument)
                {
                    documentSetup();
                    return;
                }
            }
        }
    }

    documentReset();
}

void ASEntryWindow::documentSetup()
{
    if (m_selectedDocument)
    {
        ui.documentDescription->setText(m_selectedDocument->getDescription());
        ui.documentDate->setDate(m_selectedDocument->getDocumentDate());
        ui.documentNumber->setText(m_selectedDocument->getNumber());
        ui.documentId->setText(m_selectedDocument->getId());
        ui.documentDescription->setEnabled(false);
        ui.documentDate->setEnabled(false);
        ui.documentNumber->setEnabled(false);
        ui.documentId->setEnabled(false);
    }
}

void ASEntryWindow::documentReset()
{
    m_selectedDocument = NULL;
    m_override = NULL;

    ui.documentDescription->setEnabled(true);
    ui.documentDate->setEnabled(true);
    ui.documentNumber->setEnabled(true);
    ui.documentId->setEnabled(true);

    ui.chooseDocument->setChecked(false);
}

void ASEntryWindow::calculateVat()
{
    if (!ui.amount->text().isEmpty() && !ui.vatAmount->isModified())
    {
        double vp = ui.vatPercentage->itemData(
            ui.vatPercentage->currentIndex()).value<int>();
        if (vp >= 0)
        {
            double vat = qRound(vp * ui.amount->text().toDouble()) / 100.0;
            ui.vatAmount->setText(QString::number(vat));
        }
    }
}
