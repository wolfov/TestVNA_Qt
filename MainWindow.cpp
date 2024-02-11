#include "MainWindow.h"
#include "ui_mainwindow.h"

QVector<measParam> AppSettings::measParams = QVector<measParam>
{measParam("S11","S11",-1),measParam("S12","S12",-1),measParam("S22","S22",-1),measParam("S21","S21",-1),measParam("A(1)","A",1),measParam("B(1)","B",1)};
QVector<measFormat> AppSettings::measFormats = QVector<measFormat>{measFormat("Ampl log","MLOG"),measFormat("Phase>180","UPHase")};
QVector<int> AppSettings::rowParams = QVector<int>{0,3,1,2,0,3,1,2};
QVector<GraphParam> AppSettings::Graphs = QVector<GraphParam>{GraphParam()};
QVector<QColor> GraphParam::Colors = QVector<QColor>{QColor("red"),QColor("blue"),QColor("green")};

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow)
{
    Locale = this->locale();
    ui->setupUi(this);

    _thread = new QThread(this);
    _handler = new Handler();
    _handler->moveToThread(_thread);

    qInfo() << "UI _threadID: " << QThread::currentThreadId();
    _thread->start();

    SetDevice();
    SetValidators();
    SetDeafultUI();
    UIConnectins();
    HandlerConnections();
    CreateMenus();
}

MainWindow::~MainWindow()
{
    _thread->quit();
    delete ui;
}

void MainWindow::slt_WorkStatus(bool status)
{
    QPushButton &btn = *ui->StartStop_Button;
    QPalette pal = btn.palette();
    if(status)
    {
        pal.setColor(QPalette::Button, QColor(Qt::red));
        btn.setText("Stop");
    }
    else
    {
        pal.setColor(QPalette::Button, QColor(Qt::green));
        btn.setText("Start");
    }
    btn.setPalette(pal);
}

void MainWindow::slt_RFState_Changed(bool state)
{
    QString btntext = state?"RF: ON":"RF: OFF";
    ui->RF_pushButton->setText(btntext);
}

void MainWindow::slt_XYpoints(int graphnum, QVector<QPointF> points)
{
    if(graphnum==1)
        _pointsVectors.clear();
    for(int i = 0; i<points.count();i++)
        points[i].rx() *= _device.stepfreqHz;

    QLineSeries *QLS = AppSettings::Graphs[graphnum-1].series;
    QString seriesname = QString("%1, %2").arg(AppSettings::measParams[AppSettings::Graphs[graphnum-1].measParam].Name, AppSettings::measFormats[AppSettings::Graphs[graphnum-1].measFormat].Name);
    QLS->setName(seriesname);
    QLS->replace(points);
    _pointsVectors.append(points);

    if(ui->diff_checkBox->isChecked()&&graphnum==AppSettings::Graphs.count())
    {
        QString diffname = AppSettings::measParams[AppSettings::Graphs[0].measParam].Name;
        for(int i = 1;i<AppSettings::Graphs.count();i++)
            diffname= diffname + " - " + AppSettings::measParams[AppSettings::Graphs[i].measParam].Name;
        _linediff->setName(diffname);

        QVector<QPointF> diffpoints = CalcDiffPointsF(_pointsVectors);
        _pointsVectors.append(diffpoints);
        _linediff->replace(diffpoints);
    }
}

QVector<QPointF> MainWindow::CalcDiffPointsF(QVector<QVector<QPointF>>&pntsVctrs)
{
    QVector<QPointF> difPnts;
    for(int i = 0; i<pntsVctrs[0].count();++i)
    {
        double diffY = pntsVctrs[0][i].ry();
        for(int ii = 1; ii<pntsVctrs.count();++ii)
            diffY-=pntsVctrs[ii][i].ry();
        difPnts.append(QPointF(pntsVctrs[0][i].rx(),diffY));
    }
    return difPnts;
}


void MainWindow::slt_error(const QString &err)
{
    QMessageBox Msgbox;
        Msgbox.setText(err);
        Msgbox.exec();
}


