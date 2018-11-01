#include "detailpage.h"
#include "ui_detailpage.h"

detailPage::detailPage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::detailPage)
{
    ui->setupUi(this);

    central = new QWidget();
    mainLayout = new QGridLayout();
    conf = new Config();
    timer = new QTimer();
    currentSubSystem=0;
    currentSystem=0;
    detail=false;

    plot = new QCustomPlot();
    plot->addGraph();

    xinit=0;
    yinit=0;
    maxSensorRow = 0;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);
    conf->read_config_file_data();
    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    unitWidth=width/20;//100
    unitHeight=height/20;//56

    stringSize = unitWidth/10;//10

    fontSize = QString::number(stringSize);

    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);

    this->setCentralWidget(scrollArea);

    //update();


//    connect(timer, SIGNAL(timeout()), this, SLOT(updateVals()));
//    timer->start(500);

    // can bus init here
}

detailPage::~detailPage()
{
    delete ui;
}

void detailPage::update(){

    int sectionCount = 0;
    int fieldVCount = 0;
    int fieldHCount = 0;
    int maxCtrlCount = 0;
    vector<controlSpec> ctrls;

    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (uint i = 0; i < subs.size(); i++){
        vector<meta*> subMeta = subs.at(i)->get_metadata();
        if (static_cast<int>(subMeta.size()) > maxSensorRow) maxSensorRow = static_cast<int>(subMeta.size());
    }

//    for (uint i = 0; i < subs.size(); i++){
//        ctrls = subs.at(i)->get_controlspecs();
//        if (static_cast<int>(ctrls.size()) > maxCtrlCount) maxCtrlCount = static_cast<int>(ctrls.size());
//    }

//    maxSensorRow += maxCtrlCount;

     systemName = new QString *[subs.size()];

     for(int i=0;i<subs.size();i++){
         systemName[i]= new QString [maxSensorRow];
     }

      for(int i=0;i<subs.size();i++){
     for(int j=0;j<maxSensorRow;j++){

           systemName[i][j]="0";
         }
     }


//main page
        int i=currentSystem;
        SubsystemThread * currSub = subs.at(i);
        vector<meta *> subMeta = currSub->get_metadata();
        QComboBox * box = new QComboBox;

        if(subMeta.size() > 0){
            for (uint j = 0; j < subMeta.size(); j++){
               if(fieldVCount>11){
                  fieldVCount=0;
                  fieldHCount++;
               }
                //fieldVCount = sectionCount;
                fieldVCount++;
                QLabel * label = new QLabel;
                QPushButton * button = new QPushButton;
                if (j == 0){
                    QLabel * label1 = new QLabel;
                    label1->setFixedWidth(static_cast<int>(unitWidth*1.5));
                    label1->setFixedHeight(unitHeight*2);
                    label1->setText(QString::fromStdString(currSub->subsystemId));
                    QString  subLabelFont = QString::number(stringSize*4);
                    label1->setStyleSheet("font:"+subLabelFont+"pt;");
                    mainLayout->addWidget(label1,fieldHCount,fieldVCount,1,2,Qt::AlignCenter);

                    QFrame *linea0 = new QFrame(this);
                    linea0->setLineWidth(2);
                    linea0->setMidLineWidth(1);
                    linea0->setFrameShape(QFrame::VLine);
                    linea0->setFrameShadow(QFrame::Raised);
                    mainLayout->addWidget(linea0,0,sectionCount,maxSensorRow+2,1);

                    button->setStyleSheet("font:10pt;");
                    button->setText(QString::fromStdString(currSub->subsystemId)+" Data");
                    QPalette palb = button->palette();

                    if(currSub->error){
                    palb.setColor(QPalette::Button, QColor(255,0,0));
                    }else{
                    palb.setColor(QPalette::Button, QColor(0,255,0));
                    }

                    button->setPalette(palb);
                    button->setAutoFillBackground(true);
                    QString  butLabelFont = QString::number(stringSize*1.4);
                    button->setStyleSheet("font:"+butLabelFont+"pt;");
                    button->setFixedWidth(static_cast<int>(unitWidth*1.5));
                    button->setFixedHeight(static_cast<int>(unitHeight*1.4));
                    systemButton.push_back(button);
                    mainLayout->addWidget(button,0,fieldVCount+3,Qt::AlignCenter);



                   box->setFixedWidth(static_cast<int>(unitWidth*1.5));
                   box->setFixedHeight(static_cast<int>(unitHeight*1.4));
                   QString  boxFont = QString::number(stringSize*1.2);
                   box->setStyleSheet("font:"+boxFont+"pt;");
                   mainLayout->addWidget(box,0,fieldVCount+4,Qt::AlignCenter);
                    fieldHCount++;
                }

                label->setText(QString::fromStdString(subMeta.at(j)->sensorName));
                label->setFixedWidth(unitWidth*2);
                label->setFixedHeight(static_cast<int>(unitHeight*0.8));
                QString  LabelFont = QString::number(stringSize*2);
                label->setStyleSheet("font:"+LabelFont+"pt;");
                box->addItem(QString::fromStdString(subMeta.at(j)->sensorName));
                QString name=QString::fromStdString(subMeta.at(j)->sensorName);

                systemName[i][j]=name;

                mainLayout->addWidget(label,fieldHCount,fieldVCount);
                fieldVCount++;



                QLineEdit * edit = currSub->edits.at(j);
                QString  editFont = QString::number(stringSize*1.8);
                edit->setStyleSheet("font:"+editFont+"pt;");
                edit->setFixedWidth(static_cast<int>(unitWidth*2.5));
                edit->setFixedHeight(static_cast<int>(unitHeight*0.8));
                mainLayout->addWidget(edit,fieldHCount,fieldVCount);
               fieldVCount++;


               sectionCount += 3;

               if(j<4){

               QFrame *linea0 = new QFrame(this);
               linea0->setLineWidth(2);
               linea0->setMidLineWidth(1);
               linea0->setFrameShape(QFrame::VLine);
               linea0->setFrameShadow(QFrame::Raised);
               mainLayout->addWidget(linea0,0,sectionCount,maxSensorRow+2,1);
              }

            }



            QFrame *linea1 = new QFrame(this);
            linea1->setLineWidth(2);
            linea1->setMidLineWidth(1);
            linea1->setFrameShape(QFrame::VLine);
            linea1->setFrameShadow(QFrame::Raised);
            mainLayout->addWidget(linea1,maxSensorRow+1,sectionCount,maxSensorRow,1);


            systemBox.push_back(box);

        }

    QFrame *linea0 = new QFrame(this);
    linea0->setLineWidth(2);
    linea0->setMidLineWidth(1);
    linea0->setFrameShape(QFrame::HLine);
    linea0->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(linea0,maxSensorRow+2,0,1,13);


    //main page end




    QString  butLabelFont = QString::number(stringSize*1.5);
    QString  labelFont = QString::number(stringSize*3);

    QHBoxLayout * stateButtonLayout = new QHBoxLayout;
    QLabel * label = new QLabel;
    label->setText("STATES: ");
    label->setStyleSheet("font:"+labelFont+"pt;");
    mainLayout->addWidget(label,maxSensorRow+5,1);

    QFrame * stateFrame = new QFrame(this);
    stateFrame->setLineWidth(2);
    stateFrame->setMidLineWidth(1);
    stateFrame->setFrameShape(QFrame::VLine);
    stateFrame->setFrameShadow(QFrame::Raised);
    stateButtonLayout->addWidget(stateFrame);

    for(uint t = 0; t < conf->FSMs.size(); t++){
        statemachine * currFSM = conf->FSMs.at(t);
//            stateButton = new QPushButton(QString::fromStdString(currFSM->name));
            stateButton = new QPushButton("---");
            QPalette palplot = stateButton->palette();
            palplot.setColor(QPalette::Button, QColor(70,70,70));
            stateButton->setPalette(palplot);
            stateButton->setAutoFillBackground(true);
            stateButton->setStyleSheet("font:"+butLabelFont+"pt;");
            stateButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
            stateButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
            FSMButtons.push_back(stateButton);
            stateButtonLayout->addWidget(stateButton);
    }

    for(uint s = 0; s < conf->sysStates.size(); s++){
        if (s == 0){
            QFrame * stateFrame = new QFrame(this);
            stateFrame->setLineWidth(2);
            stateFrame->setMidLineWidth(1);
            stateFrame->setFrameShape(QFrame::VLine);
            stateFrame->setFrameShadow(QFrame::Raised);
            stateButtonLayout->addWidget(stateFrame);
        }

        QFrame * stateFrame = new QFrame(this);
        stateFrame->setLineWidth(2);
        stateFrame->setMidLineWidth(1);
        stateFrame->setFrameShape(QFrame::VLine);
        stateFrame->setFrameShadow(QFrame::Raised);
        stateButtonLayout->addWidget(stateFrame);

        stateButton = new QPushButton(QString::fromStdString(conf->sysStates.at(s)->name));
        QPalette palplot = stateButton->palette();
        palplot.setColor(QPalette::Button, QColor(70,70,70));
        stateButton->setPalette(palplot);
        stateButton->setAutoFillBackground(true);
        stateButton->setStyleSheet("font:"+butLabelFont+"pt;");
        stateButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
        stateButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
        stateButtons.push_back(stateButton);
        stateButtonLayout->addWidget(stateButton);
    }

    mainLayout->addLayout(stateButtonLayout,maxSensorRow+5,2,1,8,Qt::AlignCenter);

    plotButton =new QPushButton();
    plotButton->setText("Plot");
    QPalette palplot = plotButton->palette();
    palplot.setColor(QPalette::Button, QColor(0,0,255));
    plotButton->setPalette(palplot);
    plotButton->setAutoFillBackground(true);
    plotButton->setStyleSheet("font:"+butLabelFont+"pt;");
    plotButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
    plotButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
    mainLayout->addWidget(plotButton,maxSensorRow+5,11,Qt::AlignCenter);


    exitButton =new QPushButton();
    exitButton->setText("EXIT");
    QPalette palexit = exitButton->palette();
    palexit.setColor(QPalette::Button, QColor(0,0,255));
    exitButton->setPalette(palexit);
    exitButton->setAutoFillBackground(true);
    exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
    exitButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
    exitButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
    mainLayout->addWidget(exitButton,maxSensorRow+5,10,Qt::AlignCenter);


    QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));


    QFrame *linea2 = new QFrame(this);
    linea2->setLineWidth(2);
    linea2->setMidLineWidth(1);
    linea2->setFrameShape(QFrame::HLine);
    linea2->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(linea2,maxSensorRow+6,0,1,13);

    QFrame *linea3 = new QFrame(this);
    linea3->setLineWidth(2);
    linea3->setMidLineWidth(1);
    linea3->setFrameShape(QFrame::HLine);
    linea3->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(linea3,maxSensorRow+6,0,1,13);

    mainLayout->addWidget(systemButton.at(0),maxSensorRow+7,7,Qt::AlignCenter);
    mainLayout->addWidget(systemBox.at(0),maxSensorRow+8,7,Qt::AlignCenter);

    QFont font = QFont ("Courier");
    message = new QListWidget();
    QString  errorMessage;
    errorMessage = "Begin";
    addErrorMessage(errorMessage);
    QString  messageFont = QString::number(stringSize*1.5);
    QString  messagedebug = QString::number(currentSystem);
    message->setStyleSheet("font:"+messageFont+"pt;");
    message->addItem(messagedebug);
    message->addItem("FontSize:"+fontSize);
    message->setFixedHeight(unitHeight*6);
    message->setFixedWidth(unitWidth*7);
    message->setFont(font);
    mainLayout->addWidget(message,maxSensorRow+7,8,maxSensorRow+11,10);

    QObject::connect(plotButton, SIGNAL (clicked()), this , SLOT(plotGraph()));



    connect(subs.at(currentSystem), SIGNAL(pushErrMsg(string)), this, SLOT(receiveErrMsg(string)));

    connect(conf->dataCtrl, SIGNAL(deactivateState(system_state *)), this, SLOT(deactivateStateMW(system_state *)));
    connect(conf->dataCtrl, SIGNAL(activateState(system_state *)), this, SLOT(activateStateMW(system_state *)));




}

