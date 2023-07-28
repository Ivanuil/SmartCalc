#include "banks.h"

Loan calculateDifferentiatedLoan(double loanAmount, int term,
                                 double interestRate) {
  Loan loan = {0};
  loan.loanAmount = loanAmount;
  loan.term = term;
  loan.interestRate = interestRate;
  loan.monthlyPayments = malloc(sizeof(double) * term);
  loan.totalPayment = 0;

  interestRate /= 12;
  double principalPayment = loanAmount / term;
  for (int month = 0; month < term; month++) {
    double payment = principalPayment + loanAmount * interestRate;
    loanAmount -= principalPayment;

    loan.monthlyPayments[month] = payment;
    loan.totalPayment += payment;
  }

  loan.overpayment = loan.totalPayment - loan.loanAmount;

  return loan;
}

Loan calculateAnnuityLoan(double loanAmount, int term, double interestRate) {
  Loan loan = {0};
  loan.loanAmount = loanAmount;
  loan.term = term;
  loan.interestRate = interestRate;
  loan.monthlyPayments = malloc(sizeof(double) * term);

  interestRate /= 12;
  double power = pow(1 + interestRate, (double)term);
  double payment = loanAmount * (interestRate * power) / (power - 1);
  for (int month = 0; month < term; month++)
    loan.monthlyPayments[month] = payment;
  loan.totalPayment = payment * term;
  loan.overpayment = loan.totalPayment - loanAmount;

  return loan;
}

Deposit calculateDeposit(double depositAmount, int term, double interestRate,
                         double taxRate, int paymentsPeriodicity,
                         bool capitalization, Operation *replenishment,
                         int replenishmentNumber, Operation *withdrawal,
                         int withdrawalNumber) {
  Deposit deposit = {0};
  deposit.accruedPercents = 0;
  deposit.assessedTaxes = 0;
  deposit.total = depositAmount;

  interestRate /= 365;

  for (int date = 1; date <= term; date++) {
    if (date % paymentsPeriodicity == 0) {
      double payment = depositAmount * interestRate * paymentsPeriodicity;
      deposit.accruedPercents += payment;
      double taxes = payment * taxRate;
      deposit.assessedTaxes += taxes;
      deposit.total += payment - taxes;
      if (capitalization) depositAmount += payment - taxes;
    }
    for (int i = 0; i < replenishmentNumber; i++) {
      if (replenishment[i].date == date) {
        deposit.total += replenishment[i].amount;
        depositAmount += replenishment[i].amount;
      }
    }
    for (int i = 0; i < withdrawalNumber; i++) {
      if (withdrawal[i].date == date) {
        deposit.total -= withdrawal[i].amount;
        depositAmount -= withdrawal[i].amount;
      }
    }
  }

  return deposit;
}
