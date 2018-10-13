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
//    initMetadata(conf->GLVmeta,conf->TSImeta,conf->TSVmeta,conf->COOLmeta);

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

    update();
    connect(timer, SIGNAL(timeout()), this, SLOT(updateVals()));
    timer->start(500);

    // can bus init here
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::update(){
    int maxSensorRow = 0;
    int sectionCount = 0;
    int fieldVCount = 0;
    int fieldHCount = 0;
    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (int i = 0; i < subs.size(); i++){
        vector<meta*> subMeta = subs.at(i)->get_metadata();
        if (subMeta.size() > maxSensorRow) maxSensorRow = subMeta.size();
    }

    for (int i = 0; i < subs.size(); i++){
        SubsystemThread * currSub = subs.at(i);
        vector<meta *> subMeta = currSub->get_metadata();
        if(subMeta.size() > 0){
            for (int j = 0; j < (int)subMeta.size(); j++){
                fieldVCount = sectionCount;
                fieldVCount++;
                QLabel * label = new QLabel;
                if (j == 0){
                    QLabel * label1 = new QLabel;
                    label1->setText(QString::fromStdString(currSub->subsystemId));
                    label1->setStyleSheet("font:40pt;");
                    mainLayout->addWidget(label1,fieldHCount,fieldVCount);
                    fieldHCount++;
                }
                label->setText(QString::fromStdString(subMeta.at(j)->sensorName));
                label->setStyleSheet("font:20pt;");
                mainLayout->addWidget(label,fieldHCount,fieldVCount);
                fieldVCount++;
                mainLayout->addWidget(currSub->edits.at(j),fieldHCount,fieldVCount);
                fieldHCount++;
            }
            sectionCount += 3;
            fieldHCount = 0;
            QFrame *linea0 = new QFrame(this);
            linea0->setLineWidth(2);
            linea0->setMidLineWidth(1);
            linea0->setFrameShape(QFrame::VLine);
            linea0->setFrameShadow(QFrame::Raised);
            mainLayout->addWidget(linea0,fieldHCount,sectionCount,maxSensorRow+1,1);
        }
    }

//    int maxSensorSize = (int)GLVEdits.size();
//    if ((int)TSIEdits.size() > maxSensorSize) maxSensorSize = (int)TSIEdits.size();
//    if ((int)TSVEdits.size() > maxSensorSize) maxSensorSize = (int)TSVEdits.size();
//    if ((int)COOLINGEdits.size() > maxSensorSize) maxSensorSize = (int)COOLINGEdits.size();

//    for (int i = 0; i < (int)GLVEdits.size(); i++){
//        QLabel * label = new QLabel;
//        if (i == 0){
//            QLabel * label1 = new QLabel;
//            label1->setText("GLV SENSORS");
//            label1->setStyleSheet("font:27pt;");
//            mainLayout->addWidget(label1,0,0);
//        }
//        label->setText(QString::fromStdString(GLV_meta.at(i).sensorName));
//        label->setStyleSheet("font:17pt;");
//        mainLayout->addWidget(label,i+1,0);
//        mainLayout->addWidget(GLVEdits.at(i),i+1,1);
//    }
//    QFrame *linea0 = new QFrame(this);
//    linea0->setLineWidth(2);
//    linea0->setMidLineWidth(1);
//    linea0->setFrameShape(QFrame::VLine);
//    linea0->setFrameShadow(QFrame::Raised);
//    mainLayout->addWidget(linea0,0,2,maxSensorSize+1,1);

//    for (int i = 0; i < (int)TSIEdits.size(); i++){
//        QLabel * label = new QLabel;
//        if (i == 0){
//            QLabel * label1 = new QLabel;
//            label1->setText("TSI SENSORS");
//            label1->setStyleSheet("font:27pt;");
//            mainLayout->addWidget(label1,0,3);
//        }
//        label->setText(QString::fromStdString(TSI_meta.at(i).sensorName));
//        label->setStyleSheet("font:17pt;");
//        mainLayout->addWidget(label,i+1,3);
//        mainLayout->addWidget(TSIEdits.at(i),i+1,4);
//    }

//    QFrame *linea1 = new QFrame(this);
//    linea1->setLineWidth(2);
//    linea1->setMidLineWidth(1);
//    linea1->setFrameShape(QFrame::VLine);
//    linea1->setFrameShadow(QFrame::Raised);
//    mainLayout->addWidget(linea1,0,5,maxSensorSize+1,1);

//    for (int i = 0; i < (int)TSVEdits.size(); i++){
//        QLabel * label = new QLabel;
//        if (i == 0){
//            QLabel * label1 = new QLabel;
//            label1->setText("TSV SENSORS");
//            label1->setStyleSheet("font:27pt;");
//            mainLayout->addWidget(label1,0,6);
//        }
//        label->setText(QString::fromStdString(TSV_meta.at(i).sensorName));
//        label->setStyleSheet("font:17pt;");
//        mainLayout->addWidget(label,i+1,6);
//        mainLayout->addWidget(TSVEdits.at(i),i+1,7);
//    }

//    QFrame *linea2 = new QFrame(this);
//    linea2->setLineWidth(2);
//    linea2->setMidLineWidth(1);
//    linea2->setFrameShape(QFrame::VLine);
//    linea2->setFrameShadow(QFrame::Raised);
//    mainLayout->addWidget(linea2,0,8,maxSensorSize+1,1);

//    for (int i = 0; i < (int)COOLINGEdits.size(); i++){
//        QLabel * label = new QLabel;
//        if (i == 0){
//            QLabel * label1 = new QLabel;
//            label1->setText("COOLING SENSORS");
//            label1->setStyleSheet("font:27pt;");
//            mainLayout->addWidget(label1,0,9);
//        }
//        label->setText(QString::fromStdString(COOLING_meta.at(i).sensorName));
//        label->setStyleSheet("font:17pt;");
//        mainLayout->addWidget(label,i+1,9);
//        mainLayout->addWidget(COOLINGEdits.at(i),i+1,10);
//    }

//    QFrame *linea3 = new QFrame(this);
//    linea3->setLineWidth(2);
//    linea3->setMidLineWidth(1);
//    linea3->setFrameShape(QFrame::HLine);
//    linea3->setFrameShadow(QFrame::Raised);
//    mainLayout->addWidget(linea3,maxSensorSize+2,0,1,11);

//    QLabel * blank1= new QLabel();
//    blank1->setText("                   ");
//    blank1->setStyleSheet("font:50pt;");
//    mainLayout->addWidget(blank1,0,0);

//    glvLabel = new QLabel();
//    glvLabel->setText("GLV");
//    glvLabel->setStyleSheet("font:36pt;");
//    mainLayout->addWidget(glvLabel,0,1);

//    glvLabel1 = new QLabel();
//    glvLabel1->setText("GLV sensor1");
//    glvLabel1->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(glvLabel1,1,0);

//    glvLabel2 = new QLabel();
//    glvLabel2->setText("GLV sensor2");
//    glvLabel2->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(glvLabel2,2,0);

//    glvLabel3 = new QLabel();
//    glvLabel3->setText("GLV sensor3");
//    glvLabel3->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(glvLabel3,3,0);



//    //TSI
//    QLabel * blank2= new QLabel();
//    blank2->setText("                   ");
//    blank2->setStyleSheet("font:50pt;");
//    mainLayout->addWidget(blank2,0,2);

//    tsiLabel = new QLabel();
//    tsiLabel->setText("TSI");
//    tsiLabel->setStyleSheet("font:36pt;");
//    mainLayout->addWidget(tsiLabel,0,3);

//    tsiLabel1 = new QLabel();
//    tsiLabel1->setText("TSI sensor1");
//    tsiLabel1->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(tsiLabel1,1,2);

//    tsiLabel2 = new QLabel();
//    tsiLabel2->setText("TSI sensor2");
//    tsiLabel2->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(tsiLabel2,2,2);

//    tsiLabel3 = new QLabel();
//    tsiLabel3->setText("TSI sensor3");
//    tsiLabel3->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(tsiLabel3,3,2);

//    //tsv
//    QLabel * blank3= new QLabel();
//    blank3->setText("                   ");
//    blank3->setStyleSheet("font:50pt;");
//    mainLayout->addWidget(blank3,0,4);

//    tsvLabel = new QLabel();
//    tsvLabel->setText("TSV");
//    tsvLabel->setStyleSheet("font:36pt;");
//    mainLayout->addWidget(tsvLabel,0,5);

//    tsvLabel1 = new QLabel();
//    tsvLabel1->setText("TSV sensor1");
//    tsvLabel1->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(tsvLabel1,1,4);

//    tsvLabel2 = new QLabel();
//    tsvLabel2->setText("TSV sensor2");
//    tsvLabel2->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(tsvLabel2,2,4);

//    tsvLabel3 = new QLabel();
//    tsvLabel3->setText("TSV sensor3");
//    tsvLabel3->setStyleSheet("font:24pt;");
//    mainLayout->addWidget(tsvLabel3,3,4);

//    drawEdit(glvEdit1,1,1,dataDisplayGLV1);
//    drawEdit(glvEdit2,2,1,dataDisplayGLV2);
//    drawEdit(glvEdit3,3,1,dataDisplayGLV3);
//    drawEdit(tsiEdit1,1,3,dataDisplayTSI1);
//    drawEdit(tsiEdit2,2,3,dataDisplayTSI2);
//    drawEdit(tsiEdit3,3,3,dataDisplayTSI3);
//    drawEdit(tsvEdit11,1,5,dataDisplayTSV11);
//    drawEdit(tsvEdit21,2,5,dataDisplayTSV21);
//    drawEdit(tsvEdit31,3,5,dataDisplayTSV31);
//    drawEdit(tsvEdit12,1,6,dataDisplayTSV12);
//    drawEdit(tsvEdit22,2,6,dataDisplayTSV22);
//    drawEdit(tsvEdit32,3,6,dataDisplayTSV32);
//    drawEdit(tsvEdit13,1,7,dataDisplayTSV13);
//    drawEdit(tsvEdit23,2,7,dataDisplayTSV23);
//    drawEdit(tsvEdit33,3,7,dataDisplayTSV33);
//    drawEdit(tsvEdit14,1,8,dataDisplayTSV14);
//    drawEdit(tsvEdit24,2,8,dataDisplayTSV24);
//    drawEdit(tsvEdit34,3,8,dataDisplayTSV34);

//    QLabel * blank4= new QLabel();
//    blank4->setText("         ");
//    blank4->setStyleSheet("font:38pt;");
//    mainLayout->addWidget(blank4,4,8);

//    plotButton =new QPushButton();
//    plotButton->setText("Plot");
//    plotButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    plotButton->setMinimumWidth(editsize);
//    plotButton->setMinimumHeight(editsize);
//    mainLayout->addWidget(plotButton,5,8);

//    glvButton =new QPushButton();
//    glvButton->setText("GLV Data");
//    //glvButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    glvButton->setMinimumWidth(50);
//    glvButton->setMinimumHeight(50);
//    mainLayout->addWidget(glvButton,5,7);

//    tsiButton =new QPushButton();
//    tsiButton->setText("TSI Data");
//    //tsiButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    tsiButton->setMinimumWidth(50);
//    tsiButton->setMinimumHeight(50);
//    mainLayout->addWidget(tsiButton,5,6);

//    tsvButton =new QPushButton();
//    tsvButton->setText("TSV Data");
//    //tsvButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    tsvButton->setMinimumWidth(50);
//    tsvButton->setMinimumHeight(50);
//    mainLayout->addWidget(tsvButton,5,5);

//    exitButton =new QPushButton();
//    exitButton->setText("EXIT");
//    //tsvButton->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    exitButton->setMinimumWidth(50);
//    exitButton->setMinimumHeight(50);
//    mainLayout->addWidget(exitButton,0,9);

//    glvBox = new QComboBox();
//    //glvBox->setStyleSheet("font:24pt;");
//    glvBox->addItem("GlV sensor1");
//    glvBox->addItem("GlV sensor2");
//    glvBox->addItem("GlV sensor3");
//    //glvBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    //glvBox->setMinimumHeight(15);
//    //glvBox->setMinimumWidth(15);
//    mainLayout->addWidget(glvBox,6,7);

//    tsiBox = new QComboBox();
//    //glvBox->setStyleSheet("font:24pt;");
//    tsiBox->addItem("TSI sensor1");
//    tsiBox->addItem("TSI sensor2");
//    tsiBox->addItem("TSI sensor3");
//    //tsiBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    //tsiBox->setMinimumHeight(15);
//    //tsiBox->setMinimumWidth(15);
//    mainLayout->addWidget(tsiBox,6,6);

//    tsvBox = new QComboBox();
//    //glvBox->setStyleSheet("font:24pt;");
//    tsvBox->addItem("TSV sensor1");
//    tsvBox->addItem("TSV sensor2");
//    tsvBox->addItem("TSV sensor3");
//    //tsvBox->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    //tsvBox->setMinimumHeight(15);
//    //tsvBox->setMinimumWidth(15);
//    mainLayout->addWidget(tsvBox,6,5);


//    message = new QListWidget();
//    QString  errorMessage;
//    errorMessage = "Error3";
//    addErrorMessage(errorMessage);
//    message->addItem("Error1");
//    message->addItem("Error2");
//    message->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
//    //message->setMinimumHeight(10);
//    //message->setMinimumWidth(10);
//    mainLayout->addWidget(message,7,5,7,6);

//   QObject::connect(plotButton, SIGNAL (clicked()), this , SLOT(plotGraph()));
//   QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));
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
    this->update();
}

