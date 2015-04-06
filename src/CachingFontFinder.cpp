#include "CachingFontFinder.h"

#include <SkTypeface.h>

OsmAnd::CachingFontFinder::CachingFontFinder(const std::shared_ptr<const IFontFinder>& fontFinder_)
    : fontFinder(fontFinder_)
{
}

OsmAnd::CachingFontFinder::~CachingFontFinder()
{
    QMutexLocker scopedLocker(&_lock);

    for (const auto& font : constOf(_cache))
        font->unref();
}

SkTypeface* OsmAnd::CachingFontFinder::findFontForCharacterUCS4(
    const uint32_t character,
    const SkFontStyle style /*= SkFontStyle()*/) const
{
    QMutexLocker scopedLocker(&_lock);

    CacheKey cacheKey;
    cacheKey.styleId = *reinterpret_cast<const StyleId*>(&style);
    cacheKey.character = character;

    const auto citFont = _cache.constFind(cacheKey);
    if (citFont != _cache.cend())
    {
        const auto font = *citFont;
        if (font)
            font->ref();
        return font;
    }

    const auto font = fontFinder->findFontForCharacterUCS4(character, style);
    _cache.insert(cacheKey, font);
    if (font)
        font->ref();
    return font;
}
