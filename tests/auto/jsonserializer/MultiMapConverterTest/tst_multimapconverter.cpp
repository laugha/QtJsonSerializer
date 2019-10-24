#include <QtTest>
#include <QtJsonSerializer>

#include "typeconvertertestbase.h"

#include <QtJsonSerializer/private/qjsonmultimapconverter_p.h>

template<template <typename, typename> class TContainer>
bool tCompare(const TContainer<QString, int> &lhs, const TContainer<QString, int> &rhs) {
	if(lhs.size() != rhs.size())
		return false;
	if(lhs.isEmpty())
		return true;

	for(auto it = lhs.constBegin(); it != lhs.constEnd(); it++) {
		auto fIt = std::find_if(rhs.keyValueBegin(), rhs.keyValueEnd(), [it](const std::pair<QString, int> &v){
			return it.key() == v.first &&
					it.value() == v.second;
		});
		if(fIt != rhs.keyValueEnd())
			return true;
	}
	return false;
}

// create operators with higher priority to be used by qvariant compare
bool operator==(const QMultiMap<QString, int> &lhs, const QMultiMap<QString, int> &rhs) {
	return tCompare(lhs, rhs);
}

bool operator==(const QMultiHash<QString, int> &lhs, const QMultiHash<QString, int> &rhs) {
	return tCompare(lhs, rhs);
}

class MultiMapConverterTest : public TypeConverterTestBase
{
	Q_OBJECT

protected:
	void initTest() override;
	bool compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line) override;
	QJsonTypeConverter *converter() override;
	void addConverterData() override;
	void addMetaData() override;
	void addCommonSerData() override;
	void addSerData() override;
	void addDeserData() override;

private:
	QJsonMultiMapConverter _converter;
};

void MultiMapConverterTest::initTest()
{
	QMetaType::registerEqualsComparator<QMultiMap<QString, int>>();
	QMetaType::registerEqualsComparator<QMultiHash<QString, int>>();
}

bool MultiMapConverterTest::compare(int type, QVariant &actual, QVariant &expected, const char *aName, const char *eName, const char *file, int line)
{
	return TypeConverterTestBase::compare(type, actual, expected, aName, eName, file, line);
}

QJsonTypeConverter *MultiMapConverterTest::converter()
{
	return &_converter;
}

void MultiMapConverterTest::addConverterData()
{
	QTest::newRow("multimap") << static_cast<int>(QJsonTypeConverter::Standard);
}

void MultiMapConverterTest::addMetaData()
{
	QTest::newRow("int") << qMetaTypeId<QMultiMap<QString, int>>()
						 << static_cast<QCborTag>(QCborSerializer::NoTag)
						 << QCborValue::Map
						 << true
						 << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("string") << qMetaTypeId<QMultiMap<QString, QString>>()
							<< static_cast<QCborTag>(QCborSerializer::MultiMap)
							<< QCborValue::Map
							<< true
							<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("object") << qMetaTypeId<QMultiMap<QString, QObject*>>()
							<< static_cast<QCborTag>(QCborSerializer::ExplicitMap)
							<< QCborValue::Map
							<< true
							<< QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("list") << qMetaTypeId<QMultiMap<QString, QMap<QString, bool>>>()
						  << static_cast<QCborTag>(QCborSerializer::NoTag)
						  << QCborValue::Array
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;
	QTest::newRow("pair") << qMetaTypeId<QMultiMap<int, QPair<int, bool>>>()
						  << static_cast<QCborTag>(QCborSerializer::MultiMap)
						  << QCborValue::Array
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("hash") << qMetaTypeId<QMultiHash<QString, int>>()
						  << static_cast<QCborTag>(QCborSerializer::ExplicitMap)
						  << QCborValue::Array
						  << true
						  << QJsonTypeConverter::DeserializationCapabilityResult::Positive;

	QTest::newRow("invalid.pair") << qMetaTypeId<QPair<QString, int>>()
								  << static_cast<QCborTag>(QCborSerializer::NoTag)
								  << QCborValue::Map
								  << false
								  << QJsonTypeConverter::DeserializationCapabilityResult::Negative;
	QTest::newRow("invalid.tag") << qMetaTypeId<QMultiMap<int, int>>()
								 << static_cast<QCborTag>(QCborSerializer::ConstructedObject)
								 << QCborValue::Map
								 << true
								 << QJsonTypeConverter::DeserializationCapabilityResult::WrongTag;
}

