#include "dashboard.h"
#include "ui_dashboard.h"

dashboard::dashboard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::dashboard)
{
    ui->setupUi(this);


    central = new QWidget();
    mainLayout = new QGridLayout();

    speedNum=40;
    batteryNum=80;
    timeNum=40;

    QRect rec = QApplication::desktop()->screenGeometry();
    int height=rec.height();
    int width=rec.width();

    int unitWidth;
    int unitHeight;
    int stringSize;
    QString fontSize;

    unitWidth=width/17;//100
    unitHeight=height/17;//56

    stringSize = unitWidth/10;//10

    fontSize = QString::number(stringSize);

    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    central->setLayout(mainLayout);

    QScrollArea *scrollArea = new QScrollArea();



    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(central);


    this->setCentralWidget(scrollArea);

    battery = new QProgressBar();
    battery->setTextVisible(true);
    battery->setFixedHeight(14*unitHeight);
    battery->setFixedWidth(3*unitWidth);
    battery->setMinimum(0);
    battery->setMaximum(100);
    battery->setOrientation(Qt::Vertical);
    battery->setValue(batteryNum);
    battery->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(battery,2,1);

    QLabel * batLabel = new QLabel();
    batLabel->setText("Battery");
    batLabel->setFixedHeight(2*unitHeight);
    batLabel->setFixedWidth(3*unitWidth);
    batLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(batLabel,0,1);

    QLabel * speedLabel = new QLabel();
    speedLabel->setText("Velocity");
    speedLabel->setFixedHeight(2*unitHeight);
    speedLabel->setFixedWidth(7*unitWidth);
    speedLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(speedLabel,0,0);

    QLabel * timeLabel = new QLabel();
    timeLabel->setText("Time");
    speedLabel->setFixedHeight(2*unitHeight);
    speedLabel->setFixedWidth(3*unitWidth);
    timeLabel->setStyleSheet("font:36pt;");
    mainLayout->addWidget(timeLabel,0,2);

    QLCDNumber * battery = new QLCDNumber();
    battery->setMinimumHeight(2*unitHeight);
    battery->setMinimumWidth(3*unitWidth);
    battery->display(batteryNum);
    mainLayout->addWidget(battery,1,1);

    QLCDNumber * time = new QLCDNumber();
    time->setFixedHeight(2*unitHeight);
    time->setFixedWidth(3*unitWidth);
    time->display(timeNum);
    mainLayout->addWidget(time,1,2);



    QLCDNumber * speed = new QLCDNumber();
    speed->setFixedHeight(2*unitHeight);
    speed->setFixedWidth(7*unitWidth);
    speed->display(speedNum);
    mainLayout->addWidget(speed,1,0);

//gauge
        mSpeedGauge = new QcGaugeWidget;
        mSpeedGauge->addBackground(99);
        mSpeedGauge->setFixedHeight(14*unitHeight);
        mSpeedGauge->setFixedWidth(7*unitWidth);
        QcBackgroundItem *bkg1 = mSpeedGauge->addBackground(92);
        bkg1->clearrColors();
        bkg1->addColor(0.1,Qt::black);
        bkg1->addColor(1.0,Qt::white);

        QcBackgroundItem *bkg2 = mSpeedGauge->addBackground(88);
        bkg2->clearrColors();
        bkg2->addColor(0.1,Qt::gray);
        bkg2->addColor(1.0,Qt::darkGray);

        mSpeedGauge->addArc(55);
        mSpeedGauge->addDegrees(65)->setValueRange(0,80);
        mSpeedGauge->addColorBand(50);

        mSpeedGauge->addValues(80)->setValueRange(0,80);

        mSpeedGauge->addLabel(70)->setText("Km/h");
        QcLabelItem *lab = mSpeedGauge->addLabel(40);
        lab->setText("0");
        mSpeedNeedle = mSpeedGauge->addNeedle(60);
        mSpeedNeedle->setLabel(lab);
        mSpeedNeedle->setColor(Qt::white);
        mSpeedNeedle->setValueRange(0,80);
        mSpeedNeedle->setCurrentValue(speedNum);
        mSpeedGauge->addBackground(7);
        mSpeedGauge->addGlass(88);

       mainLayout->addWidget(mSpeedGauge,2,0);

       stateButton =new QPushButton();
       stateButton->setText("state");
       QPalette palstate = stateButton->palette();
       palstate.setColor(QPalette::Button, QColor(0,255,0));
       stateButton->setPalette(palstate);
       stateButton->setAutoFillBackground(true);
       QString  butLabelFont = QString::number(stringSize*5);
       stateButton->setStyleSheet("font:"+butLabelFont+"pt;");
       stateButton->setFixedWidth(static_cast<int>(unitWidth*3));
       stateButton->setFixedHeight(static_cast<int>(unitHeight*14));
       mainLayout->addWidget(stateButton,2,2,Qt::AlignCenter);

       exitButton =new QPushButton();
       exitButton->setText("EXIT");
       QPalette palexit = exitButton->palette();
       palexit.setColor(QPalette::Button, QColor(0,0,255));
       exitButton->setPalette(palexit);
       exitButton->setAutoFillBackground(true);
       //exitButton->setStyleSheet("font:"+butLabelFont+"pt;");
       exitButton->setFixedWidth(static_cast<int>(unitWidth*1.2));
       exitButton->setFixedHeight(static_cast<int>(unitHeight*1.8));
       mainLayout->addWidget(exitButton,0,3,Qt::AlignCenter);

       QObject::connect(exitButton, SIGNAL (clicked()), this , SLOT(close()));






}

dashboard::~dashboard()
{
    delete ui;
}
