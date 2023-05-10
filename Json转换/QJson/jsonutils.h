/*
 * @Description: QJson的用法：json格式转换
 * @Author: Ivy
 * @Date: 2022-05-06 09:34:43
 * @LastEditTime: 2022-05-06 11:28:29
 * @LastEditors: XTZJ-2022OQEHLZ
 */

#pragma once
#include <QString>
#include <QJsonObject>

namespace JsonUtils
{
	// json字符串 与 QJsonObject对象
	QJsonObject getJsonObjectFromString(const QString &jsonString);
	QString getStringFromJsonObject(const QJsonObject &jsonObject);

	// json字符串 与 QJsonArray对象
	QJsonArray getJsonArrayFromQString(const QString jsonString);
	QString getStringFromJsonArray(const QJsonArray &array);

	// QByteArray 与 QJsonArray对象
	QJsonObject getJsonObjectFromByteArray(const QByteArray &byteArray);
	QByteArray getByteArrayFromJsonObject(const QJsonObject &jsonObject);

	// 判断key是否存在
	bool jsonValIsNullOrNonExists(const QJsonObject &jsonObj, const QString &key);

	// 在json中插入QVariant数据
	bool insertValToJsonObjectWithCheckValid(QJsonObject &json, const QString& key, QVariant val);

	// 判断当前的值不存在，就添加
	bool fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, bool &target);
	bool fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, int &target);
	bool fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, double &target);
	bool fillValToTargetWithCheckValid(const QJsonObject& json, const QString& key, QString &target);
};