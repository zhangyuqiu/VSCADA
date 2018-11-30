#include "postprocess.h"
#include "ui_postprocess.h"

postProcess::postProcess(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::postProcess)
{
    ui->setupUi(this);

    central = new QWidget();
    mainLayout = new QGridLayout();
    conf = new Config();
//    conf->read_config_file_data();
    displayTable = new QTableWidget();
    plot = new QCustomPlot();


    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);
    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    unitWidth=width/20;//100
    unitHeight=height/20;//56
    stringSize = unitWidth/10;//10
    maxSensorRow=0;
    raw=false;
    db = new DB_Engine;


    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);

    this->setCentralWidget(scrollArea);

    update();
    //loadTable();





          // Resister view item acticated
       // connect( list , SIGNAL(activated(const QModelIndex & )),this,SLOT(SlotListItemActivated(const QModelIndex & )));

}



postProcess::~postProcess()
{
    delete ui;
}

void postProcess::update(){

    startLabel = new QLabel();
    mainLayout->addWidget(startLabel,0,1,Qt::AlignLeft);



    dbBox = new QComboBox;
    dbBox->setEditable (true);
    dbBox->setFixedWidth(static_cast<int>(unitWidth*4));
    dbBox->setFixedHeight(static_cast<int>(unitHeight*1));
    QString  boxFont = QString::number(stringSize*1.4);
    dbBox->setStyleSheet("font:"+boxFont+"pt;");
    QFile file("../VSCADA/savedsessions/DataBase.txt");
    QTextStream stream(&file);
    QString line;

       if (file.open(QIODevice::ReadOnly))
       {
         QTextStream in(&file);
         QString line1 = in.readLine();
         while (!in.atEnd())
         {
            QString line = in.readLine();
           dbBox->addItem (line);
         }
         file.close();
       }
       mainLayout->addWidget(dbBox,0,0);



       // Create list view item model
       list = new QListView();
       list->setFixedHeight(unitHeight*6);
       list->setFixedWidth(unitWidth*4);
       list->setSelectionMode(QAbstractItemView::SingleSelection);
       QString  listFont = QString::number(stringSize*1.4);
       list->setStyleSheet("font:"+listFont+"pt;");

         poModel =new QStandardItemModel(list);

         getSensorList();

         list->setModel(poModel);

          mainLayout->addWidget(list,1,0);



//          dataTable = new QTableView();
//          dataTable->setFixedHeight(unitHeight*6);
//          dataTable->setFixedWidth(unitWidth*7);
//          mainLayout->addWidget(dataTable,0,2);

//          message = new QListWidget();
//          message->setFixedHeight(unitHeight*6);
//          message->setFixedWidth(unitWidth*7);
//          mainLayout->addWidget(message,2,0);




//       exitButton =new QPushButton();
//       exitButton->setText("EXIT");
//       QPalette palexit = exitButton->palette();
//       palexit.setColor(QPalette::Button, QColor(0,0,255));
//       exitButton->setPalette(palexit);
//       exitButton->setAutoFillBackground(true);
////       exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
//       exitButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
//       exitButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
//       mainLayout->addWidget(exitButton,0,2,Qt::AlignCenter);

//       loadButton =new QPushButton();
//       loadButton->setText("Load");
//       QPalette palload = loadButton->palette();
//       palload.setColor(QPalette::Button, QColor(0,0,255));
//       loadButton->setPalette(palload);
//       loadButton->setAutoFillBackground(true);
////       exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
//       loadButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
//       loadButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
//       mainLayout->addWidget(loadButton,0,1,Qt::AlignCenter);

//       QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));
//       QObject::connect(loadButton, SIGNAL (clicked()), this , SLOT(loadTable()));


       connect(poModel, SIGNAL(itemChanged(QStandardItem*)),this,SLOT(addSensor(QStandardItem*)));
       connect(dbBox, SIGNAL(currentIndexChanged(int)),this,SLOT(reload(int)));

}

