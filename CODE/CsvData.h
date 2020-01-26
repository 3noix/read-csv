#ifndef CSV_DATA
#define CSV_DATA


#include <vector>
#include <string>


namespace Csv
{

template <typename CsvTraits>
class CsvDataWriter;

template <typename CsvTraits>
class CsvData;

template <typename CsvTraits>
class CsvRecord
{
	public:
		using StringType = typename CsvTraits::StringType;
		using const_iterator = typename std::vector<typename CsvTraits::StringType>::const_iterator;
		
		CsvRecord(const CsvData<CsvTraits> &owner) : m_owner{owner} {};
		const_iterator begin() const {return m_fields.begin();};
		const_iterator end() const {return m_fields.end();};
	
		const StringType& operator[](int i) const {return m_fields[i];};
		int size() const {return m_fields.size();};
	
		StringType operator[](const StringType &header) const;
	
	
	private:
		void addField(const StringType &field) {m_fields.push_back(field);};
		
		friend class CsvData<CsvTraits>;
		
		const CsvData<CsvTraits>& m_owner;
		std::vector<StringType> m_fields;
};


template <typename CsvTraits>
class CsvData
{
	public:
		using StringType = typename CsvTraits::StringType;
		using const_iterator = typename std::vector<CsvRecord<CsvTraits>>::const_iterator;
		
		const_iterator begin() const {return m_records.begin();};
		const_iterator end() const {return m_records.end();};
		const CsvRecord<CsvTraits>& operator[](int i) {return m_records[i];};
		int size() const {return m_records.size();};
		const std::vector<StringType>& headers() const {return m_headers;};
		
		
	private:
		friend class CsvDataWriter<CsvTraits>;
		
		void newRecord() {m_records.emplace_back(*this);};
		void addField(const StringType& field) {m_records.back().addField(field);};
		
		std::vector<CsvRecord<CsvTraits>> m_records;
		std::vector<StringType> m_headers;
};


template <typename CsvTraits>
typename CsvRecord<CsvTraits>::StringType CsvRecord<CsvTraits>::operator[](const StringType &header) const
{
	for (int i=0; i<m_owner.headers().size(); ++i)
	{
		if (m_owner.headers()[i] == header)
			return m_fields[i];
	}
	return StringType{};
}

}

#endif

