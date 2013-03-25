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
    ui.totalAmount->setValidator(val);

    // Completers

    QStringList entryList;
    QStringList documentList;

    ASTransactionList::const_iterator it = transactions->constBegin();
    for (; it != transactions->constEnd(); ++it)
    {
        if (dynamic_cast<ASAccountEntry*>(*it))
        {
            entryList.append((*it)->getDescription());
        }
        else if (dynamic_cast<ASDocument*>(*it))
        {
            documentList.append((*it)->getDescription());
        }
    }

    m_entryCompleter = new QCompleter(entryList, this);
    m_docCompleter = new QCompleter(documentList, this);
    ui.entryDescription->setCompleter(m_entryCompleter);
    ui.documentDescription->setCompleter(m_docCompleter);

    // VAT Entries

    ui.vatPercentage->addItem("20 %", 20);
    ui.vatPercentage->addItem("10 %", 10);
    ui.vatPercentage->addItem("Anderer", -1);
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
        double vat = ui.vatAmount->text().toDouble();
        if (val > 0.0)
        {
            ui.amount->setText(QString::number(-val));
            ui.vatAmount->setText(QString::number(-vat));
            calculateTotal();
        }
    }
}

void ASEntryWindow::on_income_toggled(bool checked)
{
if (checked)
    {
        double val = ui.amount->text().toDouble();
        double vat = ui.vatAmount->text().toDouble();
        if (val < 0.0)
        {
            ui.amount->setText(QString::number(-val));
            ui.vatAmount->setText(QString::number(-vat));
            calculateTotal();
        }
    }
}

void ASEntryWindow::on_invest_toggled(bool checked)
{
    ui.category->setEnabled(!checked);
    ui.investGroup->setEnabled(checked);
}

void ASEntryWindow::on_entryDescription_textChanged()
{
    if (!ui.documentDescription->isModified() &&
        ui.documentDescription->isEnabled())
    {
        ui.documentDescription->setText(ui.entryDescription->text());
    }

    if (m_transactions->size() == 0)
        return;

    int i = 0;
    ASTransactionList::const_iterator it = m_transactions->constEnd();
    do {
        it--;
        ASAccountEntry * e = dynamic_cast<ASAccountEntry*>(*it);
        if (e && e->getDescription() == ui.entryDescription->text())
        {
            fillFields(e);
            ui.transactionDate->setDate(e->getDate().addMonths(1));

            if (e->getDocument() && e->getDocument()->getRecurring())
            {
                m_selectedDocument = e->getDocument();
                documentSetup();
            }
            else
            {
                documentReset();
            }
            return;
        }
    } while(it != m_transactions->constBegin() && ++i < 1000);

    documentReset();
}

void ASEntryWindow::on_amount_textEdited()
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
    setVatPercentage();
    calculateTotal();
}

void ASEntryWindow::on_vatAmount_textEdited()
{
    setVatPercentage();
    calculateTotal();
}

void ASEntryWindow::on_totalAmount_textEdited()
{
    double totalAmount = ui.totalAmount->text().toDouble();

    if (!ui.amount->isModified())
    {
        double vp = ui.vatPercentage->itemData(
            ui.vatPercentage->currentIndex()).value<int>();
        if (vp > 0.0)
        {
            double amount = qRound(100.0 * totalAmount /
                                   (1.0 + (vp / 100.0))) / 100.0;
            double vatAmount = totalAmount - amount;
            ui.amount->setText(QString::number(amount));
            ui.vatAmount->setText(QString::number(vatAmount));
        }
    }
    else
    {
        double amount = ui.amount->text().toDouble();
        double vatAmount = totalAmount - amount;
        ui.vatAmount->setText(QString::number(vatAmount));
    }
    setVatPercentage();
}

void ASEntryWindow::on_vatPercentage_currentIndexChanged(int index)
{
    Q_UNUSED(index);
    calculateVat();
    calculateTotal();
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
        d->setRecurring(ui.documentRecurring->isChecked());

        d->commit();

        e->setDocument(d);
    }

    e->commit();

    QStringListModel * slm = dynamic_cast<QStringListModel*>
        (m_entryCompleter->model());
    if (slm)
    {
        QStringList sl = slm->stringList();
        sl.append(ui.entryDescription->text());
        slm->setStringList(sl);
    }

    documentReset();
    ui.entryDescription->setText("");
    ui.amount->setText("");
    ui.vatAmount->setText("");
    ui.totalAmount->setText("");
    ui.documentRecurring->setChecked(false);
    ui.amount->setModified(false);
    ui.vatAmount->setModified(false);
    ui.totalAmount->setModified(false);
    ui.entryDescription->setFocus();
}

void ASEntryWindow::on_chooseDocument_clicked()
{
    if (ui.chooseDocument->isChecked() && m_docDialog->exec())
    {
        documentSelected();
    }
    else
    {
        documentReset();
    }
}

void ASEntryWindow::setOverride(ASTransaction * override)
{
    if (!override)
        return;

    ASAccountEntry * ae = dynamic_cast<ASAccountEntry*>(override);
    if (!ae)
        return;

    fillFields(ae);

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

void ASEntryWindow::fillFields(ASAccountEntry * ae)
{
    for (int i=0; i<ui.account->count(); ++i)
    {
        if (ae->getAccount() &&
            ui.account->itemText(i) == ae->getAccount()->getDescription())
        {
            ui.account->setCurrentIndex(i);
            break;
        }
    }
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
    setVatPercentage();
    calculateTotal();
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
        ui.documentRecurring->setChecked(m_selectedDocument->getRecurring());
        ui.documentDescription->setEnabled(false);
        ui.documentDate->setEnabled(false);
        ui.documentNumber->setEnabled(false);
        ui.documentId->setEnabled(false);
        ui.documentRecurring->setEnabled(false);
        ui.documentDescription->setModified(false);
        ui.documentNumber->setModified(false);
        ui.documentId->setModified(false);
        ui.chooseDocument->setChecked(true);
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
    ui.documentRecurring->setEnabled(true);

    ui.documentRecurring->setChecked(false);
    ui.chooseDocument->setChecked(false);
}

void ASEntryWindow::calculateVat()
{
    if (!ui.amount->text().isEmpty())
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

void ASEntryWindow::setVatPercentage()
{
    if (ui.amount->text().isEmpty() ||
        ui.vatAmount->text().isEmpty())
        return;

    double amount = ui.amount->text().toDouble();
    double vatAmount = ui.vatAmount->text().toDouble();

    if (amount == 0.0 || vatAmount == 0.0)
        return;

    int vat = qRound(100.0 * vatAmount / amount);

    for (int i=0; i<ui.vatPercentage->count(); ++i)
    {
        if(vat == ui.vatPercentage->itemData(i).value<int>())
        {
            ui.vatPercentage->setCurrentIndex(i);
            return;
        }
    }
    ui.vatPercentage->setCurrentIndex(ui.vatPercentage->count()-1);
}

void ASEntryWindow::calculateTotal()
{
    double amount = ui.amount->text().toDouble();
    double vatAmount = ui.vatAmount->text().toDouble();
    double total = amount + vatAmount;

    ui.totalAmount->setText(QString::number(total));
}
