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

#include <QMessageBox>
#include <QFileDialog>
#include <QDomDocument>
#include "mainwindow.h"
#include "entry.h"
#include "invest.h"

ASMainWindow::ASMainWindow(ASTransactionList * transactions,
                           QWidget * parent) :
    QMainWindow(parent),
    entryWindow(NULL),
    accountDialog(NULL),
    editDocDialog(NULL),
    categoryDialog(NULL),
    vatCategoryDialog(NULL),
    settingsDialog(NULL),
    m_transactions(transactions),
    m_entryModel(NULL),
    m_docModel(NULL),
    m_assetsModel(NULL),
    m_calc(NULL),
    m_docOverride(NULL),
    m_populate(true)
{
    ui.setupUi(this);

    docDialog = new QDialog(this);
    docUi.setupUi(docDialog);
    docUi.tableView->setModel(m_docModel);

    m_fileFilter = tr(
        "Aussackler XML Dateien (*.ass);;"
        "XML Dateien (*.xml);;"
        "Alle Dateien (*)"
        );

    on_actionNew_activated();
}

void ASMainWindow::connectModels()
{
    m_entryModel->transactionsChanged();
    m_assetsModel->transactionsChanged();
    m_docModel->transactionsChanged();

    connect(m_transactions, SIGNAL(newTransaction()),
            m_entryModel, SLOT(transactionsChanged()));
    connect(m_transactions, SIGNAL(newTransaction()),
            m_assetsModel, SLOT(transactionsChanged()));
    connect(m_transactions, SIGNAL(newTransaction()),
            m_docModel, SLOT(transactionsChanged()));
}

void ASMainWindow::disconnectModels()
{
    if (m_entryModel)
    {
        disconnect(m_transactions, SIGNAL(newTransaction()),
                   m_entryModel, SLOT(transactionsChanged()));
    }
    if (m_assetsModel)
    {
        disconnect(m_transactions, SIGNAL(newTransaction()),
                   m_assetsModel, SLOT(transactionsChanged()));
    }
    if (m_docModel)
    {
        disconnect(m_transactions, SIGNAL(newTransaction()),
                   m_docModel, SLOT(transactionsChanged()));
    }
}

void ASMainWindow::on_actionNew_activated()
{
    disconnectModels();

    for (int i = m_transactions->size() - 1; i >= 0; --i)
    {
        delete m_transactions->at(i);
    }
    m_transactions->clear();

    // Model setup

    ui.mainTable->setModel(NULL);
    ui.investTable->setModel(NULL);

    delete m_entryModel;
    delete m_docModel;
    delete m_assetsModel;
    //delete m_earningsModel;
    //delete m_investModel;

    m_entryModel = new EntryModel(m_transactions);
    m_docModel = new DocModel(m_transactions);
    m_assetsModel = new AssetsModel(m_transactions);
    m_calc = new ASCalc(m_transactions);
    m_vat = new ASVat(m_transactions);

    ui.mainTable->setModel(m_entryModel);
    ui.investTable->setModel(m_assetsModel);
    docUi.tableView->setModel(m_docModel);

    connectModels();

    m_populate = true;
}

void ASMainWindow::createEntryWindow()
{
    if (!entryWindow)
    {
        entryWindow = new ASEntryWindow(m_transactions, &docUi,
                                        docDialog, this);
    }
    if (m_populate)
    {
        entryWindow->populateLists();
        m_populate = false;
    }
}

void ASMainWindow::on_actionAddEntries_activated()
{
    createEntryWindow();

    while(entryWindow->exec())
    {
        ui.mainTable->scrollToBottom();
    }
}

void ASMainWindow::on_actionNewEntry_activated()
{
    createEntryWindow();

    entryWindow->exec();
}

void ASMainWindow::on_actionCorrectEntry_activated()
{
    createEntryWindow();

    QItemSelectionModel * sm = ui.mainTable->selectionModel();

    if (sm && sm->hasSelection())
    {
        QModelIndex mi = sm->currentIndex();
        if (mi.isValid())
        {
            ASAccountEntry * ae = m_entryModel->getTransactionByRow(mi.row());
            if (ae)
            {
                entryWindow->setOverride(ae);
                entryWindow->exec();
            }
            else
            {
                QMessageBox::critical(this,
                                      tr("Fehler"),
                                      tr("Bitte wählen Sie die Buchun, "
                                         "die ersetzt werden soll."));
            }
        }
    }
}

void ASMainWindow::on_actionDeleteEntry_activated()
{
    QItemSelectionModel * sm = ui.mainTable->selectionModel();

    if (sm && sm->hasSelection())
    {
        QModelIndex mi = sm->currentIndex();
        if (mi.isValid())
        {
            ASAccountEntry * ae = m_entryModel->getTransactionByRow(mi.row());
            if (ae)
            {
                ASAccountEntry * e = new ASAccountEntry(m_transactions, ae);
                e->setDescription(ae->getDescription());
                e->setAmount(0);
                e->setVatAmount(0);
                e->setChargePercentage(ae->getChargePercentage());
                e->setAccount(ae->getAccount());
                e->setDate(ae->getDate());
                e->setDocument(ae->getDocument());
                e->setHidden();
                e->commit();
                ui.mainTable->clearSelection();
            }
            else
            {
                QMessageBox::critical(this,
                                      tr("Fehler"),
                                      tr("Bitte wählen Sie die Buchun, "
                                         "die gelöscht werden soll."));
            }
        }
    }
}

