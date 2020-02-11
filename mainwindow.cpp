#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <bitset>
#include <alu.h>
using namespace std;

struct campos{
     unsigned int f:23;
     unsigned int e:8;
     unsigned int s:1;
};

union valores{
     float real;
     unsigned int entero;
     struct campos bits ;
};

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    float n1 = this->ui->txtn1->toPlainText().toFloat();
    float n2 = this->ui->txtn2->toPlainText().toFloat();
    union valores valor1;
    union valores valor2;
    valor1.real=n1;
    valor2.real=n2;
    Alu alu(valor1.bits.s,valor1.bits.e,valor1.bits.f,valor2.bits.s,valor2.bits.e,valor2.bits.f);
    if(this->ui->sumaButton->isChecked()){
        alu.suma();
        union valores valorRes;
        valorRes.bits.e=alu.getExponente();
        valorRes.bits.f=alu.getFraccionaria();
        valorRes.bits.s=alu.getSigno();
        this->ui->txtresult->setText(QString::number(valorRes.real));
        this->ui->textEdit_8->setText(QString::fromStdString("0x")+QString::number(valorRes.entero,16));
        this->ui->textEdit_2->setText(QString::fromStdString("0x")+QString::number(valor1.entero,16));
        this->ui->textEdit_4->setText(QString::fromStdString("0x")+QString::number(valor2.entero,16));
    }
    if(this->ui->multiButton->isChecked()){
        alu.multiplica();
        union valores valorRes;
        valorRes.bits.e=alu.getExponente();
        valorRes.bits.f=alu.getFraccionaria();
        valorRes.bits.s=alu.getSigno();
        this->ui->txtresult->setText(QString::number(valorRes.real));
        this->ui->textEdit_8->setText(QString::fromStdString("0x")+QString::number(valorRes.entero,16));
        this->ui->textEdit_2->setText(QString::fromStdString("0x")+QString::number(valor1.entero,16));
        this->ui->textEdit_4->setText(QString::fromStdString("0x")+QString::number(valor2.entero,16));
    }
    if(this->ui->divButton->isChecked()){
        alu.divide();
        union valores valorRes;
        valorRes.bits.e=alu.getExponente();
        valorRes.bits.f=alu.getFraccionaria();
        valorRes.bits.s=alu.getSigno();
        this->ui->txtresult->setText(QString::number(valorRes.real));
        this->ui->textEdit_8->setText(QString::fromStdString("0x")+QString::number(valorRes.entero,16));
        this->ui->textEdit_2->setText(QString::fromStdString("0x")+QString::number(valor1.entero,16));
        this->ui->textEdit_4->setText(QString::fromStdString("0x")+QString::number(valor2.entero,16));
    }
}
