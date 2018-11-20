#include "detailpage.h"
#include "ui_detailpage.h"

detailPage::detailPage(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::detailPage)
{
//    ui->setupUi(this);

    central = new QWidget();
    mainLayout = new QVBoxLayout();
    timer = new QTimer();
    currentSystem=0;
    detail=false;

    plot = new QCustomPlot();
    plot->addGraph();

    xinit=0;
    yinit=0;
    maxSensorRow = 0;

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);

    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    unitWidth=width/20;//100
    unitHeight=height/20;//56

    stringSize = unitWidth/10;//10

    fontSize = QString::number(stringSize);

}

detailPage::~detailPage()
{
    delete ui;
}

void detailPage::setConfObject(Config * config){
    this->conf = config;
    connect(conf->dataCtrl, SIGNAL(deactivateState(system_state *)), this, SLOT(deactivateStateMW(system_state *)));
    connect(conf->dataCtrl, SIGNAL(updateEdits(meta *)), this, SLOT(updateEdits(meta *)));
    connect(conf->dataCtrl, SIGNAL(activateState(system_state *)), this, SLOT(activateStateMW(system_state *)));
    connect(conf->dataCtrl, SIGNAL(updateFSM(statemachine *)), this, SLOT(updateFSM_MW(statemachine *)));
    connect(conf->dataCtrl, SIGNAL(pushMessage(string)), this, SLOT(receiveMsg(string)));
    connect(conf->usb7204, SIGNAL(pushMessage(string)), this, SLOT(receiveMsg(string)));
    connect(this, SIGNAL(sendControlValue(int, controlSpec *)), conf->dataCtrl, SLOT(receive_control_val(int, controlSpec *)));
}