void ASMainWindow::on_actionNewDocument_activated()
{
    m_docOverride = NULL;

    if (!editDocDialog)
    {
        editDocDialog = new QDialog(this);
        editDocUi.setupUi(editDocDialog);
        connect(editDocDialog, SIGNAL(accepted()),
                this, SLOT(createDocument()));
    }
    editDocUi.description->setFocus();
    editDocDialog->show();
}

void ASMainWindow::on_actionCorrectDocument_activated()
{
    if (!docDialog->exec())
        return;

    QItemSelectionModel * sm = docUi.tableView->selectionModel();

    if (sm && sm->hasSelection())
    {
        QModelIndex mi = sm->currentIndex();
        if (mi.isValid())
        {
            m_docOverride = m_docModel->getTransactionByRow(mi.row());
            if (m_docOverride)
            {
                if (!editDocDialog)
                {
                    editDocDialog = new QDialog(this);
                    editDocUi.setupUi(editDocDialog);
                    connect(editDocDialog, SIGNAL(accepted()),
                            this, SLOT(createDocument()));
                }
                editDocUi.description->setText(m_docOverride->getDescription());
                editDocUi.date->setDate(m_docOverride->getDocumentDate());
                editDocUi.documentNumber->setText(m_docOverride->getNumber());
                editDocUi.documentId->setText(m_docOverride->getId());
                editDocUi.recurring->setChecked(m_docOverride->getRecurring());
                editDocUi.description->setFocus();
                editDocDialog->show();
            }
        }
    }
}

void ASMainWindow::on_actionNewAccount_activated()
{
    if (!accountDialog)
    {
        accountDialog = new QDialog(this);
        accountUi.setupUi(accountDialog);
        connect(accountDialog, SIGNAL(accepted()),
                this, SLOT(createAccount()));
    }
    accountUi.replaces->clear();
    accountUi.replaces->addItem("-", 0);
    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASAccount * a = dynamic_cast<ASAccount*>(*it);
        if (a)
        {
            accountUi.replaces->addItem(a->getDescription(),
                                        QVariant::fromValue((void*)a));
        }
    }
    accountUi.description->setFocus();
    accountDialog->show();
}

void ASMainWindow::on_actionNewCategory_activated()
{
    if (!categoryDialog)
    {
        categoryDialog = new QDialog(this);
        categoryUi.setupUi(categoryDialog);
        categoryUi.replaces->addItem("-", 0);
        connect(categoryDialog, SIGNAL(accepted()),
                this, SLOT(createCategory()));
    }
    categoryUi.replaces->clear();
    categoryUi.replaces->addItem("-", 0);
    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASCategory * c = dynamic_cast<ASCategory*>(*it);
        if (c)
        {
            categoryUi.replaces->addItem(c->getDescription(),
                                         QVariant::fromValue((void*)c));
        }
    }
    categoryUi.description->setFocus();
    categoryDialog->show();
}

void ASMainWindow::on_actionNewVatCategory_activated()
{
    if (!vatCategoryDialog)
    {
        vatCategoryDialog = new QDialog(this);
        vatCategoryUi.setupUi(vatCategoryDialog);
        vatCategoryUi.replaces->addItem("-", 0);
        connect(vatCategoryDialog, SIGNAL(accepted()),
                this, SLOT(createVatCategory()));
    }
    vatCategoryUi.parent->clear();
    vatCategoryUi.parent->addItem("-", 0);
    vatCategoryUi.replaces->clear();
    vatCategoryUi.replaces->addItem("-", 0);
    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASVatCategory * c = dynamic_cast<ASVatCategory*>(*it);
        if (c)
        {
            vatCategoryUi.parent->addItem(c->getDescription(),
                                          QVariant::fromValue((void*)c));
            vatCategoryUi.replaces->addItem(c->getDescription(),
                                            QVariant::fromValue((void*)c));
        }
    }
    vatCategoryUi.description->setFocus();
    vatCategoryDialog->show();
}

void ASMainWindow::on_actionSettings_activated()
{
    if (!settingsDialog)
    {
        settingsDialog = new QDialog(this);
        settingsUi.setupUi(settingsDialog);
        settingsUi.categories->clear();
        ASTransactionList::const_iterator it = m_transactions->constBegin();
        for(; it != m_transactions->constEnd(); ++it)
        {
            ASCategory * c = dynamic_cast<ASCategory*>(*it);
            if (c)
            {
                settingsUi.categories->addItem(c->getDescription(),
                                               QVariant::fromValue((void*)c));
                settingsUi.categories2->addItem(c->getDescription(),
                                                QVariant::fromValue((void*)c));
            }
        }
    }
    settingsDialog->show();
}

void ASMainWindow::on_actionSave_activated()
{
    if (m_currentFileName.isEmpty())
    {
        on_actionSaveas_activated();
        return;
    }

    emit signalSave(m_currentFileName);
}

