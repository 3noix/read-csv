#ifndef CSV_PARSER
#define CSV_PARSER


#include <functional>
#include "CsvError.h"


namespace Csv
{

template <typename CsvTraits>
class CsvParser
{
	public:
		std::function<bool(typename CsvTraits::StringType const&)> handleComment;
		std::function<bool(typename CsvTraits::StringType const&)> handleField;
		std::function<bool()> handleEndOfLine;
		std::function<bool()> handleEndOfFile;
		std::function<void(CsvError,int,int)> handleError;
		
		CsvParser();
		CsvParser(const CsvParser &other) = delete;
		CsvParser(CsvParser &&other) = delete;
		CsvParser& operator=(const CsvParser &other) = delete;
		CsvParser& operator=(CsvParser &&other) = delete;
		~CsvParser() = default;
		
		inline void consume(typename CsvTraits::CharType c);
		void endOfData();	// informs the parser that data end has been reached.
		
		int currentLine() const {return m_currentLine;};
		int currentPosition() const {return m_currentPos;};
		bool hasError() const {return m_lastError != CsvError::NoError;}; // returns true if an error occurred
		CsvError lastError() const {return m_lastError;};
		std::string errorMessage() const
		{
			if (!this->hasError()) {return {};}
			return errorToString(m_lastError) + " (line " + std::to_string(m_currentLine) + ", column " + std::to_string(m_currentPos) + ")";
		};
		
		void reset()
		{
			m_lastError = CsvError::NoError;
			m_state = State::Initial;
			m_currentPos = 1;
			m_currentLine = 1;
			m_nbFieldsPrevLine = -1;
			m_nbFieldsCurrentLine = 0;
			CsvTraits::truncate(m_lastValue);
		};
	
	
	private:
		typename CsvTraits::StringType m_lastValue;
		
		int m_currentLine;
		int m_currentPos;
		int m_nbFieldsPrevLine;
		int m_nbFieldsCurrentLine;
		CsvError m_lastError;
		
		enum class State
		{
			Initial,
			Comment,
			Record,
			QuotedRecord,
			QuoteQuotedRecord,
			NewLine,
			BetweenRecords
		};
		
		State m_state;
		
		void call_field_handler();
		void call_end_line_handler();
		void call_comment_handler();
		void call_error_handler(CsvError err);
		
