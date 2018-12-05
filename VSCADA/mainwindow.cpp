#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    postProcessWindow = new postProcess;

    central = new QWidget();
    mainLayout = new QVBoxLayout();
    conf = new Config();
    cout << "Initialized configuration" << endl;
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
    cout << "Done configuring " << endl;
    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    unitWidth=width/20;//100
    unitHeight=height/20;//56

    stringSize = unitWidth/10;//10

    fontSize = QString::number(stringSize);

    QRect rect = QApplication::desktop()->screenGeometry();

    QScrollArea *scrollArea = new QScrollArea();
    tabs = new QTabWidget;
    tabs->setTabsClosable(true);
    QString LabelFont = QString::number(stringSize);
    tabs->setStyleSheet("QTabBar::tab {font:"+LabelFont+"pt}");
    tabs->addTab(central,"General");
    tabs->addTab(postProcessWindow->central, "PostProcessing");
    tabs->setFixedWidth(rect.width() - 18);
    tabs->setFixedHeight(rect.height() - 50);
    QString  font = QString::number(stringSize*1.5);
    tabs->setStyleSheet("font:"+font+"pt;");
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(tabs);

    connect(tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(updateTab(int)));

    for(uint i = 0; i < conf->mainSensors.size(); i++){
        lineEdit = new QLineEdit;
        checkTmr = new QTimer;
        connect(checkTmr, SIGNAL(timeout()), checkTmr, SLOT(stop()));
        connect(checkTmr, SIGNAL(timeout()), this, SLOT(checkTimeout()));
        checkTmr->start(conf->mainSensors.at(i)->checkRate);
        edits.push_back(lineEdit);
        lineEdit->setStyleSheet("font: 20pt; color: #FFFF00");
        lineEdit->setAlignment(Qt::AlignCenter);
        lineEdit->setDisabled(1);
        editTimers.push_back(checkTmr);
    }

    this->setCentralWidget(scrollArea);

    update();

    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (uint i = 0; i < subs.size(); i++){
        connect(subs.at(i), SIGNAL(pushErrMsg(string)), this, SLOT(receiveErrMsg(string)));
        connect(subs.at(i), SIGNAL(pushMessage(string)), this, SLOT(receiveMsg(string)));
        connect(subs.at(i), SIGNAL(valueChanged(meta *)), this, SLOT(updateGraph(meta *)));
        connect(subs.at(i), SIGNAL(updateDisplay(meta *)), this, SLOT(updateEdits(meta *)));
        connect(subs.at(i), SIGNAL(updateEditColor(string, meta *)), this, SLOT(changeEditColor(string, meta *)));
    }
    connect(conf->dataCtrl, SIGNAL(deactivateState(system_state *)), this, SLOT(deactivateStateMW(system_state *)));
    connect(conf->dataCtrl, SIGNAL(updateEdits(meta *)), this, SLOT(updateEdits(meta *)));
    connect(conf->dataCtrl, SIGNAL(activateState(system_state *)), this, SLOT(activateStateMW(system_state *)));
    connect(conf->dataCtrl, SIGNAL(updateFSM(statemachine *)), this, SLOT(updateFSM_MW(statemachine *)));
    connect(conf->canInterface, SIGNAL(pushMsg(string)), this, SLOT(receiveMsg(string)));
    connect(conf->dataCtrl, SIGNAL(pushMessage(string)), this, SLOT(receiveMsg(string)));
    connect(conf->usb7204, SIGNAL(pushMessage(string)), this, SLOT(receiveMsg(string)));
    connect(this, SIGNAL(sendControlValue(int, controlSpec *)), conf->dataCtrl, SLOT(receive_control_val(int, controlSpec *)));

    for (uint i = 0; i < conf->configErrors.size(); i++){
        addErrorMessage(QString::fromStdString(conf->configErrors.at(i)));
    }
}

MainWindow::~MainWindow(){
    delete ui;
}