void MainWindow::HandlerConnections()
{
    qRegisterMetaType<QVector<QPointF>>("QVector<QPointF>");

    connect(_handler,   &Handler::sgnl_WorkStatus, //_handler to UI
            this,       &MainWindow::slt_WorkStatus);
    connect(_handler,   &Handler::sgnl_XYpoints,
            this,       &MainWindow::slt_XYpoints);
    connect(_handler,   &Handler::sgnl_RFState,
            this,       &MainWindow::slt_RFState_Changed);


    connect(ui->StartStop_Button, &QAbstractButton::clicked, //UI to _handler
            _handler, &Handler::slt_StartStop);
    connect(ui->RF_pushButton, &QAbstractButton::clicked,
            _handler, &Handler::slt_RFStateChanged);
    connect(this,   SIGNAL(sgnl_StartFreqChanged(quint64)),
            _handler,SLOT(slt_StartFreqChanged(quint64)));
    connect(this,   SIGNAL(sgnl_StopFreqChanged(quint64)),
            _handler,SLOT(slt_StopFreqChanged(quint64)));
    connect(this, SIGNAL(sgnl_CntPointsChanged(quint64)),
            _handler,SLOT(slt_CntPointsChanged(quint64)));
    connect(this, SIGNAL(sgnl_MeasFilterBandChanged(quint64)),
           _handler,SLOT(slt_MeasFilterBandChanged(quint64)));
    connect(this,&MainWindow::sgnl_MeasParamChanged,_handler, &Handler::slt_MeasParamChanged);
    connect(this,&MainWindow::sgnl_MeasFormatChanged,_handler, &Handler::slt_MeasFormatChanged);
    connect(this, &MainWindow::sgnl_CntTraceChanged,_handler,&Handler::slt_CntTraceChanged);
}

void MainWindow::_slt_StartFreq_lineEdit_editingFinished()
{
    double startfreq = Locale.toDouble(ui->StartFreq_lineEdit->text());
    double stopfreq = Locale.toDouble(ui->StopFreq_lineEdit->text());

    if(startfreq == 0 || startfreq<_device.minFreqMeg)
        ui->StartFreq_lineEdit->setText(Locale.toString(_device.minFreqMeg).simplified().remove(' '));
    else if(stopfreq<startfreq)
        ui->StartFreq_lineEdit->setText(Locale.toString(round(stopfreq-_device.stepfreqHz,6),'f',6).simplified().remove(' '));
    startfreq = Locale.toDouble(ui->StartFreq_lineEdit->text());

    quint64 startfreqi64 = round(startfreq*1000000,0);
    emit sgnl_StartFreqChanged(startfreqi64);
    _axisX->setRange(startfreq,stopfreq);
    _axisX->setTickCount(_tikscount);
    _axisX->applyNiceNumbers();
}

void MainWindow::_slt_StopFreq_lineEdit_editingFinished()
{
    double startfreq = Locale.toDouble(ui->StartFreq_lineEdit->text());
    double stopfreq = Locale.toDouble(ui->StopFreq_lineEdit->text());

    if(stopfreq == 0 || stopfreq<startfreq)
        ui->StopFreq_lineEdit->setText(Locale.toString(round(startfreq+_device.stepfreqHz,6),'f',6).simplified().remove(' '));
    else if(stopfreq > _device.maxFreqMeg)
        ui->StopFreq_lineEdit->setText(Locale.toString(_device.maxFreqMeg).simplified().remove(' '));
    stopfreq = Locale.toDouble(ui->StopFreq_lineEdit->text());

    quint64 stopfreqi64 = round(stopfreq*1000000,0);
    emit sgnl_StopFreqChanged(stopfreqi64);
    _axisX->setRange(startfreq,stopfreq);
    _axisX->setTickCount(_tikscount);
    _axisX->applyNiceNumbers();
}

void MainWindow::_slt_Scale_lineEdit_editingFinished()
{
    double scale = Locale.toDouble(ui->Scale_lineEdit->text());
    if(scale == 0)
    {
        ui->Scale_lineEdit->setText("10");
        scale = 10;
    }
    _axisY->setRange(-scale*(_tikscount-1)*0.5,scale*(_tikscount-1)*0.5);
    _axisY->setTickCount(_tikscount);
    _axisY->applyNiceNumbers();
}

void MainWindow::_slt_GraphCnt_Changed(int cnt)
{
    cnt++;
    int actualCntGraphs = ui->Graphs_listWidget->count();
    if(cnt>actualCntGraphs)
        for(int i = actualCntGraphs+1; i<=cnt;i++)
        {
            ui->Graphs_listWidget->addItem("Grph "+ QString::number(i));
            GraphParam graphparam;
            graphparam.measParam = AppSettings::rowParams[i-1];
            _chart->addSeries(graphparam.series);
            graphparam.series->setColor(GraphParam::Colors[i-1]);
            graphparam.series->attachAxis(_axisX);
            graphparam.series->attachAxis(_axisY);
            AppSettings::Graphs.append(graphparam);
        }
    else
        for(int i = actualCntGraphs-1; i>=cnt;i--)
        {
            delete ui->Graphs_listWidget->item(i);
            delete AppSettings::Graphs.last().series;
            AppSettings::Graphs.removeLast();
        }
    emit sgnl_CntTraceChanged(cnt);
}