void detailPage::update(){
    vector<meta *> allSensors = currentSubSystem->get_metadata();

    for(uint i = 0; i < allSensors.size(); i++){
        lineEdit = new QLineEdit;
        checkTmr = new QTimer;
        connect(checkTmr, SIGNAL(timeout()), checkTmr, SLOT(stop()));
        connect(checkTmr, SIGNAL(timeout()), this, SLOT(checkTimeout()));
        checkTmr->start(allSensors.at(i)->checkRate);
        edits.push_back(lineEdit);
        lineEdit->setStyleSheet("font: 20pt; color: #FFFF00");
        lineEdit->setAlignment(Qt::AlignCenter);
        lineEdit->setDisabled(1);
        editTimers.push_back(checkTmr);
    }

    QRect rec = QApplication::desktop()->screenGeometry();
    int screenWidth = rec.width();

    int fieldColCount = 0;
    int fieldRowCount = 0;

    vector<SubsystemThread *> subs;
    subs = conf->subsystems;

    QGridLayout * subsystemSectionLayout = new QGridLayout;

    vector<meta *> subMeta = currentSubSystem->get_metadata();

    QLabel * headerLabel = new QLabel;
    headerLabel->setFixedWidth(static_cast<int>(unitWidth*1.5));
    headerLabel->setFixedHeight(unitHeight*2);
    headerLabel->setText(QString::fromStdString(currentSubSystem->subsystemId));
    QString  subLabelFont = QString::number(stringSize*3);
    headerLabel->setStyleSheet("font:"+subLabelFont+"pt;");
    headerLabel->setFixedWidth(screenWidth - 50);
    headerLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(headerLabel,Qt::AlignCenter);

    if(subMeta.size() > 0){
        for (uint j = 0; j < subMeta.size(); j++){
            if (fieldColCount >= 12){
                fieldRowCount ++;
                fieldColCount = 0;
            }
            QLabel * label = new QLabel;
            label->setText(QString::fromStdString(subMeta.at(j)->sensorName));
            label->setFixedWidth(unitWidth*2);
            label->setFixedHeight(static_cast<int>(unitHeight*0.8));
            QString LabelFont = QString::number(stringSize*2);
            label->setStyleSheet("font:"+LabelFont+"pt;");
            subsystemSectionLayout->addWidget(label,fieldRowCount,fieldColCount);

            for (uint k = 0; k < allSensors.size(); k++){
                if (allSensors.at(k) == subMeta.at(j)){
                    QLineEdit * edit = edits.at(k);
                    QString  editFont = QString::number(stringSize*1.8);
                    edit->setStyleSheet("font:"+editFont+"pt;");
                    edit->setFixedWidth(static_cast<int>(unitWidth*2.5));
                    edit->setFixedHeight(static_cast<int>(unitHeight*0.8));
                    subsystemSectionLayout->addWidget(edit,fieldRowCount,fieldColCount+1);
                    fieldColCount += 2;
                }
            }

            if(fieldColCount != 11 && j != subMeta.size()-1){
                QFrame * hBorder1 = new QFrame(this);
                hBorder1->setLineWidth(2);
                hBorder1->setMidLineWidth(1);
                hBorder1->setFrameShape(QFrame::VLine);
                hBorder1->setFrameShadow(QFrame::Raised);
                subsystemSectionLayout->addWidget(hBorder1, fieldRowCount,fieldColCount);
            }
            fieldColCount++;
        }
    }

    QWidget * subsystemWidget = new QWidget;
    subsystemWidget->setLayout(subsystemSectionLayout);
    subsystemWidget->adjustSize();

    QScrollArea * subsystemArea = new QScrollArea;
    subsystemArea->setFixedWidth(screenWidth-30);
    subsystemArea->setFixedHeight(screenWidth/7);
    subsystemArea->setBackgroundRole(QPalette::Window);
    subsystemArea->setWidget(subsystemWidget);

    mainLayout->addWidget(subsystemArea);
    QFrame * hBorder1 = new QFrame(this);
    hBorder1->setLineWidth(2);
    hBorder1->setMidLineWidth(1);
    hBorder1->setFrameShape(QFrame::HLine);
    hBorder1->setFrameShadow(QFrame::Raised);
    mainLayout->addWidget(hBorder1, Qt::AlignCenter);

    QString  butLabelFont = QString::number(stringSize*1.5);
    QString  labelFont = QString::number(stringSize*2);
    QGridLayout * stateButtonLayout = new QGridLayout;

    QLabel * label = new QLabel;
    label->setText("STATES & STATUSES: ");
    label->setStyleSheet("font:"+labelFont+"pt;");
    label->setFixedWidth(label->width());
    stateButtonLayout->addWidget(label,0,0,Qt::AlignLeft);

    QHBoxLayout * btnsLayout = new QHBoxLayout;

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
    message = new QListWidget();
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

    vector<meta *> sensors = currentSubSystem->get_metadata();
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
    plot = new QCustomPlot();
    plot->addGraph();
    plot->setFixedHeight(unitHeight*6);
    plot->setFixedWidth(unitWidth*7);
    plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    plot->graph(0)->setLineStyle(QCPGraph::lsLine);
    plot->yAxis->setRange(0,0, Qt::AlignCenter);
    graphLayout->addWidget(plot, Qt::AlignCenter);

    plotLayout->addLayout(comboLayout, Qt::AlignCenter);
    plotLayout->addLayout(graphLayout, Qt::AlignCenter);

    plotLayout->setAlignment(Qt::AlignCenter);
    bottomLayout->addLayout(plotLayout,0,2,Qt::AlignCenter);
    mainLayout->addLayout(bottomLayout, Qt::AlignCenter);
    central->setMaximumWidth(screenWidth - 50);
}

void detailPage::setCurrentSystem(SubsystemThread * subsystem){
    currentSubSystem = subsystem;
    connect(currentSubSystem, SIGNAL(pushErrMsg(string)), this, SLOT(receiveErrMsg(string)));
    connect(currentSubSystem, SIGNAL(pushMessage(string)), this, SLOT(receiveMsg(string)));
    connect(currentSubSystem, SIGNAL(valueChanged()), this, SLOT(updateGraph()));
    connect(currentSubSystem, SIGNAL(updateDisplay(meta *)), this, SLOT(updateEdits(meta *)));
    connect(currentSubSystem, SIGNAL(updateEditColor(string, meta *)), this, SLOT(changeEditColor(string, meta *)));
    update();
}

void detailPage::sliderValChanged(int value){
    cout << "Current Value: " << value << endl;
    QObject* obj = sender();
    for (uint i = 0; i < controlSliders.size(); i++){
        if (obj == controlSliders.at(i)){
            emit sendControlValue(value, sliderCtrls.at(i));
        }
    }
}

void detailPage::ctrlButtonPressed(){
    QObject* obj = sender();
    for (uint i = 0; i < controlButtons.size(); i++){
        if (obj == controlButtons.at(i)){
            emit sendControlValue(buttonCtrls.at(i)->pressVal, buttonCtrls.at(i));
        }
    }
}