void MainWindow::update(){

    QRect rec = QApplication::desktop()->screenGeometry();
    int screenWidth = rec.width();

    int fieldColCount = 0;
    int fieldRowCount = 0;

    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (uint i = 0; i < subs.size(); i++){
        vector<meta*> subMeta = subs.at(i)->get_mainMeta();
        if (static_cast<int>(subMeta.size()) > maxSensorRow) maxSensorRow = static_cast<int>(subMeta.size());
    }

    QGridLayout * subsystemSectionLayout = new QGridLayout;
    cout << "NUMBER OF SUBSYSTEMS TO BE DRAWN: " << subs.size() << endl;
    for (uint i = 0; i < subs.size(); i++){

        SubsystemThread * currSub = subs.at(i);
        vector<meta *> subMeta = currSub->get_mainMeta();

        QLabel * headerLabel = new QLabel;
        headerLabel->setFixedWidth(static_cast<int>(unitWidth*1.5));
        headerLabel->setFixedHeight(unitHeight*2);
        headerLabel->setText(QString::fromStdString(currSub->subsystemId));
        QString  subLabelFont = QString::number(stringSize*3);
        headerLabel->setStyleSheet("font:"+subLabelFont+"pt;");
        headerLabel->setAlignment(Qt::AlignCenter);
        headerLabel->setFixedWidth(400);
        subsystemSectionLayout->addWidget(headerLabel,fieldRowCount,fieldColCount,1,2,Qt::AlignCenter);
        fieldRowCount++;

        if(subMeta.size() > 0){
            for (uint j = 0; j < subMeta.size(); j++){

                QLabel * label = new QLabel;
                label->setText(QString::fromStdString(subMeta.at(j)->sensorName));
                label->setFixedWidth(unitWidth*2);
                label->setFixedHeight(static_cast<int>(unitHeight*0.8));
                QString LabelFont = QString::number(stringSize*2);
                label->setStyleSheet("font:"+LabelFont+"pt;");
                subsystemSectionLayout->addWidget(label,fieldRowCount,fieldColCount);

                for (uint k = 0; k < conf->mainSensors.size(); k++){
                    if (conf->mainSensors.at(k) == subMeta.at(j)){
                        QLineEdit * edit = edits.at(k);
                        QString  editFont = QString::number(stringSize*1.8);
                        edit->setStyleSheet("font:"+editFont+"pt;");
                        edit->setFixedWidth(static_cast<int>(unitWidth*2.5));
                        edit->setFixedHeight(static_cast<int>(unitHeight*0.8));
                        edit->setText("--");
                        subsystemSectionLayout->addWidget(edit,fieldRowCount,fieldColCount+1);
                        fieldRowCount++;
                    }
                }
            }
        }

        fieldRowCount = maxSensorRow + 2;

        QFrame * hBorder1 = new QFrame(this);
        hBorder1->setLineWidth(2);
        hBorder1->setMidLineWidth(1);
        hBorder1->setFrameShape(QFrame::HLine);
        hBorder1->setFrameShadow(QFrame::Raised);
        if (fieldColCount != 0) subsystemSectionLayout->addWidget(hBorder1,fieldRowCount,fieldColCount-1,1,4);
        else subsystemSectionLayout->addWidget(hBorder1,fieldRowCount,fieldColCount,1,4);
        fieldRowCount++;

        QPushButton * rebootBtn = new QPushButton;
        rebootBtn->setStyleSheet("font:10pt;");
        rebootBtn->setText("Reboot");
        QPalette rebootPal = rebootBtn->palette();
        rebootPal.setColor(QPalette::Button, QColor(0,100,0));
        rebootBtn->setPalette(rebootPal);
        rebootBtn->setAutoFillBackground(true);
        QString  butLabelFont = QString::number(stringSize*1.4);
        rebootBtn->setStyleSheet("font:"+butLabelFont+"pt;");
        rebootBtn->setFixedWidth(static_cast<int>(unitWidth*1.5));
        rebootBtn->setFixedHeight(static_cast<int>(unitHeight*1.4));
        healthButtons.push_back(rebootBtn);
        subsystemSectionLayout->addWidget(rebootBtn,fieldRowCount,fieldColCount,Qt::AlignCenter);
        connect(rebootBtn, SIGNAL(clicked()), subs.at(i), SLOT(bootSubsystem()));

        QPushButton * detailButton = new QPushButton;
        detailButton->setStyleSheet("font:10pt;");
        detailButton->setText("Detail");
        QPalette detailPal = detailButton->palette();
        detailPal.setColor(QPalette::Button, QColor(0,0,255));
        detailButton->setPalette(detailPal);
        detailButton->setAutoFillBackground(true);
        QString  detailLabelFont = QString::number(stringSize*1.4);
        detailButton->setStyleSheet("font:"+detailLabelFont+"pt;");
        detailButton->setFixedWidth(static_cast<int>(unitWidth*1.5));
        detailButton->setFixedHeight(static_cast<int>(unitHeight*1.4));

        connect(detailButton, SIGNAL(clicked()), this, SLOT(detailButtonPushed()));

        detailButtons.push_back(detailButton);
        subsystemSectionLayout->addWidget(detailButton,fieldRowCount,fieldColCount+1,Qt::AlignCenter);
        fieldRowCount++;

        QFrame * hBorder2 = new QFrame(this);
        hBorder2->setLineWidth(2);
        hBorder2->setMidLineWidth(1);
        hBorder2->setFrameShape(QFrame::HLine);
        hBorder2->setFrameShadow(QFrame::Raised);
        if (fieldColCount != 0) subsystemSectionLayout->addWidget(hBorder2,fieldRowCount,fieldColCount-1,1,4);
        else subsystemSectionLayout->addWidget(hBorder2,fieldRowCount,fieldColCount,1,4);

        fieldRowCount = 0;
        fieldColCount = fieldColCount + 2;

        QFrame * vBorder = new QFrame(this);
        vBorder->setLineWidth(2);
        vBorder->setMidLineWidth(1);
        vBorder->setFrameShape(QFrame::VLine);
        vBorder->setFrameShadow(QFrame::Raised);
        subsystemSectionLayout->addWidget(vBorder,fieldRowCount,fieldColCount,maxSensorRow+5,1);
        fieldColCount++;
    }

    QWidget * subsystemWidget = new QWidget;
    subsystemWidget->setLayout(subsystemSectionLayout);

    QScrollArea * subsystemArea = new QScrollArea;
    subsystemArea->setFixedWidth(screenWidth-30);
    subsystemArea->setFixedHeight(subsystemWidget->height()-85);
    subsystemArea->setBackgroundRole(QPalette::Window);
    subsystemArea->setWidget(subsystemWidget);

    mainLayout->addWidget(subsystemArea);

    QString  butLabelFont = QString::number(stringSize*1.5);
    QString  labelFont = QString::number(stringSize*2);
    QGridLayout * stateButtonLayout = new QGridLayout;

    QLabel * label = new QLabel;
    label->setText("STATES & STATUSES: ");
    label->setStyleSheet("font:"+labelFont+"pt;");
    label->setFixedWidth(label->width());
    stateButtonLayout->addWidget(label,0,0,Qt::AlignLeft);

    QHBoxLayout * btnsLayout = new QHBoxLayout;

    canResetButton =new QPushButton();
    canResetButton->setText("CAN\nReset");
    QPalette palCanRst = canResetButton->palette();
    palCanRst.setColor(QPalette::Button, QColor(0,100,0));
    canResetButton->setPalette(palCanRst);
    canResetButton->setAutoFillBackground(true);
    canResetButton->setStyleSheet("font:"+butLabelFont+"pt;");
    canResetButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
    canResetButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
    btnsLayout->addWidget(canResetButton,Qt::AlignCenter);
    QObject::connect(canResetButton, SIGNAL (clicked()), conf->canInterface , SLOT(rebootCAN()));

    usbResetButton =new QPushButton();
    usbResetButton->setText("USB7204\nReset");
    QPalette palUSBRst = usbResetButton->palette();
    palUSBRst.setColor(QPalette::Button, QColor(0,100,0));
    usbResetButton->setPalette(palUSBRst);
    usbResetButton->setAutoFillBackground(true);
    usbResetButton->setStyleSheet("font:"+butLabelFont+"pt;");
    usbResetButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
    usbResetButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
    btnsLayout->addWidget(usbResetButton,Qt::AlignCenter);
    QObject::connect(usbResetButton, SIGNAL (clicked()), conf->usb7204 , SLOT(rebootUSB7204()));

    QFrame * stateFrame = new QFrame(this);
    stateFrame->setLineWidth(2);
    stateFrame->setMidLineWidth(1);
    stateFrame->setFrameShape(QFrame::VLine);
    stateFrame->setFrameShadow(QFrame::Raised);
    btnsLayout->addWidget(stateFrame, Qt::AlignCenter);

    for(uint t = 0; t < conf->FSMs.size(); t++){
        QVBoxLayout * FSMLayout = new QVBoxLayout;
        QLabel * FSMLabel = new QLabel;
        FSMLabel->setText(QString::fromStdString(conf->FSMs.at(t)->name));
        FSMLabel->setAlignment(Qt::AlignCenter);
        FSMLayout->addWidget(FSMLabel, Qt::AlignCenter);
        stateButton = new QPushButton("---");
        QPalette palplot = stateButton->palette();
        palplot.setColor(QPalette::Button, QColor(70,70,70));
        stateButton->setPalette(palplot);
        stateButton->setAutoFillBackground(true);
        stateButton->setStyleSheet("font:"+butLabelFont+"pt;");
        stateButton->setFixedWidth(static_cast<int>(unitWidth));
        stateButton->setFixedHeight(static_cast<int>(unitHeight*1.5));
        FSMButtons.push_back(stateButton);
        FSMLayout->addWidget(stateButton, Qt::AlignCenter);
        btnsLayout->addLayout(FSMLayout,Qt::AlignCenter);
    }

    for(uint s = 0; s < conf->sysStates.size(); s++){
        if (s == 0){
            QFrame * stateFrame = new QFrame(this);
            stateFrame->setLineWidth(2);
            stateFrame->setMidLineWidth(1);
            stateFrame->setFrameShape(QFrame::VLine);
            stateFrame->setFrameShadow(QFrame::Raised);
            btnsLayout->addWidget(stateFrame, Qt::AlignCenter);
        }

        QFrame * stateFrame = new QFrame(this);
        stateFrame->setLineWidth(2);
        stateFrame->setMidLineWidth(1);
        stateFrame->setFrameShape(QFrame::VLine);
        stateFrame->setFrameShadow(QFrame::Raised);
        btnsLayout->addWidget(stateFrame, Qt::AlignCenter);

        stateButton = new QPushButton(QString::fromStdString(conf->sysStates.at(s)->name));
        QPalette palplot = stateButton->palette();
        palplot.setColor(QPalette::Button, QColor(70,70,70));
        stateButton->setPalette(palplot);
        stateButton->setAutoFillBackground(true);
        stateButton->setStyleSheet("font:"+butLabelFont+"pt;");
        stateButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
        stateButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
        stateButtons.push_back(stateButton);
        btnsLayout->addWidget(stateButton, Qt::AlignCenter);
    }

    stateButtonLayout->addLayout(btnsLayout,0,1,Qt::AlignCenter);

    exitButton =new QPushButton();
    exitButton->setText("EXIT");
    QPalette palexit = exitButton->palette();
    palexit.setColor(QPalette::Button, QColor(0,0,255));
    exitButton->setPalette(palexit);
    exitButton->setAutoFillBackground(true);
    exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
    exitButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
    exitButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
    stateButtonLayout->addWidget(exitButton,0,2,Qt::AlignRight);
    QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(shutdownSystem()));

    mainLayout->addLayout(stateButtonLayout);

    QFrame * stateBorder = new QFrame(this);
    stateBorder->setLineWidth(2);
    stateBorder->setMidLineWidth(1);
    stateBorder->setFrameShape(QFrame::HLine);
    stateBorder->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(stateBorder);

    controlsLayout = new QHBoxLayout;
    currLabel = new QLabel("CONTROLS: ");
    currLabel->setStyleSheet("font:"+labelFont+"pt;");
    controlsLayout->addWidget(currLabel,Qt::AlignLeft);

    for (uint i = 0; i < conf->controlSpecs.size(); i++){
        controlSpec * currSpec = conf->controlSpecs.at(i);
        if (currSpec->slider){
            QVBoxLayout * sliderLayout = new QVBoxLayout;
            currLabel = new QLabel(QString::fromStdString(currSpec->name));
            currLabel->setAlignment(Qt::AlignCenter);
            currLabel->setStyleSheet("font:"+labelFont+"pt;");
            sliderLayout->addWidget(currLabel);
            sliderControl = new QSlider(Qt::Horizontal);
            sliderControl->setRange(currSpec->minslider,currSpec->maxslider);
            QPalette palSlider = sliderControl->palette();
            sliderControl->setMaximumWidth(unitWidth*5);
            sliderLayout->addWidget(sliderControl);
            sliderLayout->setAlignment(Qt::AlignCenter);
            controlsLayout->addLayout(sliderLayout,Qt::AlignCenter);
            controlSliders.push_back(sliderControl);
            sliderCtrls.push_back(currSpec);
            connect(sliderControl, SIGNAL(valueChanged(int)), this, SLOT(sliderValChanged(int)));
        }
        if (currSpec->button){
            buttonControl =new QPushButton();
            buttonControl->setText(QString::fromStdString(currSpec->name));
            QPalette palindi = buttonControl->palette();
            palindi.setColor(QPalette::Button, QColor(142,45,197).darker());
            buttonControl->setPalette(palindi);
            buttonControl->setAutoFillBackground(true);
            buttonControl->setStyleSheet("font:"+butLabelFont+"pt;");
            buttonControl->setFixedWidth(static_cast<int>(unitWidth));
            buttonControl->setFixedHeight(static_cast<int>(unitHeight*1.5));
            controlButtons.push_back(buttonControl);
            buttonCtrls.push_back(currSpec);
            controlsLayout->addWidget(buttonControl,Qt::AlignCenter);
            connect(buttonControl, SIGNAL(pressed()), this, SLOT(ctrlButtonPressed()));
            connect(buttonControl, SIGNAL(released()), this, SLOT(ctrlButtonReleased()));
        }
        if (currSpec->textField){
            QVBoxLayout * fieldLayout = new QVBoxLayout;
            currLabel = new QLabel(QString::fromStdString(currSpec->name));
            currLabel->setAlignment(Qt::AlignCenter);
            currLabel->setStyleSheet("font:"+labelFont+"pt;");
            fieldLayout->addWidget(currLabel);
            editControl = new QLineEdit;
            editControl->setValidator( new QIntValidator(0, 999999999, this) );
            editControl->setMaximumWidth(unitWidth*5);
            editControl->setStyleSheet("font:"+labelFont+"pt;");
            fieldLayout->addWidget(editControl);
            fieldLayout->setAlignment(Qt::AlignCenter);
            controlEdits.push_back(editControl);
            editCtrls.push_back(currSpec);
            controlsLayout->addLayout(fieldLayout,Qt::AlignCenter);
            connect(editControl, SIGNAL(returnPressed()), this, SLOT(editUpdated()));
        }
    }
    mainLayout->addLayout(controlsLayout);

    QFrame * controlBorder = new QFrame(this);
    controlBorder->setLineWidth(2);
    controlBorder->setMidLineWidth(1);
    controlBorder->setFrameShape(QFrame::HLine);
    controlBorder->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(controlBorder);

    QVBoxLayout * msgLayout = new QVBoxLayout;
    QLabel * msgLabel = new QLabel;
    msgLabel->setText("System Log");
    msgLabel->setStyleSheet("font:"+butLabelFont+"pt;");
    msgLabel->setAlignment(Qt::AlignCenter);
    msgLayout->addWidget(msgLabel, Qt::AlignCenter);

    QFont font = QFont ("Courier");
    if (!initialized){
        message = new QListWidget();
    }
    QString  errorMessage;
    errorMessage = "Starting Data Acquisition...";
    addErrorMessage(errorMessage);
    QString  messageFont = QString::number(stringSize*1.5);
    message->setStyleSheet("font:"+messageFont+"pt;");
    message->addItem("FontSize:"+fontSize);
    message->setFixedHeight(unitHeight*6);
    message->setFixedWidth(unitWidth*7);
    message->setFont(font);
    msgLayout->addWidget(message);
    bottomLayout->addLayout(msgLayout,0,0,Qt::AlignCenter);

    QFrame * bottomBorder = new QFrame(this);
    bottomBorder->setLineWidth(2);
    bottomBorder->setMidLineWidth(1);
    bottomBorder->setFrameShape(QFrame::VLine);
    bottomBorder->setFrameShadow(QFrame::Raised);
    bottomLayout->addWidget(bottomBorder,0,1);

    QVBoxLayout * plotLayout = new QVBoxLayout;
    QHBoxLayout * comboLayout = new QHBoxLayout;
    QLabel * plotLabel = new QLabel("Select Plot: ");
    plotLabel->setStyleSheet("font:"+butLabelFont+"pt;");
    plotLabel->setFixedWidth(200);
    plotLabel->setAlignment(Qt::AlignCenter);
    comboLayout->addWidget(plotLabel, Qt:: AlignCenter);

    vector<meta *> sensors = conf->mainSensors;
    plotComboBox->setStyleSheet("font:"+butLabelFont+"pt;");
    plotComboBox->setFixedWidth(300);
    plotComboBox->addItem("None");
    for (uint i = 0; i < sensors.size(); i++){
        plotComboBox->addItem(QString::fromStdString(sensors.at(i)->sensorName));
    }
    comboLayout->addWidget(plotComboBox, Qt::AlignCenter);
    comboLayout->setAlignment(Qt::AlignCenter);
    connect(plotComboBox, SIGNAL(currentTextChanged(QString)), this, SLOT(plotGraph(QString)));

    QHBoxLayout * graphLayout = new QHBoxLayout;
    if (!initialized){
        plot = new QCustomPlot();
        plot->addGraph();
        plot->setFixedHeight(unitHeight*6);
        plot->setFixedWidth(unitWidth*7);
        plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
        plot->graph(0)->setLineStyle(QCPGraph::lsLine);
        plot->yAxis->setRange(0,0, Qt::AlignCenter);
    }
    graphLayout->addWidget(plot, Qt::AlignCenter);

    plotLayout->addLayout(comboLayout, Qt::AlignCenter);
    plotLayout->addLayout(graphLayout, Qt::AlignCenter);

    plotLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->addLayout(plotLayout,0,2,Qt::AlignCenter);
    mainLayout->addLayout(bottomLayout, Qt::AlignCenter);
    central->setMaximumWidth(screenWidth - 50);
    initialized = true;
}