		void updateLineCounter()
		{
			++m_currentLine;
			m_currentPos = 1;
		};
};


// CONSTRUCTOR ////////////////////////////////////////////////////////////////
template <typename CsvTraits>
CsvParser<CsvTraits>::CsvParser() :
	m_currentLine{1},
	m_currentPos{1},
	m_nbFieldsPrevLine{-1},
	m_nbFieldsCurrentLine{0},
	m_lastError{CsvError::NoError},
	m_state{State::Initial}
{
}


// CALL HANDLERS //////////////////////////////////////////////////////////////
template <typename CsvTraits>
void CsvParser<CsvTraits>::call_field_handler()
{
	if (handleField && m_lastError == CsvError::NoError)
	{
		if (!handleField(m_lastValue))
			this->call_error_handler(CsvError::ErrorUserAborted);
	}
	CsvTraits::truncate(m_lastValue);
	++m_nbFieldsCurrentLine;
}

template <typename CsvTraits>
void CsvParser<CsvTraits>::call_end_line_handler()
{
	if (handleEndOfLine) {handleEndOfLine();}
	this->updateLineCounter();
	
	if (m_nbFieldsCurrentLine != m_nbFieldsPrevLine && m_nbFieldsPrevLine != -1)
	{
		this->call_error_handler(CsvError::DifferentFieldsNumbers);
	}
	m_nbFieldsPrevLine = m_nbFieldsCurrentLine;
	m_nbFieldsCurrentLine = 0;
}

template <typename CsvTraits>
void CsvParser<CsvTraits>::call_error_handler(CsvError err)
{
	m_lastError = err;
	if (handleError) {handleError(m_lastError, m_currentLine, m_currentPos);}
}

template <typename CsvTraits>
void CsvParser<CsvTraits>::call_comment_handler()
{
	if (handleComment && m_lastError == CsvError::NoError)
	{
		if (!handleComment(m_lastValue))
			this->call_error_handler(CsvError::ErrorUserAborted);
	}
	CsvTraits::truncate(m_lastValue);
}


// CONSUME ////////////////////////////////////////////////////////////////////
template <typename CsvTraits>
void CsvParser<CsvTraits>::consume(typename CsvTraits::CharType c)
{
	++m_currentPos;
	if (CsvTraits::isIgnored(c)) {return;}
	
	if (m_state == State::Initial || m_state == State::NewLine)
	{
		if (CsvTraits::isSeparator(c))
		{
			this->call_field_handler();
			m_state = State::BetweenRecords;
		}
		else if (CsvTraits::isDoubleQuote(c))
		{
			m_state = State::QuotedRecord;
		}
		else if (CsvTraits::isStartComment(c))
		{
			m_state = State::Comment;
		}
		else if (CsvTraits::isNewLine(c))
		{
			this->call_end_line_handler();
			m_state = State::NewLine;
		}
		else
		{
			CsvTraits::append(m_lastValue,c);
			m_state = State::Record;
		}
	}
	else if (m_state == State::Record)
	{
		if (CsvTraits::isSeparator(c))
		{
			this->call_field_handler();
			m_state = State::BetweenRecords;
		}
		else if (CsvTraits::isNewLine(c))
		{
			this->call_field_handler();
			this->call_end_line_handler();
			m_state = State::NewLine;
		}
		else
		{
			CsvTraits::append(m_lastValue,c);
		}
	}
	else if (m_state == State::QuoteQuotedRecord)
	{
		if (CsvTraits::isSeparator(c))
		{
			this->call_field_handler();
			m_state = State::BetweenRecords;
		}
		else if (CsvTraits::isDoubleQuote(c))
		{
			CsvTraits::append(m_lastValue,c);
			m_state = State::QuotedRecord;
		}
		else if (CsvTraits::isNewLine(c))
		{
			this->call_field_handler();
			this->call_end_line_handler();
			m_state = State::NewLine;
		}
		else
		{
			this->call_error_handler(CsvError::MalformedQuotedString);
		}
	}
	else if (m_state == State::BetweenRecords)
	{
		if (CsvTraits::isSeparator(c))
		{
			this->call_field_handler();
			m_state = State::BetweenRecords;
		}
		else if (CsvTraits::isDoubleQuote(c))
		{
			m_state = State::QuotedRecord;
		}
		else if (CsvTraits::isNewLine(c))
		{
			this->call_field_handler();
			this->call_end_line_handler();
			m_state = State::NewLine;
		}
		else
		{
			CsvTraits::append(m_lastValue,c);
			m_state = State::Record;
		}
	}
	else if (m_state == State::Comment)
	{
		if (CsvTraits::isNewLine(c))
		{
			this->call_comment_handler();
			m_state = State::NewLine;
			this->updateLineCounter();
		}
		else
		{
			CsvTraits::append(m_lastValue,c);
		}
	}
	else if (m_state == State::QuotedRecord)
	{
		if (CsvTraits::isDoubleQuote(c))
		{
			m_state = State::QuoteQuotedRecord;
		}
		else
		{
			if (CsvTraits::isNewLine(c)) {this->updateLineCounter();}
			CsvTraits::append(m_lastValue,c);
		}
	}
}


// END OF DATA ////////////////////////////////////////////////////////////////
template <typename CsvTraits>
void CsvParser<CsvTraits>::endOfData()
{
	if (m_state == State::QuoteQuotedRecord || m_state == State::Record || m_state == State::BetweenRecords)
	{
		this->call_field_handler();
		this->call_end_line_handler();
	}
	else if (m_state == State::QuotedRecord) {this->call_error_handler(CsvError::UnterminatedQuotedString);}
	else if (m_state == State::Comment)      {this->call_comment_handler();}
	else if (m_state == State::Initial)      {this->call_error_handler(CsvError::ErrorEmptyFile);}
	
	
	if (handleEndOfFile && m_lastError == CsvError::NoError)
	{
		if (!handleEndOfFile())
			m_lastError = CsvError::ErrorUserAborted;
	}
}

}

#endif

