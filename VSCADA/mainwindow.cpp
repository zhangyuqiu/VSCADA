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

    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    unitWidth=width/16;//100
    unitHeight=height/16;//56

    stringSize = unitWidth/10;//10

    fontSize = QString::number(stringSize);

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
        QComboBox * box = new QComboBox;

        if(subMeta.size() > 0){
            for (int j = 0; j < (int)subMeta.size(); j++){
                fieldVCount = sectionCount;
                fieldVCount++;
                QLabel * label = new QLabel;
                QPushButton * button = new QPushButton;
                if (j == 0){
                    QLabel * label1 = new QLabel;
                    label1->setFixedWidth(unitWidth*1.5);
                    label1->setFixedHeight(unitHeight*2);
                    label1->setText(QString::fromStdString(currSub->subsystemId));
                    QString  subLabelFont = QString::number(stringSize*4);
                    label1->setStyleSheet("font:"+subLabelFont+"pt;");
                    mainLayout->addWidget(label1,fieldHCount,fieldVCount);

                    button->setStyleSheet("font:10pt;");
                    button->setText(QString::fromStdString(currSub->subsystemId)+" Data");
                    QPalette palb = button->palette();
                    palb.setColor(QPalette::Button, QColor(0,255,0));
                    button->setPalette(palb);
                    button->setAutoFillBackground(true);
                    QString  butLabelFont = QString::number(stringSize*1.4);
                    button->setStyleSheet("font:"+butLabelFont+"pt;");
                    button->setFixedWidth(unitWidth*1.5);
                    button->setFixedHeight(unitHeight*1.4);
                    mainLayout->addWidget(button,maxSensorRow+3,fieldVCount);

                   fieldHCount++;

                   box->setFixedWidth(unitWidth*1.5);
                   box->setFixedHeight(unitHeight*1.4);
                   QString  boxFont = QString::number(stringSize*1.2);
                   box->setStyleSheet("font:"+boxFont+"pt;");
                   mainLayout->addWidget(box,maxSensorRow+3,fieldVCount+1);
                }
                label->setText(QString::fromStdString(subMeta.at(j)->sensorName));
                label->setFixedWidth(unitWidth*2);
                label->setFixedHeight(unitHeight*0.8);
                QString  LabelFont = QString::number(stringSize*2);
                label->setStyleSheet("font:"+LabelFont+"pt;");
                box->addItem(QString::fromStdString(subMeta.at(j)->sensorName));
                mainLayout->addWidget(label,fieldHCount,fieldVCount);
                fieldVCount++;
                QLineEdit * edit = currSub->edits.at(j);
                QString  editFont = QString::number(stringSize*1.8);
                edit->setStyleSheet("font:"+editFont+"pt;");
                edit->setFixedWidth(unitWidth*2.5);
                edit->setFixedHeight(unitHeight*0.8);
                mainLayout->addWidget(edit,fieldHCount,fieldVCount);
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

    QString  butLabelFont = QString::number(stringSize*1.5);





        plotButton =new QPushButton();
        plotButton->setText("Plot");
        QPalette palplot = plotButton->palette();
        palplot.setColor(QPalette::Button, QColor(0,0,255));
        plotButton->setPalette(palplot);
        plotButton->setAutoFillBackground(true);
        plotButton->setStyleSheet("font:"+butLabelFont+"pt;");
        plotButton->setFixedWidth(unitWidth*1.2);
        plotButton->setFixedHeight(unitHeight*1.8);
        mainLayout->addWidget(plotButton,maxSensorRow+4,11);


        exitButton =new QPushButton();
        exitButton->setText("EXIT");
        QPalette palexit = exitButton->palette();
        palexit.setColor(QPalette::Button, QColor(0,0,255));
        exitButton->setPalette(palexit);
        exitButton->setAutoFillBackground(true);
        exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
        exitButton->setFixedWidth(unitWidth*1.2);
        exitButton->setFixedHeight(unitHeight*1.8);
        mainLayout->addWidget(exitButton,maxSensorRow+4,10);

        message = new QListWidget();
        QString  errorMessage;
        errorMessage = "Error3";
        addErrorMessage(errorMessage);
        QString  messageFont = QString::number(stringSize*2);
        message->setStyleSheet("font:"+messageFont+"pt;");
        message->addItem("Error1");
        message->addItem("Error2");
        message->addItem(fontSize);
        message->setFixedHeight(unitHeight*6);
        message->setFixedWidth(unitWidth*7);
        mainLayout->addWidget(message,maxSensorRow+5,8,maxSensorRow+11,10);

        QObject::connect(plotButton, SIGNAL (clicked()), this , SLOT(plotGraph()));
        QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));
}


void MainWindow::drawEdit(QLineEdit * edit, int x, int y,QString dataDisplay ){
    edit= new QLineEdit();
    edit->setText(dataDisplay);
    edit->setSizePolicy(QSizePolicy::Ignored,QSizePolicy::Preferred);
    edit->setMinimumWidth(unitWidth);
    edit->setStyleSheet("font:24pt;");
    mainLayout->addWidget(edit,x,y);
}

void MainWindow::plotGraph(){
    int maxSensorRow = 0;
    vector<SubsystemThread *> subs;
    subs = conf->subsystems;

    for (int i = 0; i < subs.size(); i++){
        vector<meta*> subMeta = subs.at(i)->get_metadata();
        if (subMeta.size() > maxSensorRow) maxSensorRow = subMeta.size();
    }
    plot = new QCustomPlot();
    plot->addGraph();
    plot->setFixedHeight(unitHeight*8);
    plot->setFixedWidth(unitWidth*9);
    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot->yAxis->setRange(0, -20, Qt::AlignRight);
    plot->xAxis->setRange(0, -20, Qt::AlignRight);
    plot->graph(0)->addData(4,4);
    plot->graph(0)->addData(7,4);
    plot->graph(0)->addData(1,3);
    plot->graph(0)->addData(2,5);
    plot->graph(0)->addData(6,8);
    mainLayout->addWidget(plot,maxSensorRow+5,0,maxSensorRow+11,6);
}

void MainWindow::addErrorMessage(QString eMessage){
    message->addItem(eMessage);
}

void MainWindow::updateVals(){
//    this->update();
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

    update();

}



