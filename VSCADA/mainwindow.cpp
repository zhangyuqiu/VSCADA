#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);


    central = new QWidget();
    mainLayout = new QGridLayout();

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);

    //central->setStyleSheet("background-color: red;");


    editsize=100;
    xinit=0;
    yinit=0;

    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);


    this->setCentralWidget(scrollArea);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    QLabel * blank1= new QLabel();
    blank1->setText("                   ");
    blank1->setStyleSheet("font:50pt;");
    mainLayout->addWidget(blank1,0,0);

   // QLabel * blank11= new QLabel();
    //blank11->setText(" ");
   // blank11->setStyleSheet("font:36pt;");
    //mainLayout->addWidget(blank11,1,0);

    //QLabel * blank12= new QLabel();
    //blank12->setText(" ");
    //blank12->setStyleSheet("font:36pt;");
    //mainLayout->addWidget(blank12,1,1);


    glvLabel = new QLabel();
    glvLabel->setText("GLV");
    glvLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(glvLabel,0,1);

    glvLabel1 = new QLabel();
    glvLabel1->setText("GLV sensor1");
    glvLabel1->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvLabel1,1,0);

    glvEdit1= new QLineEdit();
    glvEdit1->setText("0.0");
    glvEdit1->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    glvEdit1->setMinimumWidth(editsize);
    glvEdit1->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvEdit1,1,1);

    glvLabel2 = new QLabel();
    glvLabel2->setText("GLV sensor2");
    glvLabel2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvLabel2,2,0);

    glvEdit2= new QLineEdit();
    glvEdit2->setText("0.0");
    glvEdit2->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    glvEdit2->setMinimumWidth(editsize);
    glvEdit2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvEdit2,2,1);

    glvLabel3 = new QLabel();
    glvLabel3->setText("GLV sensor3");
    glvLabel3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvLabel3,3,0);

    glvEdit3= new QLineEdit();
    glvEdit3->setText("0.0");
    glvEdit3->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    glvEdit3->setMinimumWidth(editsize);
    glvEdit3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvEdit3,3,1);


    //
    QLabel * blank2= new QLabel();
    blank2->setText("                   ");
    blank2->setStyleSheet("font:50pt;");
    mainLayout->addWidget(blank2,0,2);

    tsiLabel = new QLabel();
    tsiLabel->setText("TSI");
    tsiLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(tsiLabel,0,3);

    tsiLabel1 = new QLabel();
    tsiLabel1->setText("TSI sensor1");
    tsiLabel1->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiLabel1,1,2);

    tsiEdit1= new QLineEdit();
    tsiEdit1->setText("0.0");
    tsiEdit1->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsiEdit1->setMinimumWidth(editsize);
    tsiEdit1->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiEdit1,1,3);

    tsiLabel2 = new QLabel();
    tsiLabel2->setText("TSI sensor2");
    tsiLabel2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiLabel2,2,2);

    tsiEdit2= new QLineEdit();
    tsiEdit2->setText("0.0");
    tsiEdit2->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsiEdit2->setMinimumWidth(editsize);
    tsiEdit2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiEdit2,2,3);

    tsiLabel3 = new QLabel();
    tsiLabel3->setText("TSI sensor3");
    tsiLabel3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiLabel3,3,2);

    tsiEdit3= new QLineEdit();
    tsiEdit3->setText("0.0");
    tsiEdit3->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsiEdit3->setMinimumWidth(editsize);
    tsiEdit3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiEdit3,3,3);


    //tsv
    QLabel * blank3= new QLabel();
    blank3->setText("                   ");
    blank3->setStyleSheet("font:50pt;");
    mainLayout->addWidget(blank3,0,4);

    tsvLabel = new QLabel();
    tsvLabel->setText("TSV");
    tsvLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(tsvLabel,0,5);

    tsvLabel1 = new QLabel();
    tsvLabel1->setText("TSV sensor1");
    tsvLabel1->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvLabel1,1,4);

    tsvEdit11= new QLineEdit();
    tsvEdit11->setText("0.0");
    tsvEdit11->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsvEdit11->setMinimumWidth(editsize);
    tsvEdit11->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvEdit11,1,5);

    tsvLabel2 = new QLabel();
    tsvLabel2->setText("TSV sensor2");
    tsvLabel2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvLabel2,2,4);

    tsvEdit21= new QLineEdit();
    tsvEdit21->setText("0.0");
    tsvEdit21->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsvEdit21->setMinimumWidth(editsize);
    tsvEdit21->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvEdit21,2,5);

    tsvLabel3 = new QLabel();
    tsvLabel3->setText("TSV sensor3");
    tsvLabel3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvLabel3,3,4);

    drawEdit(tsvEdit31,3,5);
    drawEdit(tsvEdit12,1,6);
    drawEdit(tsvEdit22,2,6);
    drawEdit(tsvEdit32,3,6);
    drawEdit(tsvEdit13,1,7);
    drawEdit(tsvEdit23,2,7);
    drawEdit(tsvEdit33,3,7);
    drawEdit(tsvEdit14,1,8);
    drawEdit(tsvEdit24,2,8);
    drawEdit(tsvEdit34,3,8);

    QLabel * blank4= new QLabel();
    blank4->setText("         ");
    blank4->setStyleSheet("font:38pt;");
    mainLayout->addWidget(blank4,4,8);

    plotButton =new QPushButton();
    plotButton->setText("Plot");
    plotButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    plotButton->setMinimumWidth(editsize);
    plotButton->setMinimumHeight(editsize);
    mainLayout->addWidget(plotButton,5,8);

    glvButton =new QPushButton();
    glvButton->setText("GLV Data");
    //glvButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    glvButton->setMinimumWidth(50);
    glvButton->setMinimumHeight(50);
    mainLayout->addWidget(glvButton,5,7);

    tsiButton =new QPushButton();
    tsiButton->setText("TSI Data");
    //tsiButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsiButton->setMinimumWidth(50);
    tsiButton->setMinimumHeight(50);
    mainLayout->addWidget(tsiButton,5,6);

    tsvButton =new QPushButton();
    tsvButton->setText("TSV Data");
    //tsvButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    tsvButton->setMinimumWidth(50);
    tsvButton->setMinimumHeight(50);
    mainLayout->addWidget(tsvButton,5,5);

    glvBox = new QComboBox();
    //glvBox->setStyleSheet("font:24pt;");
    glvBox->addItem("GlV sensor1");
    glvBox->addItem("GlV sensor2");
    glvBox->addItem("GlV sensor3");
    //glvBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    //glvBox->setMinimumHeight(15);
    //glvBox->setMinimumWidth(15);
    mainLayout->addWidget(glvBox,6,7);

    tsiBox = new QComboBox();
    //glvBox->setStyleSheet("font:24pt;");
    tsiBox->addItem("TSI sensor1");
    tsiBox->addItem("TSI sensor2");
    tsiBox->addItem("TSI sensor3");
    //tsiBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    //tsiBox->setMinimumHeight(15);
    //tsiBox->setMinimumWidth(15);
    mainLayout->addWidget(tsiBox,6,6);

    tsvBox = new QComboBox();
    //glvBox->setStyleSheet("font:24pt;");
    tsvBox->addItem("TSV sensor1");
    tsvBox->addItem("TSV sensor2");
    tsvBox->addItem("TSV sensor3");
    //tsvBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    //tsvBox->setMinimumHeight(15);
    //tsvBox->setMinimumWidth(15);
    mainLayout->addWidget(tsvBox,6,5);


    message = new QListWidget();
    QString  errorMessage;
    errorMessage = "Error3";
    addErrorMessage(errorMessage);
    message->addItem("Error1");
    message->addItem("Error2");
    message->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    //message->setMinimumHeight(10);
    //message->setMinimumWidth(10);
    mainLayout->addWidget(message,7,5,7,6);

    plot = new QCustomPlot();
    plot->addGraph();
    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot->yAxis->setRange(0, -20, Qt::AlignRight);
    plot->xAxis->setRange(0, -20, Qt::AlignRight);
    plot->graph(0)->addData(4,4);
    plot->graph(0)->addData(7,4);
    plot->graph(0)->addData(1,3);
    plot->graph(0)->addData(2,5);
    plot->graph(0)->addData(6,8);
    mainLayout->addWidget(plot,6,0,8,4);


   // QObject::connect(plotButton, SIGNAL (clicked()), this , SLOT(plotGraph()));

}


void MainWindow::drawEdit(QLineEdit * edit, int x, int y ){
    edit= new QLineEdit();
    edit->setText("0.0");
    edit->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    edit->setMinimumWidth(editsize);
    edit->setStyleSheet("font:24pt;");
    mainLayout->addWidget(edit,x,y);
}

void MainWindow::plotGraph(){

}

void MainWindow::addErrorMessage(QString eMessage){
    message->addItem(eMessage);
}

