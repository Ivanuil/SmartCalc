#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Loan {
  double loanAmount;
  int term;
  double interestRate;
  double *monthlyPayments;
  double overpayment;
  double totalPayment;
} Loan;

Loan calculateDifferentiatedLoan(double loanAmount, int term,
                                 double interestRate);

Loan calculateAnnuityLoan(double loanAmount, int term, double interestRate);

typedef struct Deposit {
  double accruedPercents;
  double assessedTaxes;
  double total;
} Deposit;

typedef struct Operation {
  int date;
  double amount;
} Operation;

Deposit calculateDeposit(double depositAmount, int term, double interestRate,
                         double taxRate, int paymentsPeriodicity,
                         bool capitalization, Operation *replenishment,
                         int replenishmentNumber, Operation *withdrawal,
                         int withdrawalNumber);