void postProcess::addSensor(QStandardItem *poItem)
{

    QString text=poItem->text();
    if(std::find(selected.begin(), selected.end(), text) != selected.end())
    {
     selected.erase(std::remove(selected.begin(), selected.end(), text), selected.end());
    } else{
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




    plot = new QCustomPlot();

    plot->setFixedHeight(unitHeight*8);
    plot->setFixedWidth(unitWidth*14);

    plot->yAxis->setRange(-20, -(40), Qt::AlignRight);
     plot->xAxis->setRange(-20, -(40), Qt::AlignRight);
    mainLayout->addWidget(plot,2,0,2,2);

    vector<QString> tempstart =db->getTargetColumn("system_info","starttime"," "," ");
    vector<QString> tempend=db->getTargetColumn("system_info","endtime"," "," ");

    if(tempstart.size()==0){
        startTime=" ";
        endTime=" ";
    }else{
        startTime=tempstart.at(0);
        endTime =tempend.at(0);
    }




    QString  labelFont = QString::number(stringSize*1.5);
    //startLabel = new QLabel();
    startLabel->setText("Start Time at: "+startTime+";        End Time at: "+endTime+";");
    startLabel->setStyleSheet("font:"+labelFont+"pt;");
    startLabel->setFixedWidth(unitWidth*10);
    //mainLayout->addWidget(startLabel,0,1,Qt::AlignLeft);


//    QSqlQueryModel *modal=new QSqlQueryModel();
//    mydb=QSqlDatabase::addDatabase("QSQLITE");
//    mydb.setDatabaseName(currentBase);//path of data base
//    mydb.open();

//    //QString currentTable ="GLV_caldata";

//    QString selectName = "SELECT * FROM system_info";
//    //QString selectName = "SELECT * FROM sensors";
//    QSqlQuery* qry =new QSqlQuery(mydb);
//    qry->prepare(selectName);
//    qry->exec();
//    modal->setQuery(*qry);
//    dataTable->setModel(modal);



//    selectName="SELECT value FROM TSI_rawdata WHERE sensorname = 'TSITemp'";
//    qry->prepare(selectName);
//    qry->exec();
//    while(qry->next()){
//          QString num =qry->value(0).toString();
//          message->addItem(num);

//        }



    int rowNum=2;
    int columnNum=0;

    for(int i=0; i<selected.size();i++){
        columnNum=columnNum+2;
        name=selected.at(i);
        for(int k=0; k<sensorname.size();k++){
            QString check=sensorname.at(k);
            QString rawcheck=check+"_raw";
            if(0== name.compare(check)){
               currentTable=subsystem.at(k)+"_caldata";

            }else if(0== name.compare(rawcheck)){
                name=check;
              currentTable=subsystem.at(k)+"_rawdata";

            }
        }
        vector<QString>  data=db->getTargetColumn(currentTable,"value","sensorname",name);
        for(int j=0; j<data.size();j++){
            if((j+3)>rowNum){
        rowNum++;
            }
        }

    }
//        name=selected.at(i);
//    string thisBase = currentBase.toStdString();
//   string thisTable = currentTable.toStdString();
//    string thisName = name.toStdString();
//    db->setFile(thisBase);
//    vector<QString>  data=db->getTargetColumn(currentTable,"value","sensorname",name);
//    message->addItem(name);
//    for(int i=0; i<data.size();i++){
//                QString display=data.at(i);
//                message->addItem(display);

//    }
//    }

    //table



//   int columnNum =2*(selectName.size());

 displayTable = new QTableWidget();
 displayTable->setRowCount(rowNum);
  displayTable->setColumnCount(columnNum);
  QString  tableFont = QString::number(stringSize*1.4);
  displayTable->setStyleSheet("font:"+tableFont+"pt;");
  displayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//   QTableWidgetItem* sl = new QTableWidgetItem(selected.at(0));
//   displayTable->setItem(0, 0, sl);
//   displayTable->setSpan(0, 0, 2, 1);
  int row=0;
  int column=0;


 for(int i=0; i<selected.size();i++){

     name=selected.at(i);
     for(int k=0; k<sensorname.size();k++){
         QString check=sensorname.at(k);
         QString rawcheck=check+"_raw";
         if(0== name.compare(check)){
            currentTable=subsystem.at(k)+"_caldata";

         }else if(0== name.compare(rawcheck)){
             name=check;
           currentTable=subsystem.at(k)+"_rawdata";

         }
     }


 vector<QString>  data=db->getTargetColumn(currentTable,"value","sensorname",name);
 vector<QString>  time=db->getTargetColumn(currentTable,"time","sensorname",name);





 QTableWidgetItem* unit = new QTableWidgetItem(selected.at(i));
 displayTable->setItem(row,column,unit);
 displayTable->setSpan(row,column,1,2);
 row++;

 QTableWidgetItem* timeDisplay = new QTableWidgetItem("Time");
 displayTable->setItem(row,column,timeDisplay);
 displayTable->setColumnWidth(column,unitWidth*1.5);

 QTableWidgetItem* valueDisplay = new QTableWidgetItem("Value");
 displayTable->setItem(row,column+1,valueDisplay);
 displayTable->setColumnWidth(column+1,unitWidth*1.5);

 QVector<double> gx;
 QVector<double> gy;


for(int j=0; j<data.size();j++){
    row++;
    QTableWidgetItem* timeStamp = new QTableWidgetItem(time.at(j));

    displayTable->setItem(row,column,timeStamp);

    QTableWidgetItem* item = new QTableWidgetItem(data.at(j));
    displayTable->setItem(row,column+1,item);

    double y= data.at(j).QString::toDouble();
    double x =time.at(j).QString::toDouble();

    if(y>ymax){
        ymax=y;
    }

    if(x>xmax){
        xmax=x;
    }

    if(y<ymin){
        ymin=y;
    }

    if(x<xmin){
        xmin=x;
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
// plot->legend->setFont(legendFont);
//plot->legend->setSelectedFont(legendFont);
//plot->legend->setSelectableParts(QCPLegend::spItems);

 plot->replot();
 plot->update();

 displayTable->setFixedHeight(unitHeight*8);
 displayTable->setFixedWidth(unitWidth*10);
 mainLayout->addWidget(displayTable,1,1);




}

void postProcess::getSensorList(){

     currentBase = dbBox->currentText();
     string thisBase = currentBase.toStdString();
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



//    int itemCount=0;

//    vector<SubsystemThread *> subs;
//    subs = conf->subsystems;
//    for (uint i = 0; i < subs.size(); i++){
//        vector<meta*> subMeta = subs.at(i)->get_metadata();
//        if (static_cast<int>(subMeta.size()) > maxSensorRow) maxSensorRow = static_cast<int>(subMeta.size());
//    }

//     systemName = new QString *[subs.size()];

//     for(int i=0;i<subs.size();i++){
//         systemName[i]= new QString [maxSensorRow];
//     }

//      for(int i=0;i<subs.size();i++){
//     for(int j=0;j<maxSensorRow;j++){

//           systemName[i][j]="0";
//         }
//     }

//    for (uint i = 0; i < subs.size(); i++){
//        SubsystemThread * currSub = subs.at(i);
//        vector<meta *> subMeta = currSub->get_metadata();

//        if(subMeta.size() > 0){
//            for (uint j = 0; j < subMeta.size(); j++){
//                if (j == 0){
//                    QString name = QString::fromStdString(currSub->subsystemId);
//                    tableName.push_back(name);
//                }
//                QString sensor = QString::fromStdString(subMeta.at(j)->sensorName);

//                QStandardItem * poListItem =new QStandardItem;

//                poListItem->setCheckable( true );

//                poListItem->setText(sensor);


//                poModel->insertRow(itemCount, poListItem);

//                itemCount++;

//                systemName[i][j]=sensor;
//            }

//        }
//    }

selected.clear();

loadTable();
}

void postProcess::reload(int no){
//    message->addItem("check");
//    plot->clearGraphs();
//    displayTable->clear();
//    loadTable();
    getSensorList();
}