void MainWindow::sliderValChanged(int value){
    cout << "Current Value: " << value << endl;
    QObject* obj = sender();
    for (uint i = 0; i < controlSliders.size(); i++){
        if (obj == controlSliders.at(i)){
            emit sendControlValue(value, sliderCtrls.at(i));
        }
    }
}

void MainWindow::ctrlButtonPressed(){
    QObject* obj = sender();
    for (uint i = 0; i < controlButtons.size(); i++){
        if (obj == controlButtons.at(i)){
            emit sendControlValue(buttonCtrls.at(i)->pressVal, buttonCtrls.at(i));
        }
    }
}

void MainWindow::ctrlButtonReleased(){
    QObject* obj = sender();
    for (uint i = 0; i < controlButtons.size(); i++){
        if (obj == controlButtons.at(i)){
            emit sendControlValue(buttonCtrls.at(i)->releaseVal, buttonCtrls.at(i));
        }
    }
}

void MainWindow::editUpdated(){
    QObject* obj = sender();
    for (uint i = 0; i < controlEdits.size(); i++){
        if (obj == controlEdits.at(i)){
            QLineEdit * edit = static_cast<QLineEdit *>(obj);
            int val = stoi(edit->text().toStdString());
            emit sendControlValue(val, editCtrls.at(i));
            edit->setText("");
        }
    }
}

