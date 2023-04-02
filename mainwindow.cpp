//
// Created by dmonnie5 on 22/03/23.
//
#include <QDate>
#include "mainwindow.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QChartView>

using namespace QtCharts;

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
    setMinimumSize(800, 600);

    connect(tabWidget, &QTabWidget::currentChanged, [this](int index) {
        if (index == 1) {
            updatePieChart();
            if (tblStatistics) {
                updateStatisticsTab();
            }
        }
    });
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

    dateEdit = new QDateEdit();
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");

    hbxExpenses->addWidget(lblName);
    hbxExpenses->addWidget(txtName);
    hbxExpenses->addWidget(lblPrice);
    hbxExpenses->addWidget(spnPrice);
    hbxExpenses->addWidget(lblDate);
    hbxExpenses->addWidget(dateEdit);
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
                                                        tr("Nom de la nouvelle catégorie à ajouter:"), QLineEdit::Normal,
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

    // Add controls to select start and end dates
    QHBoxLayout *hbxDateSelection = new QHBoxLayout();
    QLabel *lblDateStart = new QLabel("Date de début:");
    QLabel *lblDateEnd = new QLabel("Date de fin:");
    dateEditStart = new QDateEdit();
    dateEditEnd = new QDateEdit();
    btnApplyDates = new QPushButton("Appliquer");

    dateEditStart->setDate(QDate(2000, 1, 1));
    dateEditStart->setDisplayFormat("dd/MM/yyyy");
    dateEditEnd->setDate(QDate(2100, 1, 1));
    dateEditEnd->setDisplayFormat("dd/MM/yyyy");

    hbxDateSelection->addWidget(lblDateStart);
    hbxDateSelection->addWidget(dateEditStart);
    hbxDateSelection->addWidget(lblDateEnd);
    hbxDateSelection->addWidget(dateEditEnd);
    hbxDateSelection->addWidget(btnApplyDates);
    vbxStatistics->addLayout(hbxDateSelection);

    // Create pie chart
    QPieSeries *series = new QPieSeries();
    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle(tr("Répartition des dépenses par catégorie"));
    chart->legend()->setVisible(true);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    vbxStatistics->addWidget(chartView);

    tblStatistics = new QTableWidget();
    tblStatistics->setColumnCount(2);
    QStringList headers;
    headers << "Categorie" << "Total";
    tblStatistics->setHorizontalHeaderLabels(headers);
    vbxStatistics->addWidget(tblStatistics);

    tabStatistics->setLayout(vbxStatistics);

    // Connect the Apply button to update the statistics
    connect(btnApplyDates, &QPushButton::clicked, [this]() {
            updatePieChart();
            updateStatisticsTab();
        });
}


void MainWindow::updatePieChart()
{
    QChart *chart = chartView->chart();
    QPieSeries *series = qobject_cast<QPieSeries*>(chart->series().at(0));
    series->clear();

    QMap<QString, int> totals;

    QDate startDate = dateEditStart->date();
    QDate endDate = dateEditEnd->date();

    // Calculate the totals for each category according to the selected period
    for (int i = 0; i < tblExpenses->rowCount(); i++) {
        QString category = tblExpenses->item(i, 3)->text();
        int price = tblExpenses->item(i, 1)->text().toInt();
        QDate expenseDate = QDate::fromString(tblExpenses->item(i, 2)->text(), "dd/MM/yyyy");

        if (expenseDate >= startDate && expenseDate <= endDate) {
            totals[category] += price;
        }
    }

    // Add the totals to the pie chart
    for (auto it = totals.begin(); it != totals.end(); ++it) {
        if (it.value() > 0) {
            QPieSlice *slice = new QPieSlice(it.key(), it.value());
            series->append(slice);
        }
    }
}



void MainWindow::updateStatisticsTab() {
    QMap<QString, int> totals;

    QDate startDate = dateEditStart->date();
    QDate endDate = dateEditEnd->date();

    // Calculate the totals for each category according to the selected period
    for (int i = 0; i < tblExpenses->rowCount(); i++) {
        QString category = tblExpenses->item(i, 3)->text();
        int price = tblExpenses->item(i, 1)->text().toInt();
        QDate expenseDate = QDate::fromString(tblExpenses->item(i, 2)->text(), "dd/MM/yyyy");

        if (expenseDate >= startDate && expenseDate <= endDate) {
            totals[category] += price;
        }
    }

    // Clear the statistics table
    tblStatistics->setRowCount(0);

    // Add each category total to the statistics table
    int row = 0;
    for (auto it = totals.begin(); it != totals.end(); ++it) {
        if (it.value() > 0) {
            this->tblStatistics->insertRow(row);
            this->tblStatistics->setItem(row, 0, new QTableWidgetItem(it.key()));
            this->tblStatistics->setItem(row, 1, new QTableWidgetItem(QString::number(it.value())));
            row++;
        }
    }
}


void MainWindow::onAddExpenseClicked()
{
    // Get the input values
    QString name = txtName->text();
    int price = spnPrice->value();
    QString category = cboCategory->currentText();
    QDate date = dateEdit->date();

    // Create a new row and add it to the table
    int row = tblExpenses->rowCount();
    tblExpenses->insertRow(row);
    tblExpenses->setItem(row, 0, new QTableWidgetItem(name));
    tblExpenses->setItem(row, 1, new QTableWidgetItem(QString::number(price)));
    tblExpenses->setItem(row, 2, new QTableWidgetItem(date.toString("dd/MM/yyyy")));
    tblExpenses->setItem(row, 3, new QTableWidgetItem(category));

    updateStatisticsTab();
    updatePieChart();
}
