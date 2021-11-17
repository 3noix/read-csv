#include <QCoreApplication>
#include <QFile>
#include <iostream>
#include "MyCsv.h"


int main(int argc, char *argv[])
{
	QCoreApplication app{argc,argv};
	Q_UNUSED(app)
	
	QString csvFilePath = QCoreApplication::applicationDirPath() + "/airports1.csv";
	QString errorMessage;
	MyCsvData data = readCsvFile(csvFilePath,',',&errorMessage);
	
	std::cout << data.size() << " record(s)" << std::endl;
	for (const MyCsvRecord &r : data)
	{
		std::cout << r.size() << " / ";
		for (const QString &s : r) {std::cout << qPrintable(s) << "| ";}
		std::cout << std::endl;
	}
	std::cout << "last error = " << qPrintable(errorMessage) << std::endl;
	
	
	// the end
	std::cout << "Press a key to terminate" << std::endl;
	std::cin.get();
}

