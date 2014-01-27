/**
* @file
*
* @section LICENSE
*
* OsmAnd - Android navigation software based on OSM maps.
* Copyright (C) 2010-2014  OsmAnd Authors listed in AUTHORS file
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _OSMAND_CORE_ROUTING_CONFIGURATION_PRIVATE_H_
#define _OSMAND_CORE_ROUTING_CONFIGURATION_PRIVATE_H_

namespace OsmAnd {

    struct RoutingRule
    {
        QString _tagName;
        QString _t;
        QString _v;
        QString _param;
        QString _value1;
        QString _value2;
        QString _type;
    };

} // namespace OsmAnd

#endif // _OSMAND_CORE_ROUTING_CONFIGURATION_PRIVATE_H_
