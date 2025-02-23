#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QFile>
#include <QMutex>

QMutex sleep_mutex;

#include "queue_names.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->lineEdit_Command, &QLineEdit::textChanged, this, &MainWindow::updateCommand);
    connect(ui->lineEdit_Params, &QLineEdit::textChanged, this, &MainWindow::updateParams);
    connect(ui->pushButton_Send, &QPushButton::clicked, this, &MainWindow::send);
 }

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateCommand(const QString & text)
{
    this->command = text;
    //qDebug().noquote() << "command = " << command;
}

void MainWindow::updateParams(const QString & text)
{
    this->params = text;
    //qDebug().noquote() << "params = " << params;
}

void MainWindow::send()
{
    // Create the request
    QString separated_params = this->params.replace(" ", "\n");
    QString request = this->command + "\n" + separated_params +  "\n";
    ui->textEdit_Sent->setText(request);

    // Write the request to the input queue
    QString request_file_name{QUEUE_INPUT_DIRECTORY + "\\" + QUEUE_INPUT_FILE};
    qDebug().noquote() << "Write request file" << request_file_name;
    QFile request_file(request_file_name);
    if(request_file.open(QIODeviceBase::WriteOnly | QIODeviceBase::Text, QFile::ReadOwner|QFile::WriteOwner|QFile::ExeOwner|QFile::ReadGroup|QFile::ExeGroup|QFile::ReadOther|QFile::ExeOther))
    {
        QByteArray request_bytes = request.toUtf8();
        request_file.write(request_bytes);
        request_file.flush();
        request_file.close();
    }

    // Look for the response file
    QString response_file_name{QUEUE_OUTPUT_DIRECTORY + "\\" + QUEUE_OUTPUT_FILE};
    qDebug().noquote() << "Read response file" << response_file_name;
    QFile response_file(response_file_name);
    bool bOpened = false;
    int open_tries = 0;
    while(!bOpened && open_tries < 100)
    {
        sleep_mutex.lock();
        sleep_mutex.tryLock(50);
        sleep_mutex.unlock();

        open_tries++;
        if (response_file.exists())
        {
            bOpened = response_file.open(QIODeviceBase::ReadOnly | QIODeviceBase::Text);
        }
    }
    if(bOpened)
    {
        QByteArray response_bytes = response_file.readAll();
        response_file.close();
        response_file.remove();

        // Show the response
        QString response = QString(response_bytes);
        ui->textEdit_Received->setText(response);
    }
}