void MainWindow::activateStateMW(system_state * nextState){
    for(uint i = 0; i < stateButtons.size(); i++){
        if (stateButtons.at(i)->text().toStdString().compare(nextState->name) == 0){
            QPalette palplot = stateButtons.at(i)->palette();
            palplot.setColor(QPalette::Button, QColor(50,205,50));
            stateButtons.at(i)->setPalette(palplot);
        }
    }
}

void MainWindow::deactivateStateMW(system_state * prevState){
    for(uint i = 0; i < stateButtons.size(); i++){
        if (stateButtons.at(i)->text().toStdString().compare(prevState->name) == 0){
            QPalette palplot = stateButtons.at(i)->palette();
            palplot.setColor(QPalette::Button, QColor(70,70,70));
            stateButtons.at(i)->setPalette(palplot);
        }
    }
}

void MainWindow::drawEdit(QLineEdit * edit, int x, int y,QString dataDisplay ){
    edit= new QLineEdit();
    edit->setText(dataDisplay);
    edit->setMinimumWidth(unitWidth);
    edit->setStyleSheet("font:24pt;");
}

void MainWindow::plotGraph(QString sensorName){
    vector<QString> timeData;
    vector<QString> valueData;
    xinit=0;
    gx.clear();
    gy.clear();

    vector<meta *> subMeta = conf->mainSensors;
    if (sensorName.toStdString().compare("None") == 0){
        plot->clearGraphs();
        plot->replot();
        plot->update();
        return;
    }

    plot->addGraph();

    for (uint i = 0; i < subMeta.size(); i++){
        if (subMeta.at(i)->sensorName.compare(sensorName.toStdString()) == 0){
            plotSensor = subMeta.at(i);
            graphMax = (plotSensor->maximum);
            graphMin = (plotSensor->minimum);
            for(uint i = 0; i < conf->subsystems.size(); i++){
                vector<meta*> subSensors = conf->subsystems.at(i)->get_mainMeta();
                for (uint j = 0; j < subSensors.size(); j++){
                    if (plotSensor->sensorName.compare(subSensors.at(j)->sensorName) == 0){
                        string tableName = conf->removeSpaces(conf->subsystems.at(i)->subsystemId) + "_caldata";
                        vector<string>cols;
                        cols.push_back("time");
                        cols.push_back("value");
                        vector<string>rows;
                        timeData = conf->dbase->getTargetColumn(QString::fromStdString(tableName),"time","sensorname",QString::fromStdString(plotSensor->sensorName));
                        valueData = conf->dbase->getTargetColumn(QString::fromStdString(tableName),"value","sensorname",QString::fromStdString(plotSensor->sensorName));
                        while (valueData.size() > timeData.size()){
                            valueData.pop_back();
                        }
                    }
                }
            }
        }
    }

    plot->setFixedHeight(unitHeight*6);
    plot->setFixedWidth(unitWidth*7);
    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);

    for(uint i = 0; i < timeData.size(); i++){
        if (i == timeData.size() -1) plot->xAxis->setRange(0, -(timeData.at(i).toDouble()+1), Qt::AlignRight);
        if (valueData.at(i).toDouble() > graphMax) graphMax = valueData.at(i).toDouble() + 5;
        if (valueData.at(i).toDouble() < graphMin) graphMin = valueData.at(i).toDouble() - 1;
        gx.append(timeData.at(i).toDouble());
        gy.append(valueData.at(i).toDouble());
        plot->graph(0)->setData(gx,gy);
    }

    cout << "Graph Min: " << graphMin << " Graph Max: " << graphMax << endl;

    plot->yAxis->setRange(graphMin, -(graphMax-graphMin), Qt::AlignRight);

    plot->replot();
    plot->update();
}

