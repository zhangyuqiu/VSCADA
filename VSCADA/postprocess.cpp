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
    conf->read_config_file_data();


    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);
    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    unitWidth=width/20;//100
    unitHeight=height/20;//56
    maxSensorRow=0;
    raw=false;
    db = new DB_Engine;


    QScrollArea *scrollArea = new QScrollArea();

    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);

    this->setCentralWidget(scrollArea);

    update();






          // Resister view item acticated
       // connect( list , SIGNAL(activated(const QModelIndex & )),this,SLOT(SlotListItemActivated(const QModelIndex & )));

}



postProcess::~postProcess()
{
    delete ui;
}

void postProcess::update(){

    list = new QListView();
    list->setFixedHeight(unitHeight*6);
    list->setFixedWidth(unitWidth*7);
    list->setSelectionMode(QAbstractItemView::SingleSelection);



       // Create list view item model
     poModel =new QStandardItemModel(list);

         getSensorList();

         list->setModel(poModel);

          mainLayout->addWidget(list,1,0);

          dataTable = new QTableView();
          dataTable->setFixedHeight(unitHeight*6);
          dataTable->setFixedWidth(unitWidth*7);
          mainLayout->addWidget(dataTable,2,1);

          message = new QListWidget();
          message->setFixedHeight(unitHeight*6);
          message->setFixedWidth(unitWidth*7);
          mainLayout->addWidget(message,2,0);


          dbBox = new QComboBox;
          dbBox->setEditable (true);
          dbBox->setFixedWidth(static_cast<int>(unitWidth*1.5));
          dbBox->setFixedHeight(static_cast<int>(unitHeight*1.4));
          QFile file("DataBase.txt");
          QTextStream stream(&file);
          QString line;

             if (file.open(QIODevice::ReadOnly))
             {
               QTextStream in(&file);
               while (!in.atEnd())
               {
                  QString line = in.readLine();
                 dbBox->addItem (line);
               }
               file.close();
             }
             mainLayout->addWidget(dbBox,0,0,Qt::AlignCenter);

       exitButton =new QPushButton();
       exitButton->setText("EXIT");
       QPalette palexit = exitButton->palette();
       palexit.setColor(QPalette::Button, QColor(0,0,255));
       exitButton->setPalette(palexit);
       exitButton->setAutoFillBackground(true);
//       exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
       exitButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
       exitButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
       mainLayout->addWidget(exitButton,0,2,Qt::AlignCenter);

       loadButton =new QPushButton();
       loadButton->setText("Load");
       QPalette palload = loadButton->palette();
       palload.setColor(QPalette::Button, QColor(0,0,255));
       loadButton->setPalette(palload);
       loadButton->setAutoFillBackground(true);
//       exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
       loadButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
       loadButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
       mainLayout->addWidget(loadButton,0,1,Qt::AlignCenter);

       QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));
       QObject::connect(loadButton, SIGNAL (clicked()), this , SLOT(loadTable()));





       connect(poModel, SIGNAL(itemChanged(QStandardItem*)),this,SLOT(addSensor(QStandardItem*)));
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

}

