#include "QTGrafics.h"

QTGrafics::QTGrafics(QWidget *parent) {}

QTGrafics::QTGrafics(QCustomPlot* UIgfplot) {
    gfplot = UIgfplot;
    gfplot->setInteractions(QCP::iRangeDrag|QCP::iSelectLegend|QCP::iRangeZoom);
    gfplot->setLocale(QLocale(QLocale::Russian, QLocale::RussianFederation));
    connect(gfplot, SIGNAL(legendClick(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)), this, SLOT(_legendSelect(QCPLegend*, QCPAbstractLegendItem*, QMouseEvent*)));

    gfplot->setOpenGl(true, 8);
    gfplot->setAntialiasedElements(QCP::aeAll);
    gfplot->setNotAntialiasedElements(QCP::aeAxes);
}

QTGrafics::~QTGrafics(){}

void QTGrafics::updateGraph() {
    std::pair<double, double> axisY(0, 0), axisX(0, 0);
    bool isNullGrafics = false;
    for (int i = 0; i < gfplot->graphCount(); i++) {
        if (_enabledGraph[i] && _autoRange) {
            if (_autoRange) {
                axisY = _getRangeAxis(&graf_y[i]);  
                double tickKoef = abs(axisY.second - axisY.first) * 0.1;
                gfplot->graph(i)->valueAxis()->setRange(axisY.first - tickKoef, axisY.second + tickKoef);
            }
            isNullGrafics = true;
        }
    }
    if (isNullGrafics) {
        if (graf_x.size() > 0) {
            axisX.first = *graf_x.begin();
            axisX.second = graf_x[graf_x.size() - 1];
        }
        if (_onlyLastData)
            gfplot->graph(0)->keyAxis()->setRange(axisX.second, _countLastElem, Qt::AlignRight);
        else
            gfplot->graph(0)->keyAxis()->setRange(axisX.first, axisX.second);
    }
    gfplot->replot(QCustomPlot::rpQueuedReplot);
}

int QTGrafics::getDataCount() {
    return graf_x.size();
}

void QTGrafics::clearAllData() {
    _clearInnerData();
    setLegend(false);
}

void QTGrafics::addData(dataMeasure gfdata) {
    _addData(&gfdata);
}

QVector<QVector<double>>* QTGrafics::getDataAxisValue() {
    return &graf_y;
}

QVector<double>* QTGrafics::getDataAxisDate() {
    return &graf_x;
}

void QTGrafics::initGraph() {
    _clearInnerData();
    graf_y.clear();
    _enabledGraph.clear();
    gfplot->clearGraphs();
    gfplot->plotLayout()->clear();
    //setup axis
    QCPAxisRect* axisRect = new QCPAxisRect(gfplot, true);
    axisRect->setupFullAxesBox(false);
    QCPLayoutGrid* subLayout = new QCPLayoutGrid;
    gfplot->plotLayout()->addElement(0, 0, axisRect);
    gfplot->plotLayout()->addElement(1, 1, subLayout);
    axisRect->addAxes(QCPAxis::atLeft);
    foreach(auto * rect, gfplot->axisRects()) {
        foreach(auto * axis, rect->axes()) {
            axis->setLayer("axes");
            axis->grid()->setLayer("grid");
        }
    }
    QSharedPointer<QCPAxisTickerDateTime> dateTicker(new QCPAxisTickerDateTime);
    dateTicker->setDateTimeFormat("d MMMM yyyy\nHH:mm:ss");
    axisRect->axis(QCPAxis::atBottom)->setTicker(dateTicker);
    axisRect->axis(QCPAxis::atRight)->setTickLabels(true);
    axisRect->axis(QCPAxis::atBottom)->setLabel("Дата и Время");
    axisRect->axis(QCPAxis::atLeft)->setLabel("Напряжение");
    axisRect->axis(QCPAxis::atLeft, 1)->setLabel("Сила тока");
    axisRect->axis(QCPAxis::atRight)->setLabel("Мощность");
    axisRect->axis(QCPAxis::atLeft)->setLabelPadding(0);
    axisRect->axis(QCPAxis::atLeft, 1)->setLabelPadding(0);
    axisRect->axis(QCPAxis::atRight)->setLabelPadding(0);
    axisRect->setRangeDragAxes(QList{axisRect->axis(QCPAxis::atBottom)}, {axisRect->axis(QCPAxis::atLeft), axisRect->axis(QCPAxis::atLeft, 1),
        axisRect->axis(QCPAxis::atRight)});
    axisRect->setRangeZoomAxes(QList{ axisRect->axis(QCPAxis::atBottom) });
    //
    _enabledGraph = { true, true, true };
    for (int i = 0; i < 3; i++)
        graf_y.push_back({});
    _restoreLegend();
    gfplot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft))->setName("Напряжение");
    gfplot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atLeft, 1))->setName("Сила тока");
    gfplot->addGraph(axisRect->axis(QCPAxis::atBottom), axisRect->axis(QCPAxis::atRight))->setName("Мощность");
    gfplot->graph(VoltageIndex)->setPen(QPen(Qt::blue));
    gfplot->graph(CurrentIndex)->setPen(QPen(Qt::red));
    gfplot->graph(PowerIndex)->setPen(QPen(Qt::green));
    setLegend(true);
}

