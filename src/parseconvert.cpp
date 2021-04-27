#include "parseconvert.h"
#include "parseobject.h"
#include "parsefile.h"

namespace cg
{
    QJsonObject ParseConvert::toJsonObject(const QVariantMap &map)
    {
        return QJsonObject::fromVariantMap(convertMap(map, true));
    }

    QVariantMap ParseConvert::toVariantMap(const QJsonObject &object)
    {
        return convertMap(object.toVariantMap(), false);
    }

    QVariantMap ParseConvert::convertMap(const QVariantMap &map, bool toJson)
    {
        QVariantMap convertedMap = map;

        for (auto & key : map.keys())
        {
            QVariant variant = map.value(key);

            if (toJson && isReadOnlyKey(key))
                convertedMap.remove(key);
            else if (canConvert(variant, toJson))
                convertedMap[key] = convertVariant(variant, toJson);
            else if (variant.canConvert<QVariantMap>())
                convertedMap[key] = convertMap(variant.toMap(), toJson);
            else if (variant.canConvert<QVariantList>())
                convertedMap[key] = convertList(variant.toList(), toJson);
        }

        return convertedMap;
    }

    QVariantList ParseConvert::convertList(const QVariantList &list, bool toJson)
    {
        QVariantList convertedList = list;

        for (int i = 0; i < list.size(); i++)
        {
            QVariant variant = list.at(i);

            if (canConvert(variant, toJson))
                convertedList[i] = convertVariant(variant, toJson);
            else if (variant.canConvert<QVariantMap>())
                convertedList[i] = convertMap(variant.toMap(), toJson);
            else if (variant.canConvert<QVariantList>())
                convertedList[i] = convertList(variant.toList(), toJson);
        }

        return convertedList;
    }

    bool ParseConvert::canConvert(const QVariant &variant, bool toJson)
    {
        bool canConvert = false;
        if (toJson)
            canConvert = variant.canConvert<ParseFilePtr>() || variant.canConvert<ParseObjectPtr>();
        else
            canConvert = isObject(variant) || isPointer(variant) || isFile(variant);
        return canConvert;
    }

    QVariant ParseConvert::convertVariant(const QVariant &variant, bool toJson)
    {
        QVariant convertedVariant = variant;
        if (toJson)
        {
            if (variant.canConvert<ParseObjectPtr>())
            {
                ParseObjectPtr pObject = variant.value<ParseObjectPtr>();
                if (pObject)
                    convertedVariant = pObject->toPointer().toMap();
                else
                    convertedVariant = QVariant();
            }
            else if (variant.canConvert<ParseFilePtr>())
            {
                ParseFilePtr pFile = variant.value<ParseFilePtr>();
                convertedVariant = pFile->toMap();
            }
        }
        else
        {
            if (isObject(variant) || isPointer(variant))
            {
                ParseObjectPtr pObject = objectFromVariant(variant);
                convertedVariant = QVariant::fromValue(pObject);
            }
            else if (isFile(variant))
            {
                ParseFilePtr pFile = fileFromVariant(variant);
                convertedVariant = QVariant::fromValue(pFile);
            }
        }

        return convertedVariant;
    }

    ParseObjectPtr ParseConvert::objectFromVariant(const QVariant &variant)
    {
        ParseObjectPtr pObject;

        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            if (map.contains(Parse::TypeKey) && map.value(Parse::TypeKey).toString() == Parse::PointerValue)
            {
                QString className = map.value(Parse::ClassNameKey).toString();
                QString objectId = map.value(Parse::ObjectIdKey).toString();
                pObject = ParseObject::createWithoutData(className, objectId);
            }
            else if (map.contains(Parse::TypeKey) && map.value(Parse::TypeKey).toString() == Parse::ObjectValue)
            {
                QString className = map.value(Parse::ClassNameKey).toString();
                pObject = ParseObject::create(className);
                pObject->setValues(map);
                pObject->clearDirtyState();
            }
        }

        return pObject;
    }

    bool ParseConvert::isPointer(const QVariant &variant)
    {
        bool pointer = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            pointer = map.contains(Parse::TypeKey) &&
                map.value(Parse::TypeKey).toString() == Parse::PointerValue;
        }

        return pointer;
    }

    bool ParseConvert::isObject(const QVariant &variant)
    {
        bool object = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            object = map.contains(Parse::TypeKey) &&
                map.value(Parse::TypeKey).toString() == Parse::ObjectValue;
        }

        return object;
    }

    bool ParseConvert::isFile(const QVariant &variant)
    {
        bool file = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            file = map.contains(Parse::TypeKey) &&
                map.value(Parse::TypeKey).toString() == Parse::FileValue;
        }

        return file;
    }

    ParseFilePtr ParseConvert::fileFromVariant(const QVariant &variant)
    {
        ParseFilePtr pFile;

        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            if (map.contains(Parse::TypeKey) && map.value(Parse::TypeKey).toString() == Parse::FileValue)
            {
                pFile = ParseFile::create();
                if (map.contains("name"))
                    pFile->setName(map.value("name").toString());

                if (map.contains("url"))
                    pFile->setUrl(map.value("url").toString());
            }
        }

        return pFile;
    }

    bool ParseConvert::isReadOnlyKey(const QString & key)
    {
        return key == Parse::CreatedAtKey ||
            key == Parse::UpdatedAtKey ||
            key == Parse::EmailVerifiedKey ||
            key == Parse::PerishableTokenKey;
    }
}