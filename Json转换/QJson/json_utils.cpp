#include "pch.h"
#include "json_utils.h"
#include "genericdef.h"

QJsonObject JsonUtils::getJsonObjectFromString(const QString &jsonString)
{
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());
	if (jsonDocument.isNull())
	{
		return QJsonObject();
	}
	QJsonObject jsonObject = jsonDocument.object();
	return jsonObject;
}

QString JsonUtils::getStringFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.isEmpty())
		return QString("");
	return QString(QJsonDocument(jsonObject).toJson());
}

QJsonArray JsonUtils::getJsonArrayFromQString(const QString jsonString)
{
	QJsonDocument jsonDocument = QJsonDocument::fromJson(jsonString.toUtf8());
	if (jsonDocument.isNull())
	{
		return QJsonArray();
	}
	QJsonArray jsonArray = jsonDocument.array();
	return jsonArray;
}

QString JsonUtils::getStringFromJsonArray(const QJsonArray &array)
{
	if (array.isEmpty())
		return QString();
	return QJsonDocument(array).toJson(QJsonDocument::Compact);
}

QJsonObject JsonUtils::getJsonObjectFromByteArray(const QByteArray &byteArray)
{
	QJsonDocument jsonDocument = QJsonDocument::fromJson(byteArray);
	if (jsonDocument.isNull()) {
		return QJsonObject();
	}
	QJsonObject jsonObject = jsonDocument.object();
	return jsonObject;
}

QByteArray JsonUtils::getByteArrayFromJsonObject(const QJsonObject& jsonObject)
{
	if (jsonObject.isEmpty())
		return QByteArray();
	return QJsonDocument(jsonObject).toJson(QJsonDocument::Compact);
}

QString JsonUtils::JsonArrayToString(QJsonArray jsonArray)
{
	QJsonDocument jsonDoc;
	jsonDoc.setArray(jsonArray);
	return QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
}

QString JsonUtils::JsonObjectToString(QJsonObject jsonObj)
{
	QJsonDocument jsonDoc;
	jsonDoc.setObject(jsonObj);
	return QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));
}

bool JsonUtils::jsonValIsNullOrNonExists(const QJsonObject &jsonObj, const QString &key)
{
	if (!jsonObj.contains(key))
		return true;
	if (jsonObj.value(key).isNull())
		return true;
	if (jsonObj.value(key).isString() && jsonObj.value(key).toString().isEmpty())
		return true;
	return false;
}

bool JsonUtils::insertValToJsonObjectWithCheckValid(QJsonObject &json, const QString& key, QVariant val)
{
	if (!val.isValid())
		return false;
	if (val.type() == QVariant::String && !val.toString().isEmpty())
	{
		json[key] = val.toString();
		return true;
	}

	if (val.type() == QVariant::Int && ISVALID(val.toInt()))
	{
		json[key] = val.toInt();
		return true;
	}
	if (val.type() == QVariant::Double && ISVALID(val.toDouble()))
	{
		json[key] = val.toDouble();
		return true;
	}
	if (val.type() == QMetaType::QJsonObject && !val.toJsonObject().isEmpty())
	{
		json[key] = val.toJsonObject();
		return true;
	}
	if (val.type() == QMetaType::QJsonArray && !val.toJsonArray().isEmpty())
	{
		json[key] = val.toJsonArray();
		return true;
	}
	return false;
}

bool JsonUtils::fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, double &target)
{
	if (json.isEmpty() || key.isEmpty())
		return false;
	if (!jsonValIsNullOrNonExists(json, key) && json[key].toVariant().type() == QVariant::Double)
	{
		target = json[key].toDouble();
		return true;
	}
	return false;
}

bool JsonUtils::fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, int &target)
{
	if (json.isEmpty() || key.isEmpty())
		return false;
	if (!jsonValIsNullOrNonExists(json, key) && json[key].toVariant().type() == QVariant::Int)
	{
		target = json[key].toInt();
		return true;
	}
	return false;
}

bool JsonUtils::fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, QString &target)
{
	if (json.isEmpty() || key.isEmpty())
		return false;
	if (!jsonValIsNullOrNonExists(json, key) && json[key].toVariant().type() == QVariant::String)
	{
		target = json[key].toString();
		return true;
	}
	return false;
}

bool JsonUtils::fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, bool &target)
{
	if (json.isEmpty() || key.isEmpty())
		return false;
	if (!jsonValIsNullOrNonExists(json, key) && json[key].toVariant().type() == QVariant::Bool)
	{
		target = json[key].toBool();
		return true;
	}
	return false;
}