void detailPage::ctrlButtonReleased(){
    QObject* obj = sender();
    for (uint i = 0; i < controlButtons.size(); i++){
        if (obj == controlButtons.at(i)){
            emit sendControlValue(buttonCtrls.at(i)->releaseVal, buttonCtrls.at(i));
        }
    }
}

void detailPage::editUpdated(){
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

void detailPage::activateStateMW(system_state * nextState){

    for(uint i = 0; i < stateButtons.size(); i++){
        if (stateButtons.at(i)->text().toStdString().compare(nextState->name) == 0){
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
    edit->setMinimumWidth(unitWidth);
    edit->setStyleSheet("font:24pt;");
}

void detailPage::plotGraph(QString sensorName){
    vector<QString> timeData;
    vector<QString> valueData;
    xinit=0;
    gx.clear();
    gy.clear();

    meta * sensor;
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
            sensor = subMeta.at(i);
            graphMax = (sensor->maximum);
            graphMin = (sensor->minimum);
            for(uint i = 0; i < conf->subsystems.size(); i++){
                vector<meta*> subSensors = conf->subsystems.at(i)->get_mainMeta();
                for (uint j = 0; j < subSensors.size(); j++){
                    if (sensor->sensorName.compare(subSensors.at(j)->sensorName) == 0){
                        string tableName = conf->removeSpaces(conf->subsystems.at(i)->subsystemId) + "_caldata";
                        vector<string>cols;
                        cols.push_back("time");
                        cols.push_back("value");
                        vector<string>rows;
                        timeData = conf->dbase->getTargetColumn(QString::fromStdString(tableName),"time","sensorname",QString::fromStdString(sensor->sensorName));
                        valueData = conf->dbase->getTargetColumn(QString::fromStdString(tableName),"value","sensorname",QString::fromStdString(sensor->sensorName));
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

void detailPage::receiveMsg(string msg){
    addErrorMessage(QString::fromStdString(msg));
}

void detailPage::addPoint(double x, double y){
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

void detailPage::receiveErrMsg(string msg){
    addErrorMessage(QString::fromStdString(msg));
}

void detailPage::updateFSM_MW(statemachine * currFSM){
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

void detailPage::addErrorMessage(QString eMessage){
    message->addItem(eMessage);
    message->scrollToBottom();
}

void detailPage::updateVals(){
// addPoint(xinit,yinit);
//yinit++;
}

void detailPage::getCurrentSystem(int i){
    currentSystem=i;
}

void detailPage::closeTab(int tabId){
    if ((tabId != 0) && (tabId != 1)) tabs->removeTab(tabId);
    else if (tabId == 0) shutdownSystem();
    tabs->setCurrentIndex(0);
}

void detailPage:: closeDetailPage(){

}

/**
 * @brief MainWindow::passive_dialog creates a passive dialog box displaying
 *  the respective message. User response is not required
 * @param msg message to be displayed on the dialog box
 * @return 1 upon succesful run
 */
int detailPage::passive_dialog(string msg){
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
int detailPage::active_dialog(string msg){
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
string detailPage::info_dialog(string msg){
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

void detailPage::shutdownSystem(){
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
    QFile file("DataBase.txt");
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
void detailPage::updateEdits(meta * sensor){
    cout << "Check 1" << endl;
    vector<meta*> allSensors = currentSubSystem->get_metadata();
    cout << "size of all sensors" << allSensors.size() << endl;
    cout << "size of edits" << edits.size() << endl;
    fflush(stdout);
    for(uint i = 0; i < edits.size(); i++){
        cout << "Check 3" << endl;
        if(allSensors.at(i) == sensor){
            cout << "Check 4" << endl;
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
void detailPage::checkTimeout(){
    for(uint i = 0; i < edits.size(); i++){
        if (!editTimers.at(i)->isActive()) edits.at(i)->setText("--");
    }
}

void detailPage::changeEditColor(string color, meta * sensor){
    vector<meta*> allSensors = currentSubSystem->get_metadata();
    for(uint i = 0; i < allSensors.size(); i++){
        if(color.compare("red") == 0){
            if (allSensors.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FF0000");
            }
        } else if(color.compare("blue") == 0){
            if (allSensors.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #1E90FF");
            }
        } else if(color.compare("yellow") == 0){
            if (allSensors.at(i) == sensor) {
                edits.at(i)->setStyleSheet("color: #FFFF00");
            }
        }
    }
}

void detailPage::updateGraph(){
    cout << "Updating graph" << endl;
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
