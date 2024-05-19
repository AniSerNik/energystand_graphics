#pragma once
#include <QtWidgets/QMainWindow>
#include <QCloseEvent>
#include <QtSerialPort>
#include <QTimer>
#include "ui_MyApp.h"
#include "QTGrafics.h"
#include "DataAnalitycs.h"

class MyApp : public QMainWindow {
    Q_OBJECT

public:
    MyApp(QWidget* parent = nullptr);
    ~MyApp();
    void start();
    void readData();
    void end();
    void openSettingsMenu();
    void closeEvent(QCloseEvent* event);
private:
    Ui::MyApp ui;
    QSerialPort* serialPort;
    QTGrafics* qtgrafic;
    DataAnalitycs* dataAnaliz;
    QTimer* updateGraphtmr;
    QTimer* updateAnalitycstmr;
    QByteArray charBufferRead;
    //for settings dialog
    QDialog* _mwindow;
    QGridLayout* _mlayout;
    QLabel* _selComLabel;
    QLabel* _selBaudLabel;
    QComboBox* _listOfPort;
    QSpinBox* _inputBaudBox;
    QPushButton* _submitButt;
    QSerialPortInfo* serialInfo;

    QThread* grafthread;

    bool _enableportplain = true,
        _isActiveCom = false;

    void _setPortStatus(QString status);
    void _changeVisibleData();
    void _setAutoUpdates();
    void _setLastData();
    void _setLastDataCount();
    void _addValueToGraph(dataMeasure newData);
    void _clearPortDataPlain();
    void _setEnablePortPlain();
    dataMeasure _parseReadString(QString str);
    void _changeActivityCom();
    void _setButtonComVisualMode(int i = 0);
    void _resetActivity();
    void _resetComPort();
    bool _isValidDataDM(dataMeasure* dm);
    void _openAnalitycs();
    void _disableAutoRange();
    //settings
    void _setSettings();
    QStringList _getAllNamesPort();
    void _errorPort();
private slots:
    void _updateGrafics();
    void _updateAnalitycs();
};