void QTGrafics::setLegend(bool enable) {
    if (gfplot->legend == NULL)
        _restoreLegend();
    gfplot->legend->setVisible(enable);
    _isSetLegend = enable;
}

void QTGrafics::setEnabledGraphs(QVector<bool> qv) {
    for (int i = 0; i < qv.size(); i++) {
        if (i > gfplot->graphCount())
            break;
        _enabledGraph[i] = qv[i];
        gfplot->graph(i)->setVisible(qv[i]);
        gfplot->graph(i)->valueAxis()->setVisible(qv[i]);
    }
}

void QTGrafics::setOnlyLastData(bool enable) {
    _onlyLastData = enable;
}

void QTGrafics::setCountLastData(int count) {
    _countLastElem = count;
}

void QTGrafics::disableAutoRange(bool enable) {
    _autoRange = !enable;
}

void QTGrafics::_addData(dataMeasure* gfdata) {
    if (graf_y.count() != 3)
        return;
    int curIndex = getDataCount();
    graf_y[VoltageIndex].push_back(gfdata->voltage);
    graf_y[CurrentIndex].push_back(gfdata->current);
    graf_y[PowerIndex].push_back(gfdata->power);
    graf_x.push_back(gfdata->unix);
    for (int i = 0; i < gfplot->graphCount(); i++)
        gfplot->graph(i)->addData(graf_x[curIndex], graf_y[i][curIndex]);
}

void QTGrafics::_clearInnerData() {
    graf_x.clear();
    for(int i = 0; i < graf_y.size(); i++)
        graf_y[i].clear();
}

void QTGrafics::_restoreLegend() {
    gfplot->legend = new QCPLegend;
    gfplot->axisRect()->insetLayout()->addElement(gfplot->legend, Qt::AlignRight | Qt::AlignTop);
    gfplot->axisRect()->insetLayout()->setMargins(QMargins(12, 12, 12, 12));
    gfplot->legend->setLayer("legend");
    gfplot->legend->setSelectableParts(QCPLegend::spItems);
    setLegend(false);
}

std::pair<double, double> QTGrafics::_getRangeAxis(QVector<double>* vect) {
    std::pair<double, double> pairT(0, 0);
    if (vect->size() > 0) {
        pairT.first = *std::min_element(vect->begin(), vect->end());
        pairT.second = *std::max_element(vect->begin(), vect->end());
    }
    return pairT;
}

void QTGrafics::_legendSelect(QCPLegend* legend, QCPAbstractLegendItem* item, QMouseEvent* event) {
    QPen Pen1;
    const int penWidth = 5;
    bool isHaveSelected = false;
    QCPPlottableLegendItem* plItem;
    if (plItem = qobject_cast<QCPPlottableLegendItem*>(item)) {
        Pen1 = plItem->plottable()->pen();
        if (Pen1.widthF() != penWidth) {
            Pen1.setWidthF(penWidth);
            plItem->plottable()->valueAxis()->grid()->setVisible(true);
            plItem->plottable()->valueAxis()->setVisible(true);
            plItem->plottable()->setPen(Pen1);
            isHaveSelected = true;
        }
    }
    for (int i = 0; i < 3; i++) {
        if (isHaveSelected && plItem->plottable() == gfplot->graph(i))
            continue;
        Pen1 = gfplot->graph(i)->pen();
        if (Pen1.widthF() == penWidth) {
            Pen1.setWidthF(1);
            gfplot->graph(i)->setPen(Pen1);
        }
        gfplot->graph(i)->valueAxis()->grid()->setVisible(false);
        if (isHaveSelected)
            gfplot->graph(i)->valueAxis()->setVisible(false);
        else
            gfplot->graph(i)->valueAxis()->setVisible(true);
    }
    if (!isHaveSelected)
        gfplot->graph(0)->valueAxis()->grid()->setVisible(true);
    gfplot->replot(QCustomPlot::rpQueuedReplot);
}