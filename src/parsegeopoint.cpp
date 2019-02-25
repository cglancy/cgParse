/**
* Copyright 2017 Charles Glancy (charles@glancyfamily.net)
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without restriction, including  without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
* is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
* WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
* COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define _USE_MATH_DEFINES
#include <cmath>

#include "parsegeopoint.h"
#include <QJsonObject>

namespace cg
{
    // static
    bool ParseGeoPoint::isGeoPoint(const QVariant &variant)
    {
        bool geoPoint = false;
        if (variant.canConvert<QVariantMap>())
        {
            QVariantMap map = variant.toMap();
            geoPoint = map.contains(Parse::TypeKey) &&
                map.value(Parse::TypeKey).toString() == Parse::GeoPointValue;
        }

        return geoPoint;
    }

    // static
    bool ParseGeoPoint::isGeoPoint(const QJsonValue &jsonValue)
    {
        bool geoPoint = false;
        if (jsonValue.isObject())
        {
            QJsonObject jsonObject = jsonValue.toObject();
            geoPoint = jsonObject.contains(Parse::TypeKey) &&
                jsonObject.value(Parse::TypeKey).toString() == Parse::GeoPointValue;
        }

        return geoPoint;
    }

    ParseGeoPoint::ParseGeoPoint()
    {
    }

    ParseGeoPoint::ParseGeoPoint(double latitude, double longitude)
    {
        insert(Parse::TypeKey, Parse::GeoPointValue);
        insert(Parse::LatitudeKey, latitude);
        insert(Parse::LongitudeKey, longitude);
    }

    ParseGeoPoint::ParseGeoPoint(const ParseGeoPoint &geoPoint)
        : QVariantMap(geoPoint)
    {
    }

    ParseGeoPoint::ParseGeoPoint(const QVariant & variant)
        : QVariantMap(variant.toMap())
    {
    }

    ParseGeoPoint & ParseGeoPoint::operator=(const ParseGeoPoint &geoPoint)
    {
        if (geoPoint.contains(Parse::LatitudeKey))
            insert(Parse::LatitudeKey, geoPoint.value(Parse::LatitudeKey));
        if (geoPoint.contains(Parse::LongitudeKey))
            insert(Parse::LongitudeKey, geoPoint.value(Parse::LongitudeKey));
        return *this;
    }

    bool ParseGeoPoint::operator==(const ParseGeoPoint &geoPoint) const
    {
        return value(Parse::LatitudeKey) == geoPoint.value(Parse::LatitudeKey) &&
            value(Parse::LongitudeKey) == geoPoint.value(Parse::LongitudeKey);
    }

    bool ParseGeoPoint::operator<(const ParseGeoPoint &geoPoint) const
    {
        if (value(Parse::LatitudeKey) == geoPoint.value(Parse::LatitudeKey))
            return value(Parse::LongitudeKey) < geoPoint.value(Parse::LongitudeKey);
        else
            return value(Parse::LatitudeKey) < geoPoint.value(Parse::LatitudeKey);
    }

    bool ParseGeoPoint::isNull() const
    {
        return isEmpty();
    }

    double ParseGeoPoint::latitude() const
    {
        return value(Parse::LatitudeKey).toDouble();
    }

    void ParseGeoPoint::setLatitude(double latitude)
    {
        insert(Parse::TypeKey, Parse::GeoPointValue);
        insert(Parse::LatitudeKey, latitude);
    }

    double ParseGeoPoint::longitude() const
    {
        return value(Parse::LongitudeKey).toDouble();
    }

    void ParseGeoPoint::setLongitude(double longitude)
    {
        insert(Parse::TypeKey, Parse::GeoPointValue);
        insert(Parse::LongitudeKey, longitude);
    }

    double ParseGeoPoint::distanceInKilometersTo(const ParseGeoPoint &geoPoint) const
    {
        static const double earthRadiusKm = 6371.;
        return earthRadiusKm * distanceInRadiansTo(geoPoint);
    }

    double ParseGeoPoint::distanceInMilesTo(const ParseGeoPoint &geoPoint) const
    {
        static const double earthRadiusMiles = 3959.;
        return earthRadiusMiles * distanceInRadiansTo(geoPoint);
    }

    // https://en.wikipedia.org/wiki/Haversine_formula
    double ParseGeoPoint::distanceInRadiansTo(const ParseGeoPoint &geoPoint) const
    {
        double lat1r = latitude() * M_PI / 180.;
        double lon1r = longitude() * M_PI / 180.;
        double lat2r = geoPoint.latitude() * M_PI / 180.;
        double lon2r = geoPoint.longitude() * M_PI / 180.;
        double u = sin((lat2r - lat1r) / 2.);
        double v = sin((lon2r - lon1r) / 2.);
        return 2. * asin(sqrt(u * u + cos(lat1r) * cos(lat2r) * v * v));
    }
}