void MainWindow::receiveMsg(string msg){
    addErrorMessage(QString::fromStdString(msg));
}

void MainWindow::addPoint(double x, double y){
    plot->xAxis->setRange(0, -(x+1), Qt::AlignRight);
    if (y > graphMax) graphMax = y + 5;
    if (y < graphMin) graphMin = y - 1;
    plot->yAxis->setRange(graphMin, -(graphMax-graphMin), Qt::AlignRight);
    gx.append(x);
    gy.append(y);
    plot->graph(0)->setData(gx,gy);
    plot->replot();
    plot->update();
}

void MainWindow::detailButtonPushed(){
    QObject* obj = sender();
    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (uint i = 0; i < detailButtons.size(); i++){
        if (obj == detailButtons.at(i)) emit openDetailWindow(subs.at(i));
    }
}

void MainWindow::receiveErrMsg(string msg){
    QString str = QString::fromStdString(msg);
    addErrorMessage(QString::fromStdString(msg));
    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (uint i = 0; i < subs.size(); i++){
            bool error = subs.at(i)->error;
            if(error){
                QPalette palb = healthButtons.at(i)->palette();
                palb.setColor(QPalette::Button, QColor(255,0,0));
                healthButtons.at(i)->setPalette(palb);
            }
     }
}

void MainWindow::updateFSM_MW(statemachine * currFSM){
    for (uint i = 0; i < conf->FSMs.size(); i++){
        statemachine * currMachine = conf->FSMs.at(i);
        if (currMachine == currFSM){
            for (uint j = 0; j < currMachine->states.size(); j++){
                if (currMachine->states.at(j)->active){
                    FSMButtons.at(i)->setText(QString::fromStdString(currMachine->states.at(j)->name));
                    QPalette palb = FSMButtons.at(i)->palette();
                    palb.setColor(QPalette::Button, QColor(50,205,50));
                    FSMButtons.at(i)->setPalette(palb);
                    return;
                }
            }
        }
    }
}

