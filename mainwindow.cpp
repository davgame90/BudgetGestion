//
// Created by dmonnie5 on 22/03/23.
//
#include <QDate>
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    tabWidget = new QTabWidget(this);

    setupExpensesTab();
    tabWidget->addTab(tabExpenses, "Dépenses");

    setupStatisticsTab();
    tabWidget->addTab(tabStatistics, "Statistiques");

    setCentralWidget(tabWidget);
    setWindowTitle("Gestion de budget");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupExpensesTab()
{
    // Create the widgets for the Expenses tab
    tabExpenses = new QWidget();
    lblName = new QLabel("Nom:");
    lblPrice = new QLabel("Prix:");
    lblDate = new QLabel("Date:");
    lblCategory = new QLabel("Categorie:");
    txtName = new QLineEdit();
    spnPrice = new QSpinBox();
    cboCategory = new QComboBox();
    tblExpenses = new QTableWidget();
    btnAddExpense = new QPushButton("Ajouter");
    hbxExpenses = new QHBoxLayout();
    vbxExpenses = new QVBoxLayout();

    cboCategory->addItem("Nouveau...");
    cboCategory->addItem("Nourriture");
    cboCategory->addItem("Maison");
    cboCategory->addItem("Transport");
    cboCategory->addItem("Loisir");
    cboCategory->addItem("Autre");

    tblExpenses->setColumnCount(4);
    QStringList headers;
    headers << "Nom" << "Prix" << "Date" << "Categorie";
    tblExpenses->setHorizontalHeaderLabels(headers);

    hbxExpenses->addWidget(lblName);
    hbxExpenses->addWidget(txtName);
    hbxExpenses->addWidget(lblPrice);
    hbxExpenses->addWidget(spnPrice);
    hbxExpenses->addWidget(lblCategory);
    hbxExpenses->addWidget(cboCategory);
    hbxExpenses->addWidget(btnAddExpense);
    vbxExpenses->addLayout(hbxExpenses);
    vbxExpenses->addWidget(tblExpenses);
    tabExpenses->setLayout(vbxExpenses);

    // Connect the add button to a slot that will check if the selected category is "Nouveau..."
// and show a dialog to add a new category
    connect(btnAddExpense, &QPushButton::clicked, [this]() {
        if (cboCategory->currentText() == "Nouveau...") {
            bool ok;
            QString newCategory = QInputDialog::getText(this, tr("Ajouter une catégorie"),
                                                        tr("Nom de la nouvelle catégorie :"), QLineEdit::Normal,
                                                        "", &ok);
            if (ok && !newCategory.isEmpty()) {
                cboCategory->addItem(newCategory);
                cboCategory->setCurrentText(newCategory);
            }
        } else {
            onAddExpenseClicked();
        }
    });

}

void MainWindow::setupStatisticsTab()
{
    tabStatistics = new QWidget();
    vbxStatistics = new QVBoxLayout();
    QLabel *lblStatistics = new QLabel("Statistiques");
    vbxStatistics->addWidget(lblStatistics);
    tabStatistics->setLayout(vbxStatistics);
}
void MainWindow::onAddExpenseClicked()
{
    // Get the input values
    QString name = txtName->text();
    int price = spnPrice->value();
    QString category = cboCategory->currentText();
    QDate date = QDate::currentDate();

    // Create a new row and add it to the table
    int row = tblExpenses->rowCount();
    tblExpenses->insertRow(row);
    tblExpenses->setItem(row, 0, new QTableWidgetItem(name));
    tblExpenses->setItem(row, 1, new QTableWidgetItem(QString::number(price)));
    tblExpenses->setItem(row, 2, new QTableWidgetItem(date.toString("dd/MM/yyyy")));
    tblExpenses->setItem(row, 3, new QTableWidgetItem(category));
}
