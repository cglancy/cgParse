#include "parseconvert.h"
#include "parseobject.h"
#include "parsefile.h"

namespace cg
{
    QJsonObject ParseConvert::toJsonObject(const QVariantMap &map)
    {
        return QJsonObject::fromVariantMap(convertMapToJson(map));
    }

    QVariantMap ParseConvert::toVariantMap(const QJsonObject &object)
    {
        return convertMap(object.toVariantMap());
    }

    QVariantMap ParseConvert::toVariantMap(const ParseObject& object)
    {
        return object.toPointer().toMap(); 
    }

    QVariantMap ParseConvert::convertMap(const QVariantMap &map)
    {
        QVariantMap convertedMap = map;

        for (auto & key : map.keys())
        {
            QVariant variant = map.value(key);

            if (canConvert(variant))
                convertedMap[key] = convertVariant(variant);
            else if (variant.typeId() == QMetaType::QVariantMap)
                convertedMap[key] = convertMap(variant.toMap());
            else if (variant.typeId() == QMetaType::QVariantList)
                convertedMap[key] = convertList(variant.toList());
        }

        return convertedMap;
    }

    QVariantMap ParseConvert::convertMapToJson(const QVariantMap& map)
    {
       QVariantMap convertedMap = map;

       for (auto& key : map.keys())
       {
          QVariant variant = map.value(key);

          if (isReadOnlyKey(key))
             convertedMap.remove(key);
          else if (canConvertToJson(variant))
             convertedMap[key] = convertVariantToJson(variant);
          else if (variant.typeId() == QMetaType::QVariantMap)
             convertedMap[key] = convertMapToJson(variant.toMap());
          else if (variant.typeId() == QMetaType::QVariantList)
             convertedMap[key] = convertListToJson(variant.toList());
       }

       return convertedMap;
    }

    QVariantList ParseConvert::convertList(const QVariantList &list)
    {
        QVariantList convertedList = list;

        for (int i = 0; i < list.size(); i++)
        {
            QVariant variant = list.at(i);

            if (canConvert(variant))
                convertedList[i] = convertVariant(variant);
            else if (variant.canConvert<QVariantMap>())
                convertedList[i] = convertMap(variant.toMap());
            else if (variant.canConvert<QVariantList>())
                convertedList[i] = convertList(variant.toList());
        }

        return convertedList;
    }

    QVariantList ParseConvert::convertListToJson(const QVariantList& list)
    {
       QVariantList convertedList = list;

       for (int i = 0; i < list.size(); i++)
       {
          QVariant variant = list.at(i);

          if (canConvertToJson(variant))
             convertedList[i] = convertVariantToJson(variant);
          else if (variant.canConvert<QVariantMap>())
             convertedList[i] = convertMapToJson(variant.toMap());
          else if (variant.canConvert<QVariantList>())
             convertedList[i] = convertListToJson(variant.toList());
       }

       return convertedList;
    }

    bool ParseConvert::canConvert(const QVariant &variant)
    {
        return isObject(variant) || isPointer(variant) || isFile(variant);
    }

    bool ParseConvert::canConvertToJson(const QVariant& variant)
    {
       return variant.canConvert<ParseFile>() || variant.canConvert<ParseObject>();
    }

    QVariant ParseConvert::convertVariant(const QVariant &variant)
    {
         QVariant convertedVariant = variant;

         if (isObject(variant) || isPointer(variant))
         {
               ParseObject object = toObject(variant);
               convertedVariant = QVariant::fromValue(object);
         }
         else if (isFile(variant))
         {
               ParseFile file = fileFromVariant(variant);
               convertedVariant = QVariant::fromValue(file);
         }

         return convertedVariant;
    }

    QVariant ParseConvert::convertVariantToJson(const QVariant& variant)
    {
       QVariant convertedVariant = variant;

       if (variant.canConvert<ParseObject>())
       {
          ParseObject object = variant.value<ParseObject>();
          if (!object.isNull())
             convertedVariant = object.toPointer().toMap();
          else
             convertedVariant = QVariant();
       }
       else if (variant.canConvert<ParseFile>())
       {
          ParseFile file = variant.value<ParseFile>();
          convertedVariant = file.toMap();
       }

       return convertedVariant;
    }

    ParseObject ParseConvert::toObject(const QVariant &variant)
    {
        ParseObject object;

        if (variant.canConvert<QVariantMap>())
        {
            // convert sub-objects
            QVariantMap map = convertMap(variant.toMap());

            if (map.contains(Parse::TypeKey) && map.value(Parse::TypeKey).toString() == Parse::PointerValue)
            {
                QString className = map.value(Parse::ClassNameKey).toString();
                QString objectId = map.value(Parse::ObjectIdKey).toString();
                object = ParseObject::createWithoutData(className, objectId);
            }
            else if (map.contains(Parse::TypeKey) && map.value(Parse::TypeKey).toString() == Parse::ObjectValue)
            {
                QString className = map.value(Parse::ClassNameKey).toString();
                object = ParseObject::create(className);
                object.setValues(map);
                object.clearDirtyState();
            }
        }

        return object;
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

    ParseFile ParseConvert::fileFromVariant(const QVariant &variant)
    {
        ParseFile file;

        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            if (map.contains(Parse::TypeKey) && map.value(Parse::TypeKey).toString() == Parse::FileValue)
            {
                if (map.contains("name") && map.contains("url"))
                {
                    QString name = map.value("name").toString();
                    QString url = map.value("url").toString();
                    file = ParseFile(name, url);
                }
            }
        }

        return file;
    }

    bool ParseConvert::isReadOnlyKey(const QString & key)
    {
        return key == Parse::CreatedAtKey ||
            key == Parse::UpdatedAtKey ||
            key == Parse::EmailVerifiedKey ||
            key == Parse::PerishableTokenKey;
    }
}