void detailPage::activateStateMW(system_state * nextState){
//    cout << "State Name: " << nextState->name << endl;
    for(uint i = 0; i < stateButtons.size(); i++){
//        cout << "Comparing: " << nextState->name << " and " << stateButtons.at(i)->text().toStdString() << endl;
        if (stateButtons.at(i)->text().toStdString().compare(nextState->name) == 0){
//            cout << "We're in!" << endl;
            QPalette palplot = stateButtons.at(i)->palette();
            palplot.setColor(QPalette::Button, QColor(50,205,50));
            stateButtons.at(i)->setPalette(palplot);
        }
    }
}

void detailPage::deactivateStateMW(system_state * prevState){
//    cout << "Inside deactivation " << endl;
    for(uint i = 0; i < stateButtons.size(); i++){
        if (stateButtons.at(i)->text().toStdString().compare(prevState->name) == 0){
            cout << "We're in " << prevState->name << endl;
            QPalette palplot = stateButtons.at(i)->palette();
            palplot.setColor(QPalette::Button, QColor(70,70,70));
            stateButtons.at(i)->setPalette(palplot);
        }
    }
}

void detailPage::drawEdit(QLineEdit * edit, int x, int y,QString dataDisplay ){
    edit= new QLineEdit();
    edit->setText(dataDisplay);
    //edit->setSizePolicy(QSizePolicy::Ignored,QSizePolicy:referred);
    edit->setMinimumWidth(unitWidth);
    edit->setStyleSheet("font:24pt;");
    mainLayout->addWidget(edit,x,y);
}