void MainWindow::initMetadata(vector<meta> glv, vector<meta> tsi, vector<meta> tsv, vector<meta> cooling){
    GLV_meta = glv;
    TSI_meta = tsi;
    TSV_meta = tsv;
    COOLING_meta = cooling;

    for (int i = 0; i < (int)GLV_meta.size(); i++){
        QLineEdit * edit = new QLineEdit;
        edit->setStyleSheet("font:17pt;");
        GLVEdits.push_back(edit);
    }
    for (int i = 0; i < (int)TSI_meta.size(); i++){
        QLineEdit * edit = new QLineEdit;
        edit->setStyleSheet("font:17pt;");
        TSIEdits.push_back(edit);
    }
    for (int i = 0; i < (int)TSV_meta.size(); i++){
        QLineEdit * edit = new QLineEdit;
        edit->setStyleSheet("font:17pt;");
        TSVEdits.push_back(edit);
    }
    for (int i = 0; i < (int)COOLING_meta.size(); i++){
        QLineEdit * edit = new QLineEdit;
        edit->setStyleSheet("font:17pt;");
        COOLINGEdits.push_back(edit);
    }
}

void MainWindow::updateData(vector <int> glvVector,vector <int> tsiVector,vector <int> tsvVector,vector <int> coolingVector ){
    for (int i = 0; i < (int)GLV_meta.size(); i++){
        GLVEdits.at(i)->setText(QString::fromStdString(to_string(glvVector.at(i))));
    }
    for (int i = 0; i < (int)TSI_meta.size(); i++){
        TSIEdits.at(i)->setText(QString::fromStdString(to_string(tsiVector.at(i))));
    }
    for (int i = 0; i < (int)TSV_meta.size(); i++){
        TSVEdits.at(i)->setText(QString::fromStdString(to_string(tsvVector.at(i))));
    }
    for (int i = 0; i < (int)COOLING_meta.size(); i++){
        COOLINGEdits.at(i)->setText(QString::fromStdString(to_string(coolingVector.at(i))));
    }
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