void MainWindow::_slt_CurrentGraph_Changed(int indx)
{
    _selectedGraphIndx = indx;
    GraphParam& selectedGraph = AppSettings::Graphs[indx];
    ui->MeasParam_comboBox->setCurrentIndex(selectedGraph.measParam);
    ui->MeasFormat_comboBox->setCurrentIndex(selectedGraph.measFormat);
    qInfo()<<"Current graph: "<<indx;
}

void MainWindow::_slt_MeasParam_Changed(int indx)
{
    GraphParam& selectedGraph = AppSettings::Graphs[_selectedGraphIndx];
    if(selectedGraph.measParam != indx)
    {
    selectedGraph.measParam = indx;
    measParam& measparam = AppSettings::measParams[indx];
    qInfo()<<"_slt_MeasParam_Changed, graph: "+QString::number(_selectedGraphIndx+1)+" param: "+measparam.Param;
    emit sgnl_MeasParamChanged(_selectedGraphIndx+1, measparam.Port,measparam.Param);
    }
}

void MainWindow::_slt_MeasFormat_Changed(int indx)
{
    GraphParam& selectedGraph = AppSettings::Graphs[_selectedGraphIndx];
    if(selectedGraph.measFormat != indx)
    {
        selectedGraph.measFormat = indx;
        qInfo()<<"_slt_MeasFormat_Changed, graph: "+QString::number(_selectedGraphIndx+1)+" param: "+ QString::number(selectedGraph.measFormat);
        emit sgnl_MeasFormatChanged(_selectedGraphIndx+1,AppSettings::measFormats[indx].Format);
    }
}

void MainWindow::UIConnectins()
{
    connect(ui->StartFreq_lineEdit,SIGNAL(editingFinished()),this,SLOT(_slt_StartFreq_lineEdit_editingFinished()));
    connect(ui->StopFreq_lineEdit,SIGNAL(editingFinished()),this,SLOT(_slt_StopFreq_lineEdit_editingFinished()));
    connect(ui->Scale_lineEdit,SIGNAL(editingFinished()),this,SLOT(_slt_Scale_lineEdit_editingFinished()));

    connect(ui->PointsCnt_lineEdit,      &QLineEdit::editingFinished,
            [this] {emit sgnl_CntPointsChanged(ui->PointsCnt_lineEdit->text().toULongLong());});
    connect(ui->MeasFilterBand_lineEdit, &QLineEdit::editingFinished,
            [this] {emit sgnl_MeasFilterBandChanged(ui->MeasFilterBand_lineEdit->text().toULongLong());});
    connect(_handler, &Handler::sgnl_error,
            this, &MainWindow::slt_error);
    connect(ui->GraphCount_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &MainWindow::_slt_GraphCnt_Changed);
    connect(ui->Graphs_listWidget,&QListWidget::currentRowChanged, this,&MainWindow::_slt_CurrentGraph_Changed);
    connect(ui->MeasParam_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,&MainWindow::_slt_MeasParam_Changed);
    connect(ui->MeasFormat_comboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this,&MainWindow::_slt_MeasFormat_Changed);

    connect(ui->diff_checkBox,&QCheckBox::toggled,this,[this](bool isChecked)
    {
        if(!isChecked)
        {
         _chart->removeSeries(_linediff);
         _linediff->clear();
        }
        else
        {
         _chart->addSeries(_linediff);
         _linediff->attachAxis(_axisX);
         _linediff->attachAxis(_axisY);
         _linediff->setColor(QColor("black"));
        }
    });
}

void MainWindow::SetValidators()
{
    QRegExp rx("([0-9]{0,32},[0-9]{0,6})|([0-9]{0,32})");
    _doubleRegValidator = new QRegExpValidator(rx,this);

    _pointsValidator = new QIntValidator(_device.minPoints,_device.maxPoints,this);
    _measFilterBandValidator = new QIntValidator(_device.minMeasFilterBand,_device.maxMeasFilterBand,this);

    ui->StartFreq_lineEdit->setValidator(_doubleRegValidator);
    ui->StopFreq_lineEdit->setValidator(_doubleRegValidator);
    ui->Scale_lineEdit->setValidator(_doubleRegValidator);
    ui->PointsCnt_lineEdit->setValidator(_pointsValidator);
    ui->MeasFilterBand_lineEdit->setValidator(_measFilterBandValidator);
}

