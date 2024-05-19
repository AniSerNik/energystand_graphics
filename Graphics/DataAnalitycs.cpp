#include "DataAnalitycs.h"

DataAnalitycs::DataAnalitycs(QWidget* parent) {
	_mwindow = new QDialog(this);
	_mlayout = new QGridLayout();
	_avgCurrentLabel = new QLabel();
	_avgPowerLabel = new QLabel();
	_minCurrentLabel = new QLabel();
	_maxCurrentLabel = new QLabel();
	_minPowerLabel = new QLabel();
	_maxPowerLabel = new QLabel();
	_minVoltageLabel = new QLabel();
	_maxVoltageLabel = new QLabel();
	_countRecordLabel = new QLabel();
	_mwindow->setLayout(_mlayout);
	_mlayout->addWidget(_countRecordLabel);
	_mlayout->addWidget(_minVoltageLabel);
	_mlayout->addWidget(_maxVoltageLabel);
	_mlayout->addWidget(_minCurrentLabel);
	_mlayout->addWidget(_maxCurrentLabel);
	_mlayout->addWidget(_avgCurrentLabel);
	_mlayout->addWidget(_minPowerLabel);
	_mlayout->addWidget(_maxPowerLabel);
	_mlayout->addWidget(_avgPowerLabel);
}

DataAnalitycs::~DataAnalitycs() {}

void DataAnalitycs::updateInfo(const QVector<double>& date, const QVector<QVector<double>>& value) {
	double minPower = 0, maxPower = 0, minCurrent = 0, maxCurrent = 0, 
		minVoltage = 0, maxVoltage = 0, avgCurrent = 0, avgPower = 0, recordsCount = date.size();
	QVector<double> voltage = value[QTGrafics::VoltageIndex];
	QVector<double> current = value[QTGrafics::CurrentIndex];
	QVector<double> power = value[QTGrafics::PowerIndex];
	if (recordsCount > 0 && voltage.size() == recordsCount
		&& current.size() == recordsCount && power.size() == recordsCount) {
		avgCurrent = _averageVector(current);
		avgPower = _averageVector(power);
		minVoltage = *std::min_element(voltage.begin(), voltage.end());
		maxVoltage = *std::max_element(voltage.begin(), voltage.end());
		minPower = *std::min_element(power.begin(), power.end());
		maxPower = *std::max_element(power.begin(), power.end());
		minCurrent = *std::min_element(current.begin(), current.end());
		maxCurrent = *std::max_element(current.begin(), current.end());
	}
	_countRecordLabel->setText("Количество измерений: " + QString::number(recordsCount));
	_minVoltageLabel->setText("Мин Напряжение: " + QString::number(minVoltage, 'f', 6) + "V");
	_maxVoltageLabel->setText("Макс Напряжение: " + QString::number(maxVoltage, 'f', 6) + "V");
	_minCurrentLabel->setText("Мин сила тока: " + QString::number(minCurrent, 'f', 6) + "A");
	_maxCurrentLabel->setText("Макс сила тока: " + QString::number(maxCurrent, 'f', 6) + "A");
	_avgCurrentLabel->setText("Средняя сила тока: " + QString::number(avgCurrent, 'f', 6) + "A");
	_minPowerLabel->setText("Мин мощность: " + QString::number(minPower, 'f', 6) + "W");
	_maxPowerLabel->setText("Макс мощность: " + QString::number(maxPower, 'f', 6) + "W");
	_avgPowerLabel->setText("Средняя мощность: " + QString::number(avgPower, 'f', 6) + "W");
}


void DataAnalitycs::openWindow() {
	_mwindow->show();
}

void DataAnalitycs::closeWindow() {
	_mwindow->close();
}

double DataAnalitycs::_averageVector(QVector<double> const& v) {
	int n = 0;
	double mean = 0.0;
	for (auto x : v) {
		double delta = x - mean;
		mean += delta / ++n;
	}
	return mean;
}
