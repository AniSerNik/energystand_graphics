#pragma once
#include <QtCore/QVariant>
#include <QtWidgets/QMainWindow>
#include "qcustomplot.h"

typedef struct dataMeasure {
    double voltage = -1;
    double current = -1;
    double power = -1;
    double unix;
}dataMeasure;

class QTGrafics : public QObject {
    Q_OBJECT

public:
    QTGrafics(QWidget *parent = nullptr);
    QTGrafics(QCustomPlot* UIgfplot);
    ~QTGrafics();
    void initGraph();
    void updateGraph();
    int getDataCount();
    void clearAllData();
    void addData(dataMeasure gfdata);
    QVector<QVector<double>>* getDataAxisValue();
    QVector<double>* getDataAxisDate();
    void setLegend(bool enable);
    void setEnabledGraphs(QVector<bool> qv);
    void setOnlyLastData(bool enable);
    void setCountLastData(int count);
    void disableAutoRange(bool enable);
    enum dataIndex {
        VoltageIndex = 0,
        CurrentIndex = 1,
        PowerIndex = 2
    }; 
private:
    QCustomPlot* gfplot;
    QVector<QVector<double>> graf_y; //for graf
    QVector<double> graf_x;
    QVector<bool> _enabledGraph;
    int _countLastElem = 10;
    bool _isSetLegend = false, _onlyLastData = false,
        _autoRange = true;

    void _addData(dataMeasure* gfdata);
    void _clearInnerData();
    void _restoreLegend();
    std::pair<double, double> _getRangeAxis(QVector<double>* vect);
private slots:
    void _legendSelect(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event);
};