void MainWindow::addErrorMessage(QString eMessage){
    message->addItem(eMessage);
    message->scrollToBottom();
}

void MainWindow::updateVals(){
// addPoint(xinit,yinit);
//yinit++;
}

void MainWindow::getCurrentSystem(int i){
    currentSystem=i;
}

void MainWindow::openDetailWindow(SubsystemThread *thread){
    detailWindow= new detailPage();
    detailWindow->setConfObject(conf);
    detailWindow->setCurrentSystem(thread);
    tabs->addTab(detailWindow->central, QString::fromStdString(thread->subsystemId + "_Detail"));
    tabs->setCurrentIndex(tabs->count() - 1);
    for (uint i = 0; i < conf->FSMs.size(); i++){
        detailWindow->updateFSM_MW(conf->dataCtrl->FSMs.at(i));
    }
    for (uint i = 0; i < conf->dataCtrl->states.size(); i++){
        if (conf->dataCtrl->states.at(i)->active){
            detailWindow->activateStateMW(conf->dataCtrl->states.at(i));
        }
    }
}

void MainWindow::closeTab(int tabId){
    if ((tabId != 0) && (tabId != 1)) tabs->removeTab(tabId);
    else if (tabId == 0) shutdownSystem();
    tabs->setCurrentIndex(0);
}

