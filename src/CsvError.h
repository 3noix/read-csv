#ifndef CSV_ERROR
#define CSV_ERROR

namespace Csv
{

enum class CsvError
{
	NoError,
	ErrorOther,
	ErrorUserAborted,
	MalformedQuotedString,
	UnterminatedQuotedString,
	ErrorEmptyFile,
	DifferentFieldsNumbers
};

std::string errorToString(CsvError e)
{
	if (e == CsvError::NoError) {return "No error";}
	if (e == CsvError::ErrorOther) {return "Other";}
	if (e == CsvError::ErrorUserAborted) {return "User aborted";}
	if (e == CsvError::MalformedQuotedString) {return "Malformed quoted string";}
	if (e == CsvError::UnterminatedQuotedString) {return "Unterminated quoted string";}
	if (e == CsvError::ErrorEmptyFile) {return "Empty file";}
	if (e == CsvError::DifferentFieldsNumbers) {return "Different fields numbers";}
	return "";
};

}
#endif

