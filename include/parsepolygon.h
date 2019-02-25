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
#ifndef CGPARSE_PARSEPOLYGON_H
#define CGPARSE_PARSEPOLYGON_H
#pragma once

#include "parsegeopoint.h"
#include <QVariant>

namespace cg
{
    class ParsePolygon : public QVariantList
    {
    public:
        ParsePolygon() {}
        ParsePolygon(const ParsePolygon &polygon)
            : QVariantList(polygon) {}

        QList<ParseGeoPoint> coordinates() const
        {
            QList<ParseGeoPoint> coords;
            for (int i = 0; i < size(); i++)
                coords.append(ParseGeoPoint(at(i)));
            return coords;
        }

        void setCoordinates(const QList<ParseGeoPoint> &coords)
        {
            clear();
            for (auto & point : coords)
                append(point);
        }

        bool containsPoint(const ParseGeoPoint &p)
        {
            bool result = false;
            QList<ParseGeoPoint> coords = coordinates();
            int i, j;
            for (i = 0, j = size() - 1; i < size(); j = i++)
            {
                if ((coords.at(i).latitude() > p.latitude()) != (coords.at(j).latitude() > p.latitude()) &&
                    (p.longitude() < (coords.at(j).longitude() - coords.at(i).longitude()) * (p.latitude() - coords.at(i).latitude()) /
                    (coords.at(j).latitude() - coords.at(i).latitude()) + coords.at(i).longitude()))
                {
                    result = !result;
                }
            }

            return result;
        }
    };
}


#endif // CGPARSE_PARSEPOLYGON_H