void MainWindow:: closeDetailPage(){

}

/**
 * @brief MainWindow::passive_dialog creates a passive dialog box displaying
 *  the respective message. User response is not required
 * @param msg message to be displayed on the dialog box
 * @return 1 upon succesful run
 */
int MainWindow::passive_dialog(string msg){
    QDialog dlg;
    QVBoxLayout la(&dlg);
    QLabel ed;
    ed.setText(QString::fromStdString(msg));
    la.addWidget(&ed);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);

    connect(buttonBox, SIGNAL(accepted()), &dlg, SLOT(close()));

    la.addWidget(buttonBox);
    dlg.setLayout(&la);
    dlg.adjustSize();

    dlg.exec();
    return 1;
}

/**
 * @brief MainWindow::active_dialog creates an interactive dialog on which the
 *  user can either accept or reject a request
 * @param msg message to be displayed on the dialog
 * @return either accepted or rejected
 */
int MainWindow::active_dialog(string msg){
    QDialog dlg;
    QVBoxLayout la(&dlg);
    QLabel ed;
    ed.setText(QString::fromStdString(msg));
    la.addWidget(&ed);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));

    la.addWidget(buttonBox);
    dlg.setLayout(&la);
    int result = dlg.exec();

    if(result == QDialog::Accepted){
        return QDialog::Accepted;
    } else {
        return QDialog::Rejected;
    }
    return QDialog::Rejected;
}

