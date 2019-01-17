#include "postprocess.h"
#include "ui_postprocess.h"

postProcess::postProcess(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::postProcess)
{
    ui->setupUi(this);

    central = new QWidget();
    mainLayout = new QVBoxLayout();
    displayTable = new QTableWidget();
    plot = new QCustomPlot();
    db = new DB_Engine();

    central->setLayout(mainLayout);

    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();
    unitWidth=width/20;
    unitHeight=height/20;
    stringSize = unitWidth/10;

    central->setFixedHeight(static_cast<int>(height*0.9));
    central->setFixedWidth(static_cast<int>(width*0.98));

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);

    this->setCentralWidget(scrollArea);
    update();
}

postProcess::~postProcess()
{
    delete central;
    delete mainLayout;
    delete displayTable;
    delete plot;
    delete ui;
}

void postProcess::update(){
    QHBoxLayout * headerLayout = new QHBoxLayout;
    QHBoxLayout * bodyLayout = new QHBoxLayout;
    QHBoxLayout * graphLayout = new QHBoxLayout;

    QString  boxFont = QString::number(stringSize*1.4);
    dbBox = new QComboBox;
    dbBox->setEditable (true);
    dbBox->setFixedWidth(static_cast<int>(unitWidth*4));
    dbBox->setFixedHeight(static_cast<int>(unitHeight*1));
    dbBox->setStyleSheet("font:"+boxFont+"pt;");

    system("cd savedsessions/ && ls *.db > DataBase.txt && cd ../");

    // read available databases
    QFile file("../VSCADA/savedsessions/DataBase.txt");
    QTextStream stream(&file);
    QString line;
    if (file.open(QIODevice::ReadOnly)){
        QTextStream in(&file);
        QString line1 = in.readLine();
        dbBox->addItem("--select--");
        while (!in.atEnd()){
            QString line = in.readLine();
            dbBox->addItem(line);
        }
        file.close();
    }
    headerLayout->addWidget(dbBox,Qt::AlignCenter);

    startLabel = new QLabel();
    headerLayout->addWidget(startLabel,Qt::AlignCenter);

    // Create list view item model
    QString  listFont = QString::number(stringSize*1.4);
    list = new QListView();
    list->setFixedHeight(unitHeight*7.5);
    list->setFixedWidth(unitWidth*4);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setStyleSheet("font:"+listFont+"pt;");

    poModel =new QStandardItemModel(list);
    list->setModel(poModel);
    bodyLayout->addWidget(list,Qt::AlignCenter);

    exportButton =new QPushButton();
    exportButton->setText("Export");
    QPalette palExport = exportButton->palette();
    palExport.setColor(QPalette::Button, QColor(0,0,255));
    exportButton->setPalette(palExport);
    exportButton->setAutoFillBackground(true);
    exportButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
    exportButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
    headerLayout->addWidget(exportButton,Qt::AlignCenter);
    QObject::connect(exportButton, SIGNAL (clicked()), this , SLOT(exportCSV()));

    QString  tableFont = QString::number(stringSize*1.4);
    displayTable = new QTableWidget();
    displayTable->setFixedHeight(unitHeight*7.5);
    displayTable->setFixedWidth(unitWidth*10);
    displayTable->setStyleSheet("font:"+tableFont+"pt;");
    displayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    bodyLayout->addWidget(displayTable,Qt::AlignCenter);

    plot = new QCustomPlot();
    plot->setFixedHeight(unitHeight*8);
    plot->setFixedWidth(unitWidth*14);
    plot->yAxis->setRange(-20, -(40), Qt::AlignRight);
    plot->xAxis->setRange(-20, -(40), Qt::AlignRight);
    graphLayout->addWidget(plot,Qt::AlignCenter);

    mainLayout->addLayout(headerLayout);
    mainLayout->addLayout(bodyLayout);
    mainLayout->addLayout(graphLayout);

    connect(poModel, SIGNAL(itemChanged(QStandardItem*)),this,SLOT(addSensor(QStandardItem*)));
    connect(dbBox, SIGNAL(currentIndexChanged(int)),this,SLOT(reload(int)));
}

void postProcess::addSensor(QStandardItem * poItem){
    QString text = poItem->text();
    if(std::find(selected.begin(), selected.end(), text) != selected.end()){
        selected.erase(std::remove(selected.begin(), selected.end(), text), selected.end());
    } else  {
        selected.push_back(text);
    }
    loadTable();
}