void ASMainWindow::on_actionSaveas_activated()
{
    m_currentFileName= QFileDialog::getSaveFileName(this,
                                                    tr("Speichern unter"),
                                                    QString(), m_fileFilter);

    if (!m_currentFileName.isEmpty())
    {
        on_actionSave_activated();
    }
}

void ASMainWindow::on_actionLoad_activated()
{
    m_currentFileName= QFileDialog::getOpenFileName(this,
                                                    tr("Datei laden"),
                                                    QString(), m_fileFilter);

    if (m_currentFileName.isEmpty())
        return;

    on_actionNew_activated();
    disconnectModels();

    emit signalLoad(m_currentFileName);

    connectModels();

    m_populate = true;
}

void ASMainWindow::on_actionQuit_activated()
{
    close();
}

void ASMainWindow::on_tabWidget_currentChanged(int index)
{
    QDate fromDate;
    QDate toDate;
    ASCategory * depreciationCategory;
    if (settingsDialog)
    {
        depreciationCategory =
        (ASCategory*)settingsUi.categories->itemData(
            settingsUi.categories->currentIndex()).value<void *>();
        fromDate = settingsUi.from->date();
        toDate = settingsUi.to->date();
    }
    else
    {
        depreciationCategory = NULL;
        fromDate = QDate::currentDate();
        toDate = QDate::currentDate();
    }

    switch(index)
    {
    case 1:
        ui.summary->setText(m_calc->getCalculation(fromDate, toDate,
                                                   depreciationCategory));
        break;
    case 2:
        if (m_assetsModel)
        {
            m_assetsModel->transactionsChanged();
            m_assetsModel->setPeriod(fromDate, toDate);
        }
        break;
    case 3:
        ui.vat->setText(m_vat->getCalculation(fromDate, toDate));
        break;
    default:
        break;
    }
}

void ASMainWindow::createAccount()
{
    QString desc = accountUi.description->text();

    if (desc.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Fehler"),
                              tr("Beschreibung erforderlich!"));
        return;
    }

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASAccount * a = dynamic_cast<ASAccount*>(*it);
        if (a && a->getDescription() == desc)
        {
            QMessageBox::critical(this,
                                  tr("Fehler"),
                                  tr("Beschreibung bereits vorhanden!"));
            return;
        }
    }

    ASAccount * a = new ASAccount(m_transactions,
                                  (ASAccount*)accountUi.replaces->itemData(
                                      accountUi.replaces->currentIndex()).
                                  value<void *>());
    a->setDescription(desc);
    a->commit();

    m_populate = true;
}

void ASMainWindow::createDocument()
{
    QString desc = editDocUi.description->text();

    if (desc.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Fehler"),
                              tr("Beschreibung erforderlich!"));
        return;
    }

    ASDocument * d = new ASDocument(m_transactions, m_docOverride);
    d->setDescription(desc);
    d->setDocumentDate(editDocUi.date->date());
    d->setNumber(editDocUi.documentNumber->text());
    d->setId(editDocUi.documentId->text());
    d->commit();

    m_docOverride = NULL;
}

void ASMainWindow::createCategory()
{
    QString desc = categoryUi.description->text();

    if (desc.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Fehler"),
                              tr("Beschreibung erforderlich!"));
        return;
    }

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASCategory * c = dynamic_cast<ASCategory*>(*it);
        if (c && c->getDescription() == desc)
        {
            QMessageBox::critical(this,
                                  tr("Fehler"),
                                  tr("Beschreibung bereits vorhanden!"));
            return;
        }
    }

    ASCategory * c = new ASCategory(m_transactions,
                                    (ASCategory*)categoryUi.replaces->itemData(
                                        categoryUi.replaces->currentIndex()).
                                    value<void *>());
    c->setDescription(desc);
    c->commit();

    m_populate = true;
}

void ASMainWindow::createVatCategory()
{
    QString desc = vatCategoryUi.description->text();

    if (desc.isEmpty())
    {
        QMessageBox::critical(this,
                              tr("Fehler"),
                              tr("Beschreibung erforderlich!"));
        return;
    }

    ASTransactionList::const_iterator it = m_transactions->constBegin();
    for (; it != m_transactions->constEnd(); ++it)
    {
        ASVatCategory * c = dynamic_cast<ASVatCategory*>(*it);
        if (c && c->getDescription() == desc)
        {
            QMessageBox::critical(this,
                                  tr("Fehler"),
                                  tr("Beschreibung bereits vorhanden!"));
            return;
        }
    }

    ASVatCategory * c = new ASVatCategory(m_transactions,
                                          (ASVatCategory*)vatCategoryUi.replaces->itemData(
                                              vatCategoryUi.replaces->currentIndex()).
                                          value<void *>());

    c->setDescription(desc);
    c->setVatPercentage(vatCategoryUi.vatPercentage->value());
    c->setParent((ASVatCategory*)vatCategoryUi.parent->itemData(
                     vatCategoryUi.parent->currentIndex()).
                 value<void *>());
    c->commit();

    m_populate = true;
}