/**
 * @brief MainWindow::info_dialog creates an interactive dialog on which the
 *  user can either accept or reject a request
 * @param msg message to be displayed on the dialog
 * @return either accepted or rejected
 */
string MainWindow::info_dialog(string msg){
    QDialog dlg;
    QLineEdit edit;
    QVBoxLayout la(&dlg);
    QLabel ed;
    ed.setText(QString::fromStdString(msg));
    la.addWidget(&ed);
    la.addWidget(&edit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, SIGNAL(accepted()), &dlg, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), &dlg, SLOT(reject()));

    la.addWidget(buttonBox);
    dlg.setLayout(&la);
reprompt:
    int result = dlg.exec();

    if(result == QDialog::Accepted){
        string str = edit.text().toStdString();
        if (str.compare("") == 0){
            goto reprompt;
        }
        str += ".db";
        return str;
    } else {
        return "0";
    }
}

void MainWindow::shutdownSystem(){
    int confirmation = active_dialog("Save Session?");
    string clearMsg = "Save session as?\n Please enter name without space characters";
    string errMsg = "Name contains space characters. Please try again:";
    bool error = 0;
    string name = "";
    if (confirmation == QDialog::Accepted){

repeat:
        if (error) name = info_dialog(errMsg);
        else name = info_dialog(clearMsg);

        if (name.compare("0") != 0){
            for (uint i = 0; i < name.size(); ++i){
                if (name[i] == ' '){
                    goto repeat;
                }
            }
            conf->dbase->update_value("system_info","endtime","rowid","1",conf->get_curr_time());
            conf->dataCtrl->saveSession(name);
            passive_dialog("Saved!");
        }
        this->close();
    } else {
        conf->dbase->update_value("system_info","endtime","rowid","1",conf->get_curr_time());
        this->close();
    }
    QFile file("../VSCADA/savedsessions/DataBase.txt");
     file.open(QIODevice::WriteOnly | QIODevice::Append);
     QTextStream out(&file);
     QString db = QString::fromStdString(name);
     out<<db<<endl;
     file.flush();

     file.close();

}

/**
 * @brief SubsystemThread::updateEdits - updates text edit fields
 */
void MainWindow::updateEdits(meta * sensor){
    for(uint i = 0; i < edits.size(); i++){
        if(conf->mainSensors.at(i) == sensor){
            double num = sensor->calVal;
            ostringstream streamObj;
            streamObj << fixed;
            streamObj << setprecision(2);
            streamObj << num;
            string val = streamObj.str();
            editTimers.at(i)->start(sensor->checkRate);
            string field = val + " " + sensor->unit;
            edits.at(i)->setText(QString::fromStdString(field));
        }
    }
}

/**
 * @brief SubsystemThread::checkTimeout - checks whether any lineEdit hasn't received updates
 */
void MainWindow::checkTimeout(){
    for(uint i = 0; i < edits.size(); i++){
        if (!editTimers.at(i)->isActive()) edits.at(i)->setText("--");
    }
}

void MainWindow::changeEditColor(string color, meta * sensor){
    for(uint i = 0; i < conf->mainSensors.size(); i++){
        if(color.compare("red") == 0){
            if (conf->mainSensors.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FF0000");
            }
        } else if(color.compare("blue") == 0){
            if (conf->mainSensors.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #1E90FF");
            }
        } else if(color.compare("yellow") == 0){
            if (conf->mainSensors.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FFFF00");
            }
        }
    }
}

void MainWindow::updateTab(int tabId){
    if (tabId == 0){
//        this->update();
    }
}

void MainWindow::updateGraph(meta * sen){
    meta * currSensor;
    vector<meta *> mainSensors = conf->mainSensors;
    string sensorName = plotComboBox->currentText().toStdString();
    for (uint i = 0; i < mainSensors.size(); i++){
        if (mainSensors.at(i)->sensorName.compare(sensorName) == 0){
            currSensor = mainSensors.at(i);
            double data = currSensor->calVal;
            addPoint(stod(conf->dataCtrl->getProgramTime()),data);
        }
    }
}
