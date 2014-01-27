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

#ifndef _OSMAND_CORE_RASTERIZER_CONTEXT_P_H_
#define _OSMAND_CORE_RASTERIZER_CONTEXT_P_H_

#include <OsmAndCore/stdlib_common.h>

#include <OsmAndCore/QtExtensions.h>
#include <QList>
#include <QVector>

#include <SkColor.h>

#include <OsmAndCore.h>
#include <CommonTypes.h>
#include <Rasterizer_P.h>

namespace OsmAnd {

    class Rasterizer_P;

    class RasterizerContext;
    class RasterizerContext_P
    {
    private:
    protected:
        RasterizerContext_P(RasterizerContext* owner);

        RasterizerContext* const owner;

        SkColor _defaultBgColor;
        int _shadowRenderingMode;
        SkColor _shadowRenderingColor;
        double _polygonMinSizeToDisplay;
        uint32_t _roadDensityZoomTile;
        uint32_t _roadsDensityLimitPerTile;

        AreaI _area31;
        ZoomLevel _zoom;
        double _tileDivisor;
        uint32_t _shadowLevelMin;
        uint32_t _shadowLevelMax;

        QVector< std::shared_ptr<const Rasterizer_P::PrimitivesGroup> > _primitivesGroups;
        QVector< std::shared_ptr<const Rasterizer_P::Primitive> > _polygons, _polylines, _points;

        QVector< std::shared_ptr<const Rasterizer_P::SymbolsGroup> > _symbolsGroups;
        typedef std::pair<
            std::shared_ptr<const Model::MapObject>,
            QVector< std::shared_ptr<const Rasterizer_P::PrimitiveSymbol> >
        > SymbolsEntry;
        QVector<SymbolsEntry> _symbols;

        void clear();
    public:
        virtual ~RasterizerContext_P();

    friend class OsmAnd::RasterizerContext;
    friend class OsmAnd::Rasterizer_P;
    };

} // namespace OsmAnd

#endif // _OSMAND_CORE_RASTERIZER_CONTEXT_P_H_
