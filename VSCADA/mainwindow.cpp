#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    central = new QWidget();
    mainLayout = new QGridLayout();
    conf = new Config();
    timer = new QTimer();

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);
    conf->read_config_file_data("config_test.txt");

//    central->setStyleSheet("background-color: red;");


    editsize=100;
    xinit=0;
    yinit=0;

    dataDisplayGLV1= "0.0";
    dataDisplayGLV2= "0.0";
    dataDisplayGLV3= "0.0";

    dataDisplayTSI1= "0.0";
    dataDisplayTSI2= "0.0";
    dataDisplayTSI3= "0.0";

    dataDisplayTSV11= "0.0";
    dataDisplayTSV21= "0.0";
    dataDisplayTSV31= "0.0";

    dataDisplayTSV12= "0.0";
    dataDisplayTSV22= "0.0";
    dataDisplayTSV32= "0.0";

    dataDisplayTSV13= "0.0";
    dataDisplayTSV23= "0.0";
    dataDisplayTSV33= "0.0";

    dataDisplayTSV14= "0.0";
    dataDisplayTSV24= "0.0";
    dataDisplayTSV34= "0.0";


    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);

    this->setCentralWidget(scrollArea);

    connect(timer, SIGNAL(timeout()), this, SLOT(updateVals()));
    timer->start(500);

    // can bus init here
    //canbus_interface *c = new canbus_interface();

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

    glvLabel = new QLabel();
    glvLabel->setText("GLV");
    glvLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(glvLabel,0,1);

    glvLabel1 = new QLabel();
    glvLabel1->setText("GLV sensor1");
    glvLabel1->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvLabel1,1,0);

    glvLabel2 = new QLabel();
    glvLabel2->setText("GLV sensor2");
    glvLabel2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvLabel2,2,0);

    glvLabel3 = new QLabel();
    glvLabel3->setText("GLV sensor3");
    glvLabel3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(glvLabel3,3,0);



    //TSI
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

    tsiLabel2 = new QLabel();
    tsiLabel2->setText("TSI sensor2");
    tsiLabel2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiLabel2,2,2);

    tsiLabel3 = new QLabel();
    tsiLabel3->setText("TSI sensor3");
    tsiLabel3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsiLabel3,3,2);

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

    tsvLabel2 = new QLabel();
    tsvLabel2->setText("TSV sensor2");
    tsvLabel2->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvLabel2,2,4);

    tsvLabel3 = new QLabel();
    tsvLabel3->setText("TSV sensor3");
    tsvLabel3->setStyleSheet("font:24pt;");
    mainLayout->addWidget(tsvLabel3,3,4);

    drawEdit(glvEdit1,1,1,dataDisplayGLV1);
    drawEdit(glvEdit2,2,1,dataDisplayGLV2);
    drawEdit(glvEdit3,3,1,dataDisplayGLV3);
    drawEdit(tsiEdit1,1,3,dataDisplayTSI1);
    drawEdit(tsiEdit2,2,3,dataDisplayTSI2);
    drawEdit(tsiEdit3,3,3,dataDisplayTSI3);
    drawEdit(tsvEdit11,1,5,dataDisplayTSV11);
    drawEdit(tsvEdit21,2,5,dataDisplayTSV21);
    drawEdit(tsvEdit31,3,5,dataDisplayTSV31);
    drawEdit(tsvEdit12,1,6,dataDisplayTSV12);
    drawEdit(tsvEdit22,2,6,dataDisplayTSV22);
    drawEdit(tsvEdit32,3,6,dataDisplayTSV32);
    drawEdit(tsvEdit13,1,7,dataDisplayTSV13);
    drawEdit(tsvEdit23,2,7,dataDisplayTSV23);
    drawEdit(tsvEdit33,3,7,dataDisplayTSV33);
    drawEdit(tsvEdit14,1,8,dataDisplayTSV14);
    drawEdit(tsvEdit24,2,8,dataDisplayTSV24);
    drawEdit(tsvEdit34,3,8,dataDisplayTSV34);

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

    exitButton =new QPushButton();
    exitButton->setText("EXIT");
    //tsvButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    exitButton->setMinimumWidth(50);
    exitButton->setMinimumHeight(50);
    mainLayout->addWidget(exitButton,0,9);

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




   QObject::connect(plotButton, SIGNAL (clicked()), this , SLOT(plotGraph()));
   QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));

}


void MainWindow::drawEdit(QLineEdit * edit, int x, int y,QString dataDisplay ){
    edit= new QLineEdit();
    edit->setText(dataDisplay);
    edit->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    edit->setMinimumWidth(editsize);
    edit->setStyleSheet("font:24pt;");
    mainLayout->addWidget(edit,x,y);
}

void MainWindow::plotGraph(){

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
}

void MainWindow::addErrorMessage(QString eMessage){
    message->addItem(eMessage);
}

void MainWindow::updateVals(){
    vector<int> glvData = conf->glv_thread->get_GLV_Data();
    vector<int> tsiData = conf->tsi_thread->get_TSI_Data();
    vector<int> tsvData = conf->tsv_thread->get_TSV_Data();
    vector<int> coolData = conf->cool_thread->get_COOL_data();
    updateData(glvData,tsiData,tsvData,coolData);
}

void MainWindow::updateData(vector <int> glvVector,vector <int> tsiVector,vector <int> tsvVector,vector <int> cooling ){
//    dataDisplayGLV1 = QString::fromStdString(to_string(glvVector[0]));
//    dataDisplayGLV2 = QString::fromStdString(to_string(glvVector[1]));
//    dataDisplayGLV3 = QString::fromStdString(to_string(glvVector[2]));
//    dataDisplayTSI1 = QString::fromStdString(to_string(tsiVector[0]));
//    dataDisplayTSI1 = QString::fromStdString(to_string(tsiVector[1]));
//    dataDisplayTSI1 = QString::fromStdString(to_string(tsiVector[2]));
//    dataDisplayTSV11 = QString::fromStdString(to_string(tsvVector[0]));
//    dataDisplayTSV12 = QString::fromStdString(to_string(tsvVector[1]));
//    dataDisplayTSV21 = QString::fromStdString(to_string(tsvVector[2]));
//    dataDisplayTSV22 = QString::fromStdString(to_string(tsvVector[3]));
//    dataDisplayTSV31 = QString::fromStdString(to_string(tsvVector[4]));
//    dataDisplayTSV32 = QString::fromStdString(to_string(tsvVector[5]));
    update();

}