void postProcess::loadTable(){
    int xmax=0;
    int ymax=0;
    int xmin=0;
    int ymin=0;
    QString name;
    QString subname;
    QString currentTable;
    currentBase = dbBox->currentText();
    string thisBase = currentBase.toStdString();
    db->setFile(thisBase);

    vector<QString> tempstart =db->getTargetColumn("system_info","starttime"," "," ");
    vector<QString> tempend=db->getTargetColumn("system_info","endtime"," "," ");

    if(tempstart.size()==0){
        startTime=" ";
        endTime=" ";
    } else {
        startTime=tempstart.at(0);
        endTime =tempend.at(0);
    }

    QString labelFont = QString::number(stringSize*2);
    startLabel->setText("Start Time: "+startTime+"\tEnd Time: "+endTime);
    startLabel->setStyleSheet("font:"+labelFont+"pt;");
    startLabel->setFixedWidth(unitWidth*10);

    int rowNum = 2;
    int columnNum = 0;

    for(uint i = 0; i < selected.size(); i++){
        columnNum = columnNum+2;
        name = selected.at(i);
        for(uint k = 0; k < sensorname.size(); k++){
            QString check = sensorname.at(k);
            QString rawcheck = check+"_raw";
            if(name.compare(check) == 0){
                currentTable=subsystem.at(k)+"_caldata";
            } else if (name.compare(rawcheck) == 0){
                name=check;
                currentTable=subsystem.at(k)+"_rawdata";
            }
        }

        vector<QString> data = db->getTargetColumn(currentTable,"value","sensorname",name);
        for(uint j = 0; j < data.size(); j++){
            if((j+3) > static_cast<uint>(rowNum)) rowNum++;
        }
    }

    displayTable->setRowCount(rowNum);
    displayTable->setColumnCount(columnNum);

    int row=0;
    int column=0;
    plot->clearGraphs();
    for(uint i = 0; i < selected.size(); i++){
        name = selected.at(i);
        for(uint k = 0; k < sensorname.size(); k++){
            QString check = sensorname.at(k);
            QString rawcheck = check + "_raw";
            if (name.compare(check) == 0){
                currentTable = subsystem.at(k) + "_caldata";
            } else if (name.compare(rawcheck) == 0){
                name = check;
                currentTable = subsystem.at(k) + "_rawdata";
            }
        }
        vector<QString> data = db->getTargetColumn(currentTable,"value","sensorname",name);
        vector<QString> time = db->getTargetColumn(currentTable,"time","sensorname",name);
        QTableWidgetItem * unit = new QTableWidgetItem(selected.at(i));
        displayTable->setItem(row,column,unit);
        displayTable->setSpan(row,column,1,2);
        row++;

        QTableWidgetItem* timeDisplay = new QTableWidgetItem("Time");
        displayTable->setItem(row,column,timeDisplay);
        displayTable->setColumnWidth(column,static_cast<int>(unitWidth*1.5));

        QTableWidgetItem* valueDisplay = new QTableWidgetItem("Value");
        displayTable->setItem(row,column+1,valueDisplay);
        displayTable->setColumnWidth(column+1,static_cast<int>(unitWidth*1.5));

        QVector<double> gx;
        QVector<double> gy;

        for(uint j = 0; j < data.size(); j++){
            row++;
            QTableWidgetItem* timeStamp = new QTableWidgetItem(time.at(j));
            displayTable->setItem(row,column,timeStamp);
            QTableWidgetItem* item = new QTableWidgetItem(data.at(j));
            displayTable->setItem(row,column+1,item);

            double y= data.at(j).QString::toDouble();
            double x =time.at(j).QString::toDouble();

            if (y > ymax){
                ymax = static_cast<int>(ceil(y));
            }
            if(x > xmax){
                xmax = static_cast<int>(ceil(x));
            }
            if(y < ymin){
                ymin = static_cast<int>(ceil(y));
            }
            if(x < xmin){
                xmin = static_cast<int>(ceil(x));
            }
            gx.append(x);
            gy.append(y);
        }

        column=column+2;
        row=0;
        plot->addGraph();
        int a=rand() % 253 + 1;
        int b=rand() % 253 + 1;
        int c=rand() % 253 + 1;

        plot->graph(i)->setPen(QPen(QColor(a,b,c)));
        plot->graph(i)->setName(selected.at(i));
        plot->graph(i)->setScatterStyle(QCPScatterStyle::ssCircle);
        plot->graph(i)->setLineStyle(QCPGraph::lsLine);
        plot->yAxis->setRange(ymin-4, -(ymax-ymin+4), Qt::AlignRight);
        plot->xAxis->setRange(0, -(xmax+4), Qt::AlignRight);
        plot->graph(i)->setData(gx,gy);
    }

    plot->legend->setVisible(true);
    plot->replot();
    plot->update();
}

