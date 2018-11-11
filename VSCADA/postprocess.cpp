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
          mainLayout->addWidget(dataTable,1,1);

          message = new QListWidget();
          message->setFixedHeight(unitHeight*6);
          message->setFixedWidth(unitWidth*7);
          mainLayout->addWidget(message,1,2);

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
    label = new QLabel;
    label->setText(text);
    mainLayout->addWidget(label,0,1);

}

void postProcess::loadTable(){
    currentBase = dbBox->currentText();

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

    selectName = "SELECT value FROM "+currentTable;
    qry->prepare(selectName);
    qry->exec();
    while(qry->next()){
          QString num =qry->value(0).toString();
          message->addItem(num);
        }

    mydb.close();

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