void MainWindow::SetDeafultUI()
{
    ui->StartFreq_lineEdit->setText(Locale.toString(_device.minFreqMeg).simplified().remove(' '));
    ui->StopFreq_lineEdit->setText(Locale.toString(_device.maxFreqMeg).simplified().remove(' '));
    ui->Scale_lineEdit->setText("10");
    ui->PointsCnt_lineEdit->setText("201");
    ui->MeasFilterBand_lineEdit->setText("10000");
    foreach (measParam param, AppSettings::measParams)
            ui->MeasParam_comboBox->addItem(param.Name);
    foreach (measFormat format, AppSettings::measFormats)
        ui->MeasFormat_comboBox->addItem(format.Name);
    for(int i=1;i<=AppSettings::maxGraphs;i++)
        ui->GraphCount_comboBox->addItem(QString::number(i));

    QLineSeries *QLS = AppSettings::Graphs[0].series;
    QLS->setColor(GraphParam::Colors[0]);

    _axisX->setLabelFormat("%g");
    _axisX->setTitleText("Frequency, MHz");
    _axisX->setTickCount(_tikscount);
    _axisX->setRange(_device.minFreqMeg,_device.maxFreqMeg);
    _axisX->applyNiceNumbers();

    _axisY->setTickCount(_tikscount);
    _axisY->setRange((-10*(_tikscount-1)*0.5),(10*(_tikscount-1))*0.5);

    _chart->setAxisX(_axisX);
    _chart->setAxisY(_axisY);
    _chart->addSeries(QLS);
    QLS->attachAxis(_axisX);
    QLS->attachAxis(_axisY);

    ui->graphicsView->setChart(_chart);

    ui->Graphs_listWidget->addItem("Grph 1");
    ui->Graphs_listWidget->setCurrentRow(0);
}

void MainWindow::CreateMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *exit_action = fileMenu->addAction("Screenshot chart");
    QAction *savecsv_action = fileMenu->addAction("Save to csv");
    connect(exit_action, &QAction::triggered,this,&MainWindow::_slt_ScreenshotChart);
    connect(savecsv_action,&QAction::triggered,this,&MainWindow::_slt_SaveToCSV);

}

void MainWindow::_slt_ScreenshotChart()
{
    QFileDialog objFlDlg(this);
    objFlDlg.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    objFlDlg.setOption(QFileDialog::ShowDirsOnly, true);
    objFlDlg.setAcceptMode(QFileDialog::AcceptSave);
    objFlDlg.setNameFilter("JPEG (*.jpg *.jpeg)");
    if(!objFlDlg.exec())
     return;

    QPixmap screen_shot = ui->graphicsView->grab();
    screen_shot.save(objFlDlg.selectedFiles().at(0));

}

void MainWindow::_slt_SaveToCSV()
{
    QFileDialog objFlDlg(this);
    objFlDlg.setDirectory(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    objFlDlg.setOption(QFileDialog::ShowDirsOnly, true);
    objFlDlg.setAcceptMode(QFileDialog::AcceptSave);
    objFlDlg.setNameFilter("CSV (*.csv)");
    if(!objFlDlg.exec())
     return;

     QFile file(objFlDlg.selectedFiles().at(0));
     if (file.open(QIODevice::WriteOnly)) {
         QTextStream stream(&file);
         stream << "MHz";
         for(int i = 0;i<AppSettings::Graphs.count();i++)
             stream << QString(";%1, %2").arg(AppSettings::measParams[AppSettings::Graphs[i].measParam].Name, AppSettings::measFormats[AppSettings::Graphs[i].measFormat].Name);
         if(ui->diff_checkBox->isChecked())
         {
             stream << ";" + AppSettings::measParams[AppSettings::Graphs[0].measParam].Name;
             for(int i = 1;i<AppSettings::Graphs.count();i++)
                 stream << " - " + AppSettings::measParams[AppSettings::Graphs[i].measParam].Name;
         }
         stream << endl;
         qInfo()<<QString::number(_pointsVectors.count());
         if(!_pointsVectors.isEmpty())
             for(int i = 0; i <_pointsVectors[0].count();i++)
             {
                 stream <<QString::number(_pointsVectors[0][i].rx());
                 for(int ii = 0; ii<_pointsVectors.count();++ii)
                     stream <<';'+QString::number(_pointsVectors[ii][i].ry());
                     stream << endl;
             }
     }
     file.close();
}

void MainWindow::SetDevice()
{
    _device.stepfreqHz = 0.000001;
    _device.minFreqMeg = 0.1;
    _device.maxFreqMeg = 20000;
    _device.minPoints = 2;
    _device.maxPoints = 500001;
    _device.minMeasFilterBand = 1;
    _device.maxMeasFilterBand = 1000000;
}


double MainWindow::round(double number, int n)
{
    double result,drob;
    qint64 inumber;
    int k = 1;
    if (number < 0) k = -1;
    inumber = (qint64) number;
    drob = number - inumber;
    result = ((qint64)(drob * pow(10 ,n) + k * 0.5)) / pow(10, n);
    return inumber + result;
}