void detailPage::receiveMsg(string msg){
    addErrorMessage(QString::fromStdString(msg));
}

void detailPage::plotGraph(){

    int max =20;
    int mini=0;
    xinit=0;

    gx.clear();
    gy.clear();

    vector<SubsystemThread *> subs;
     subs = conf->subsystems;
    SubsystemThread * currSub = subs.at(currentSystem);




    vector<meta *> subMeta = currSub->get_metadata();
    QString choice= systemBox.at(0)->currentText();
    for(int i=0;i<maxSensorRow;i++){
       int x = choice.compare(systemName[currentSystem][i]);
        if(x==0){
        currentSubSystem=i;
        }
    }


    meta * sensor =subMeta.at(currentSubSystem);
    max = (sensor->maximum)*1.5;
    mini = (sensor->minimum)*0.5;
    plot = new QCustomPlot();
    plot->addGraph();
    plot->setFixedHeight(unitHeight*8);
    plot->setFixedWidth(unitWidth*9);
    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot->yAxis->setRange(mini, -(max-mini), Qt::AlignRight);
    mainLayout->addWidget(plot,maxSensorRow+7,0,maxSensorRow+11,6);
}



void detailPage::addErrorMessage(QString eMessage){
    message->addItem(eMessage);
    message->scrollToBottom();
}

void detailPage::updateVals(){
// addPoint(xinit,yinit);
//yinit++;
}


void detailPage::setCurrentSystem(int current){
    this->currentSystem=current;
    update();
}