void postProcess::loadTable(){
    currentBase = dbBox->currentText();

    plot = new QCustomPlot();

    plot->setFixedHeight(unitHeight*6);
    plot->setFixedWidth(unitWidth*7);

    plot->yAxis->setRange(-20, -(40), Qt::AlignRight);
     plot->xAxis->setRange(-20, -(40), Qt::AlignRight);
    mainLayout->addWidget(plot,3,0);


    QSqlQueryModel *modal=new QSqlQueryModel();
    mydb=QSqlDatabase::addDatabase("QSQLITE");
    mydb.setDatabaseName(currentBase);//path of data base
    mydb.open();

    QString currentTable ="GLV_caldata";
    QString selectName = "SELECT * FROM "+currentTable;
    QSqlQuery* qry =new QSqlQuery(mydb);
    qry->prepare(selectName);
    qry->exec();
    modal->setQuery(*qry);
    dataTable->setModel(modal);

    QString name;
    string thisBase = currentBase.toStdString();
    db->setFile(thisBase);

    int rowNum=2;
    int columnNum=0;

    for(int i=0; i<selected.size();i++){
        columnNum=columnNum+2;
        name=selected.at(i);
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
  displayTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//   QTableWidgetItem* sl = new QTableWidgetItem(selected.at(0));
//   displayTable->setItem(0, 0, sl);
//   displayTable->setSpan(0, 0, 2, 1);
  int row=0;
  int column=0;


 for(int i=0; i<selected.size();i++){

     name=selected.at(i);

 vector<QString>  data=db->getTargetColumn(currentTable,"value","sensorname",name);
 vector<QString>  time=db->getTargetColumn(currentTable,"time","sensorname",name);



 QTableWidgetItem* unit = new QTableWidgetItem(selected.at(i));
 displayTable->setItem(row,column,unit);
 displayTable->setSpan(row,column,1,2);
 row++;

 QTableWidgetItem* timeDisplay = new QTableWidgetItem("Time");
 displayTable->setItem(row,column,timeDisplay);

 QTableWidgetItem* valueDisplay = new QTableWidgetItem("Value");
 displayTable->setItem(row,column+1,valueDisplay);

 QVector<double> gx;
 QVector<double> gy;


for(int j=0; j<data.size();j++){
    row++;
    QTableWidgetItem* timeStamp = new QTableWidgetItem(time.at(j));
    displayTable->setItem(row,column,timeStamp);

    QTableWidgetItem* item = new QTableWidgetItem(data.at(j));
    displayTable->setItem(row,column+1,item);

    double y= data.at(j).QString::toDouble();

    gx.append(j);
    gy.append(y);



 }

  column=column+2;
  row=0;
  plot->addGraph();
  int a=rand() % 253 + 1;
  int b=rand() % 253 + 1;
  int c=rand() % 253 + 1;

  plot->graph(i)->setPen(QPen(QColor(a,b,c)));
 plot->graph(i)->setScatterStyle(QCPScatterStyle::ssCircle);
 plot->graph(i)->setLineStyle(QCPGraph::lsLine);
  plot->graph(i)->setData(gx,gy);

}
 plot->legend->setVisible(true);
// plot->legend->setFont(legendFont);
//plot->legend->setSelectedFont(legendFont);
//plot->legend->setSelectableParts(QCPLegend::spItems);
 plot->replot();
 plot->update();

 displayTable->setFixedHeight(unitHeight*6);
 displayTable->setFixedWidth(unitWidth*7);
 mainLayout->addWidget(displayTable,1,1);




}

void postProcess::getSensorList(){
    int itemCount=0;

    vector<SubsystemThread *> subs;
    subs = conf->subsystems;
    for (uint i = 0; i < subs.size(); i++){
        vector<meta*> subMeta = subs.at(i)->get_metadata();
        if (static_cast<int>(subMeta.size()) > maxSensorRow) maxSensorRow = static_cast<int>(subMeta.size());
    }

     systemName = new QString *[subs.size()];

     for(int i=0;i<subs.size();i++){
         systemName[i]= new QString [maxSensorRow];
     }

      for(int i=0;i<subs.size();i++){
     for(int j=0;j<maxSensorRow;j++){

           systemName[i][j]="0";
         }
     }

    for (uint i = 0; i < subs.size(); i++){
        SubsystemThread * currSub = subs.at(i);
        vector<meta *> subMeta = currSub->get_metadata();

        if(subMeta.size() > 0){
            for (uint j = 0; j < subMeta.size(); j++){
                if (j == 0){
                    QString name = QString::fromStdString(currSub->subsystemId);
                    tableName.push_back(name);
                }
                QString sensor = QString::fromStdString(subMeta.at(j)->sensorName);

                QStandardItem * poListItem =new QStandardItem;

                poListItem->setCheckable( true );

                poListItem->setText(sensor);


                poModel->insertRow(itemCount, poListItem);

                itemCount++;

                systemName[i][j]=sensor;
            }

        }
    }

}