void postProcess::getSensorList(){
    currentBase = dbBox->currentText();
    string thisBase = currentBase.toStdString();
//    if (thisBase.compare("--select--") == 0) return;
    db->setFile(thisBase);

    poModel->removeRows( 0, poModel->rowCount() );
    sensorname=db->getTargetColumn("sensors","sensorname"," "," ");
    subsystem=db->getTargetColumn("sensors","subsystem"," "," ");
    int itemCount=0;

    for (uint i = 0; i < sensorname.size(); i++){
        QString sensor = sensorname.at(i);
        QString sensorraw = sensor +"_raw";
        QStandardItem * poListItem =new QStandardItem;
        QStandardItem * poListItemRaw =new QStandardItem;

        poListItem->setCheckable( true );
        poListItemRaw->setCheckable( true );
        poListItem->setText(sensor);
        poListItemRaw->setText(sensorraw);

        poModel->insertRow(itemCount, poListItem);
        itemCount++;
        poModel->insertRow(itemCount, poListItemRaw);
        itemCount++;
    }

    selected.clear();
    loadTable();
}

void postProcess::reload(int no){
    getSensorList();
}

void postProcess::exportCSV(){
    currentBase = dbBox->currentText();
    string thisBase = currentBase.toStdString();
    QString newName;
    QString numName;
    QString numTable;
    int max=0;

    for(uint i = 0; i < selected.size(); i++){
        columnNum = columnNum + 2;
        numName=selected.at(i);
        for(uint k = 0; k < sensorname.size(); k++){
            QString check = sensorname.at(k);
            QString rawcheck = check + "_raw";
            if(numName.compare(check) == 0){
                numTable = subsystem.at(k) + "_caldata";
            } else if (numName.compare(rawcheck) == 0){
                numName = check;
                numTable=subsystem.at(k)+"_rawdata";
            }
        }
        vector<QString> data = db->getTargetColumn(numTable,"value","sensorname",numName);
        for(uint j = 0; j < data.size(); j++){
            if((j+1) > static_cast<uint>(max)) max++;
        }
    }

    newName = QFileDialog::getSaveFileName(this, tr("Save File"), "/home");

    db->setFile(thisBase);

    newName = newName + ".csv";
    QFile file(newName);
    if ( file.open(QIODevice::WriteOnly) ){
        QTextStream stream( &file );
        stream << "StartTime" << "," << startTime  << endl << "EndTime" << "," << endTime  << endl;

        QString sensorNames;
        for(uint x = 0; x < selected.size(); x++){
            sensorNames = sensorNames + "Time,";
            sensorNames = sensorNames + selected.at(x) + ",";
        }

        stream << sensorNames << endl;

        for(int n = 0; n < max; n++){
            QString name;
            QString currentTable;
            QString output;
            for(uint i = 0; i < selected.size(); i++){
                name = selected.at(i);
                for(uint k = 0; k < sensorname.size(); k++){
                    QString check = sensorname.at(k);
                    QString rawcheck = check + "_raw";
                    if(name.compare(check) == 0){
                        currentTable = subsystem.at(k) + "_caldata";
                    } else if (name.compare(rawcheck) == 0){
                        name = check;
                        currentTable = subsystem.at(k) + "_rawdata";
                    }
                }
                vector<QString> data = db->getTargetColumn(currentTable,"value","sensorname",name);
                vector<QString> time = db->getTargetColumn(currentTable,"time","sensorname",name);

                int thisSize = static_cast<int>(time.size()) - 1;
                if(n > thisSize){
                    output = output + " , ,";
                } else {
                    output = output + time.at(static_cast<uint>(n)) + "," + data.at(static_cast<uint>(n)) + ",";
                }
            }
            stream << output << endl;
        }
    }
    file.close();
    string changePerm = "chmod 777 " + newName.toStdString();
    system(changePerm.c_str());
}


