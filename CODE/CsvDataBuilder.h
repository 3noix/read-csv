#ifndef CSV_DATA_WRITER
#define CSV_DATA_WRITER


#include "CsvParser.h"
#include "CsvData.h"
#include <vector>
#include <string>

namespace Csv
{

template <typename CsvTraits>
class CsvDataBuilder
{
	public:
		using StringType = typename CsvTraits::StringType;
		
		
		CsvDataBuilder(CsvData<CsvTraits> &data, CsvParser<CsvTraits> &parser, bool hasHeaderLine) :
			m_data{data},
			m_hasHeaders{hasHeaderLine},
			m_atBeginningOfLine{true},
			m_atFirstLine{true}
		{
			parser.handleField = [this](const StringType &s){return this->handleField(s);};
			parser.handleEndOfLine = [this](){return this->handleEndOfLine();};
		}
		
		bool handleField(const StringType &s)
		{
			if (m_atFirstLine && m_hasHeaders) {m_data.m_headers.push_back(s);}
			else
			{
				if (m_atFirstLine)
				{
					std::string str = std::string{"header"} + std::to_string(m_data.m_headers.size());
					m_data.m_headers.push_back(StringType{str.c_str()});
				}
				if (m_atBeginningOfLine)
				{
					m_data.newRecord();
					m_atBeginningOfLine = false;
				}
				m_data.addField(s);
			}
			return true;
		}
		
		bool handleEndOfLine()
		{
			m_atBeginningOfLine = true;
			m_atFirstLine = false;
			return true;
		}
		
		
	private:
		CsvData<CsvTraits>& m_data;
		
		bool m_hasHeaders;
		bool m_atBeginningOfLine;
		bool m_atFirstLine;
};
}

#endif

