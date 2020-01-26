#ifndef MY_CSV_TRAITS
#define MY_CSV_TRAITS


namespace Csv
{

template <typename StringType_, typename CharType_ = typename StringType_::value_type>
class CsvGenericTraits
{
	public:
		using StringType = StringType_;
		using CharType = CharType_;
		
		static void append(StringType& ref, CharType val) {ref.push_back(val);}
		static void truncate(StringType& ref) {ref.clear();}
		static bool isSeparator(CharType c) {return c == CharType{','};}
		static bool isNewLine(CharType c) {return (c == CharType{'\n'} || c == CharType{'\r'});}
		static bool isDoubleQuote(CharType c) {return c == CharType{'"'};}
		static bool isStartComment(CharType c) {return c == CharType{'#'};}
};


#include <QString>
class CsvQtTraits
{
	public:
		using StringType = QString;
		using CharType = QChar;
		
		static void append(StringType& ref, CharType val) {ref.push_back(val);}
		static void truncate(StringType& ref) {ref.clear();}
		static bool isSeparator(CharType c) {return c == separator;}
		static bool isNewLine(CharType c) {return (c == CharType{'\n'} || c == CharType{'\r'});}
		static bool isDoubleQuote(CharType c) {return c == CharType{'"'};}
		static bool isStartComment(CharType c) {return c == CharType{'#'};}
		
		static CharType separator;
};
CsvQtTraits::CharType CsvQtTraits::separator = ',';


#include <string>
class CsvStlTraits
{
	public:
		using StringType = std::string;
		using CharType = char;
		
		static void append(StringType& ref, CharType val) {ref.push_back(val);}
		static void truncate(StringType& ref) {ref.clear();}
		static bool isSeparator(CharType c) {return c == separator;}
		static bool isNewLine(CharType c) {return (c == CharType{'\n'} || c == CharType{'\r'});}
		static bool isDoubleQuote(CharType c) {return c == CharType{'"'};}
		static bool isStartComment(CharType c) {return c == CharType{'#'};}
		
		static CharType separator;
};
CsvStlTraits::CharType CsvStlTraits::separator = ',';

}
#endif
