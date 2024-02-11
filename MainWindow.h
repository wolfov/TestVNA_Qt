#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Handler.h"
#include "Device.h"
#include "appsettings.h"

#include <QMainWindow>
#include <QDebug>
#include <QIntValidator>
#include <QFileDialog>
#include <QStandardPaths>

#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
QT_CHARTS_USE_NAMESPACE

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QLocale Locale;
    static void ClearQLineSeries(QLineSeries*);
    double round(double, int);
    template <class T> inline T max(T a, T b){ return a > b ? a : b;}
    template <class T> inline T min(T a, T b){ return a < b ? a : b;}

signals:
    void sgnl_StartFreqChanged(quint64 freq);
    void sgnl_StopFreqChanged(quint64 freq);
    void sgnl_CntPointsChanged(quint64 points);
    void sgnl_MeasFilterBandChanged(quint64 freqband);
    void sgnl_MeasParamChanged(int, int,const QString&);
    void sgnl_MeasFormatChanged(int, const QString&);
    void sgnl_CntTraceChanged(int);
    void sgnl_StartStop(bool);

public slots:
    void slt_WorkStatus(bool status);
    void slt_XYpoints(int tr, QVector<QPointF>);
    void slt_error(const QString&);
    void slt_RFState_Changed(bool);

private slots:
    void _slt_StartFreq_lineEdit_editingFinished();
    void _slt_StopFreq_lineEdit_editingFinished();
    void _slt_Scale_lineEdit_editingFinished();
    void _slt_GraphCnt_Changed(int cnt);
    void _slt_CurrentGraph_Changed(int indx);
    void _slt_MeasParam_Changed(int indx);
    void _slt_MeasFormat_Changed(int indx);

    void _slt_ScreenshotChart();
    void _slt_SaveToCSV();


private:
    void HandlerConnections();
    void UIConnectins();
    void SetDeafultUI();
    void SetValidators();
    void SetDevice();
    void CreateMenus();

    QVector<QPointF> CalcDiffPointsF(QVector<QVector<QPointF>>&);

    Ui::MainWindow *ui;
    QMenu *fileMenu;
    Handler *_handler;
    QThread *_thread;
    Device _device;

    QRegExpValidator *_doubleRegValidator;
    QIntValidator *_pointsValidator;
    QIntValidator *_measFilterBandValidator;

    QVector<QVector<QPointF>> _pointsVectors;
    QLineSeries* _linediff = new QLineSeries();
    QChart* _chart = new QChart();
    QValueAxis *_axisX = new QValueAxis();
    QValueAxis *_axisY = new QValueAxis();
    int _tikscount = 11;
    int _selectedGraphIndx = 0;
};
#endif // MAINWINDOW_H
