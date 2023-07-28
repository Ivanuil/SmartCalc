#include "mainwindow.h"

#include "./ui_mainwindow.h"
#include "iostream"
#include "stdio.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  calculate_button = ui->calculateButton;
  dif_loan_button = ui->loanDifButton;
  ann_loan_button = ui->loanAnnButton;
  deposit_button = ui->depositButton;
  connect(calculate_button, &QPushButton::pressed, this,
          &MainWindow::handleCalculateButton);
  connect(dif_loan_button, &QPushButton::pressed, this,
          &MainWindow::handleDifLoanButton);
  connect(ann_loan_button, &QPushButton::pressed, this,
          &MainWindow::handleAnnLoanButton);
  connect(deposit_button, &QPushButton::pressed, this,
          &MainWindow::handleDepositButton);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::handleCalculateButton() {
  try {
    QString string = ui->expressionTextEdit->toPlainText();
    QString Xstring = ui->xTextEdit->displayText();
    if (string.size() >= 256 || Xstring.size() >= 256) {
      ui->resultTextEdit->setPlainText(QString("Input too big"));
      return;
    }

    char expression[1024];
    for (int i = 0; i < 1024; i++) expression[i] = '\0';
    std::string stdString = string.toStdString();
    for (int i = 0; i < stdString.size(); i++) {
      expression[i] = stdString.at(i);
    }
    expression[stdString.size()] = '\0';

    bool error = false;
    double XValue = Xstring.toDouble(&error);
    List infix = parseString(expression, &error);
    List polish = toPolish(infix, &error);
    if (error == false) {
      char *polishString = getAsString(polish, ' ');
      ui->polishNotationTextEdit->setPlainText(QString(polishString));
      free(polishString);
    }
    double result = calculateForX(polish, XValue, &error);

    if (error == true)
      ui->resultTextEdit->setPlainText(QString("ERROR"));
    else {
      QString resQString = QString::number(result);

      ui->resultTextEdit->setPlainText(resQString);
    }

    bool er;
    double Xmin = ui->Xmin->displayText().toDouble(&er);
    double Xmax = ui->Xmax->displayText().toDouble(&er);
    double num = 1000000;
    QVector<double> x(num), y(num);
    for (int i = 0; i < num; i++) {
      x[i] = Xmin + (Xmax - Xmin) / num * i;
      y[i] = calculateForX(polish, x[i], &error);
    }

    QCustomPlot *plot = ui->Plot;
    plot->addGraph();
    plot->graph(0)->setData(x, y);
    plot->xAxis->setRange(Xmin, Xmax);
    if (ui->Yauto->isChecked()) {
      double Ymin = y[0];
      double Ymax = y[0];
      for (auto i : y) {
        if (i < Ymin) Ymin = i;
        if (i > Ymax) Ymax = i;
      }
      plot->yAxis->setRange(Ymin, Ymax);
      ui->Ymin->setText(QString::number(Ymin));
      ui->Ymax->setText(QString::number(Ymax));
    } else {
      plot->yAxis->setRange(ui->Ymin->displayText().toDouble(&er),
                            ui->Ymax->displayText().toDouble(&er));
    }
    plot->replot();

    listFree(&infix);
    listFree(&polish);
  } catch (std::exception &e) {
    ui->resultTextEdit->setPlainText(QString("ERROR"));
  }
}

void MainWindow::handleDifLoanButton() {
  bool error = false;
  double loanAmmount = ui->loanAmmountTextEdit->displayText().toDouble(&error);
  int term = ui->termTextEdit->displayText().toInt(&error, 10);
  double interstRate = ui->interestRateTextEdit->displayText().toDouble(&error);

  Loan loan = calculateDifferentiatedLoan(loanAmmount, term, interstRate);
  QString res = "";
  res.append("Loan amount: ");
  res.append(QString::number(loan.loanAmount));
  res.append("\nTerm: ");
  res.append(QString::number(loan.term));
  res.append("\nInterest rate: ");
  res.append(QString::number(loan.interestRate));
  res.append("\nTotal payment: ");
  res.append(QString::number(loan.totalPayment));
  res.append("\nOverpayment: ");
  res.append(QString::number(loan.overpayment));
  res.append("\nPayments: ");
  for (int i = 0; i < term; i++) {
    res.append("\n ");
    res.append(QString::number(i + 1));
    res.append(" : ");
    res.append(QString::number(loan.monthlyPayments[i]));
  }

  ui->loanOutput->setPlainText(res);
}

void MainWindow::handleAnnLoanButton() {
  bool error = false;
  double loanAmmount = ui->loanAmmountTextEdit->displayText().toDouble(&error);
  int term = ui->termTextEdit->displayText().toInt(&error, 10);
  double interstRate = ui->interestRateTextEdit->displayText().toDouble(&error);

  Loan loan = calculateAnnuityLoan(loanAmmount, term, interstRate);
  QString res = "";
  res.append("Loan ammount: ");
  res.append(QString::number(loan.loanAmount));
  res.append("\nTerm: ");
  res.append(QString::number(loan.term));
  res.append("\nInterest rate: ");
  res.append(QString::number(loan.interestRate));
  res.append("\nTotal payment: ");
  res.append(QString::number(loan.totalPayment));
  res.append("\nOverpayment: ");
  res.append(QString::number(loan.overpayment));
  res.append("\nPayments: ");
  for (int i = 0; i < term; i++) {
    res.append("\n ");
    res.append(QString::number(i + 1));
    res.append(" : ");
    res.append(QString::number(loan.monthlyPayments[i]));
  }

  ui->loanOutput->setPlainText(res);
}

void MainWindow::handleDepositButton() {
  bool error = false;
  double depositAmmount =
      ui->depositAmmoutTextEdit->displayText().toDouble(&error);
  int term = ui->depositTermTextEdit->displayText().toInt(&error, 10);
  double interestRate =
      ui->depositInteterstRateTextEdit->displayText().toDouble(&error);
  double taxRate = ui->taxRateTextEdit->displayText().toDouble(&error);
  int paymentsPeriodicity =
      ui->paymentsPeriodicity->displayText().toDouble(&error);
  bool capitalization = ui->CapitalisationCheckBox->isChecked();

  Deposit deposit = calculateDeposit(
      depositAmmount, term, interestRate, taxRate, paymentsPeriodicity,
      capitalization, new Operation[0], 0, new Operation[0], 0);
  QString res = "";
  res.append("\nAcquired percents: ");
  res.append(QString::number(deposit.accruedPercents));
  res.append("\nAssessed taxes: ");
  res.append(QString::number(deposit.assessedTaxes));
  res.append("\nTotal: ");
  res.append(QString::number(deposit.total));

  ui->depositOutout->setPlainText(res);
}
