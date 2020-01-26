#ifndef CSV
#define CSV


#include "CsvParser.h"
#include "CsvDataWriter.h"
#include "CsvData.h"
#include "MyCsvTraits.h"

using MyCsvData = Csv::CsvData<Csv::CsvQtTraits>;
using MyCsvRecord = Csv::CsvRecord<Csv::CsvQtTraits>;


MyCsvData readCsvFile(const QString &csvFilePath, QChar sep, QString *errorMessage)
{
	// read the full csv file
	QFile csvFile{csvFilePath};
	if (!csvFile.open(QIODevice::ReadOnly))
	{
		if (errorMessage) {*errorMessage = "Could not open file: " + csvFilePath;}
		return {};
	}
	QString rawData = csvFile.readAll();
	rawData.remove('\r');
	csvFile.close();
	
	
	// parse csv data
	Csv::CsvQtTraits::separator = sep;
	Csv::CsvParser<Csv::CsvQtTraits> parser;
	Csv::CsvData<Csv::CsvQtTraits> data;
	Csv::CsvDataWriter<Csv::CsvQtTraits> dataWriter{data,parser,false};
	for (QChar c : rawData)
	{
		parser.consume(c);
		if (parser.hasError())
		{
			if (errorMessage) {*errorMessage = QString::fromStdString(errorToString(parser.lastError()));}
			return data;
		}
	}
	
	// the end
	if (parser.hasError() && errorMessage) {*errorMessage = QString::fromStdString(errorToString(parser.lastError()));}
	return data;
}

#endif

