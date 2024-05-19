#pragma once
#include <QtWidgets/QMainWindow>
#include "QTGrafics.h"

class DataAnalitycs : public QMainWindow {
    Q_OBJECT
public:
    DataAnalitycs(QWidget* parent = nullptr);
    ~DataAnalitycs();
    void updateInfo(const QVector<double>& date, const QVector<QVector<double>>& value);
    void openWindow();
    void closeWindow();
private:
    QDialog* _mwindow;
    QGridLayout* _mlayout;
    QLabel *_countRecordLabel,
        * _avgCurrentLabel, * _avgPowerLabel,
        *_minPowerLabel, *_maxPowerLabel,
        *_minCurrentLabel, *_maxCurrentLabel,
        *_minVoltageLabel, *_maxVoltageLabel;
    double _averageVector(QVector<double> const& v);
};