void MultiMapConverterTest::addCommonSerData()
{
	QTest::newRow("empty.map") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<QMultiMap<QString, int>>()
							   << QVariant::fromValue(QMultiMap<QString, int>{})
							   << QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{}}
							   << QJsonValue{QJsonObject{}};
	QTest::newRow("empty.arr") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
							   << TestQ{}
							   << static_cast<QObject*>(nullptr)
							   << qMetaTypeId<QMultiMap<QString, int>>()
							   << QVariant::fromValue(QMultiMap<QString, int>{})
							   << QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborArray{}}
							   << QJsonValue{QJsonArray{}};
	QTest::newRow("empty.dense") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::DenseMap)}}
								 << TestQ{}
								 << static_cast<QObject*>(nullptr)
								 << qMetaTypeId<QMultiMap<QString, int>>()
								 << QVariant::fromValue(QMultiMap<QString, int>{})
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{}}
								 << QJsonValue{QJsonObject{}};
	QTest::newRow("filled.map.arr") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
									<< TestQ{
										   {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										   {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										   {QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
										   {QMetaType::Int, 1, 2},
										   {QMetaType::Int, 3, 4},
										   {QMetaType::Int, 5, 6}
									   }
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QMultiMap<QString, int>>()
									<< QVariant::fromValue(QMultiMap<QString, int>{
										   {QStringLiteral("a"), 3},
										   {QStringLiteral("a"), 1},
										   {QStringLiteral("b"), 5}
									   })
									<< QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborArray{
											QCborArray{QStringLiteral("A"), 2},
											QCborArray{QStringLiteral("A"), 4},
											QCborArray{QStringLiteral("B"), 6}
										}}
									<< QJsonValue{QJsonArray{
											QJsonArray{QStringLiteral("A"), 2},
											QJsonArray{QStringLiteral("A"), 4},
											QJsonArray{QStringLiteral("B"), 6}
										}};
	QTest::newRow("filled.hash") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
								 << TestQ{
										{QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										{QMetaType::Int, 1, 2}
									}
								 << static_cast<QObject*>(this)
								 << qMetaTypeId<QMultiHash<QString, int>>()
								 << QVariant::fromValue(QMultiHash<QString, int>{
										{QStringLiteral("a"), 1}
									})
								 << QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{
										{QStringLiteral("A"), QCborArray{2}}
									}}
								 << QJsonValue{QJsonObject{
										{QStringLiteral("A"), QJsonArray{2}}
									}};
}

void MultiMapConverterTest::addSerData()
{
	QTest::newRow("filled.map.map") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
									<< TestQ{
										   {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										   {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										   {QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
										   {QMetaType::Int, 1, 2},
										   {QMetaType::Int, 3, 4},
										   {QMetaType::Int, 5, 6}
									   }
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QMultiMap<QString, int>>()
									<< QVariant::fromValue(QMultiMap<QString, int>{
										   {QStringLiteral("a"), 3},
										   {QStringLiteral("a"), 1},
										   {QStringLiteral("b"), 5}
									   })
									<< QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{
																										{QStringLiteral("A"), QCborArray{2, 4}},
																										{QStringLiteral("B"), QCborArray{6}}
																									}}
									<< QJsonValue{QJsonObject{
										   {QStringLiteral("A"), QJsonArray{2, 4}},
										   {QStringLiteral("B"), QJsonArray{6}}
									   }};
	QTest::newRow("filled.map.dense") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::DenseMap)}}
									  << TestQ{
											 {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
											 {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
											 {QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
											 {QMetaType::Int, 1, 2},
											 {QMetaType::Int, 3, 4},
											 {QMetaType::Int, 5, 6}
										 }
									  << static_cast<QObject*>(this)
									  << qMetaTypeId<QMultiMap<QString, int>>()
									  << QVariant::fromValue(QMultiMap<QString, int>{
											 {QStringLiteral("a"), 3},
											 {QStringLiteral("a"), 1},
											 {QStringLiteral("b"), 5}
										 })
									  << QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{
																										  {QStringLiteral("A"), QCborArray{2, 4}},
																										  {QStringLiteral("B"), 6}
																									  }}
									  << QJsonValue{QJsonObject{
											 {QStringLiteral("A"), QJsonArray{2, 4}},
											 {QStringLiteral("B"), 6}
										 }};
}

void MultiMapConverterTest::addDeserData()
{
	QTest::newRow("filled.map.map") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
									<< TestQ{
										   {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
										   {QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
										   {QMetaType::Int, 1, 2},
										   {QMetaType::Int, 3, 4},
										   {QMetaType::Int, 5, 6}
									   }
									<< static_cast<QObject*>(this)
									<< qMetaTypeId<QMultiMap<QString, int>>()
									<< QVariant::fromValue(QMultiMap<QString, int>{
										   {QStringLiteral("a"), 3},
										   {QStringLiteral("a"), 1},
										   {QStringLiteral("b"), 5}
									   })
									<< QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{
																										{QStringLiteral("A"), QCborArray{2, 4}},
																										{QStringLiteral("B"), QCborArray{6}}
																									}}
									<< QJsonValue{QJsonObject{
										   {QStringLiteral("A"), QJsonArray{2, 4}},
										   {QStringLiteral("B"), QJsonArray{6}}
									   }};
	QTest::newRow("filled.map.dense") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::DenseMap)}}
									  << TestQ{
											 {QMetaType::QString, QStringLiteral("a"), QStringLiteral("A")},
											 {QMetaType::QString, QStringLiteral("b"), QStringLiteral("B")},
											 {QMetaType::Int, 1, 2},
											 {QMetaType::Int, 3, 4},
											 {QMetaType::Int, 5, 6}
										 }
									  << static_cast<QObject*>(this)
									  << qMetaTypeId<QMultiMap<QString, int>>()
									  << QVariant::fromValue(QMultiMap<QString, int>{
											 {QStringLiteral("a"), 3},
											 {QStringLiteral("a"), 1},
											 {QStringLiteral("b"), 5}
										 })
									  << QCborValue{static_cast<QCborTag>(QCborSerializer::MultiMap), QCborMap{
																										  {QStringLiteral("A"), QCborArray{2, 4}},
																										  {QStringLiteral("B"), 6}
																									  }}
									  << QJsonValue{QJsonObject{
											 {QStringLiteral("A"), QJsonArray{2, 4}},
											 {QStringLiteral("B"), 6}
										 }};
	QTest::newRow("unmatchable.array") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
									   << TestQ{}
									   << static_cast<QObject*>(nullptr)
									   << qMetaTypeId<QMultiMap<QString, int>>()
									   << QVariant{}
									   << QCborValue{QCborArray{
											  QCborArray{QStringLiteral("a"), 4, false}
										  }}
									   << QJsonValue{QJsonArray{
											  QJsonArray{QStringLiteral("a"), 4, false}
										  }};
	QTest::newRow("unmatchable.int") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::List)}}
									 << TestQ{}
									 << static_cast<QObject*>(nullptr)
									 << qMetaTypeId<QMultiMap<QString, int>>()
									 << QVariant{}
									 << QCborValue{QCborArray{5}}
									 << QJsonValue{QJsonArray{5}};
	QTest::newRow("unwritable") << QVariantHash{{QStringLiteral("multiMapMode"), QVariant::fromValue(QJsonSerializer::MultiMapMode::Map)}}
								<< TestQ{}
								<< static_cast<QObject*>(nullptr)
								<< qMetaTypeId<QMultiMap<QString, OpaqueDummy>>()
								<< QVariant{}
								<< QCborValue{QCborArray{}}
								<< QJsonValue{QJsonArray{}};
}

QTEST_MAIN(MultiMapConverterTest)

#include "tst_multimapconverter.moc"
