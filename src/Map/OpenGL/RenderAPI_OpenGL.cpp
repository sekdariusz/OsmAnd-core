#include "RenderAPI_OpenGL.h"

#include <assert.h>

#include <SkBitmap.h>

#include "IMapBitmapTileProvider.h"
#include "Logging.h"

#undef GL_CHECK_RESULT
#undef GL_GET_RESULT
#if defined(_DEBUG) || defined(DEBUG)
#   define GL_CHECK_RESULT validateResult()
#   define GL_GET_RESULT validateResult()
#else
#   define GL_CHECK_RESULT
#   define GL_GET_RESULT glGetError()
#endif

OsmAnd::RenderAPI_OpenGL::RenderAPI_OpenGL()
{
    _textureSamplers.fill(0);
}

OsmAnd::RenderAPI_OpenGL::~RenderAPI_OpenGL()
{
}

GLenum OsmAnd::RenderAPI_OpenGL::validateResult()
{
    GL_CHECK_PRESENT(glGetError);

    auto result = glGetError();
    if(result == GL_NO_ERROR)
        return result;

    LogPrintf(LogSeverityLevel::Error, "OpenGL error 0x%08x : %s\n", result, gluErrorString(result));

    return result;
}

bool OsmAnd::RenderAPI_OpenGL::initialize()
{
    bool ok;

    ok = RenderAPI_OpenGL_Common::initialize();
    if(!ok)
        return false;

    glewExperimental = GL_TRUE;
    glewInit();
    // For now, silence OpenGL error here, it's inside GLEW, so it's not ours
    (void)glGetError();
    //GL_CHECK_RESULT;

    GL_CHECK_PRESENT(glGetError);
    GL_CHECK_PRESENT(glGetString);
    GL_CHECK_PRESENT(glGetFloatv);
    GL_CHECK_PRESENT(glGetIntegerv);
    GL_CHECK_PRESENT(glGenSamplers);
    GL_CHECK_PRESENT(glSamplerParameteri);
    GL_CHECK_PRESENT(glSamplerParameterf);
    GL_CHECK_PRESENT(glShadeModel);
    GL_CHECK_PRESENT(glHint);
    GL_CHECK_PRESENT(glEnable);
    GL_CHECK_PRESENT(glClearColor);
    GL_CHECK_PRESENT(glClearDepth);
    GL_CHECK_PRESENT(glDepthFunc);
    
    const auto glVersionString = glGetString(GL_VERSION);
    GL_CHECK_RESULT;
    GLint glVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &glVersion[0]);
    GL_CHECK_RESULT;
    glGetIntegerv(GL_MINOR_VERSION, &glVersion[1]);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "Using OpenGL version %d.%d [%s]\n", glVersion[0], glVersion[1], glVersionString);
    assert(glVersion[0] >= 3);

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, reinterpret_cast<GLint*>(&_maxTextureSize));
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture size %dx%d\n", _maxTextureSize, _maxTextureSize);

    GLint maxTextureUnitsInFragmentShader;
    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &maxTextureUnitsInFragmentShader);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture units in fragment shader %d\n", maxTextureUnitsInFragmentShader);
    assert(maxTextureUnitsInFragmentShader >= (MapTileLayerIdsCount - MapTileLayerId::RasterMap));

    GLint maxTextureUnitsInVertexShader;
    glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &maxTextureUnitsInVertexShader);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture units in vertex shader %d\n", maxTextureUnitsInVertexShader);
    _isSupported_vertexShaderTextureLookup = (maxTextureUnitsInVertexShader >= MapTileLayerId::RasterMap);

    GLint maxTextureUnitsCombined;
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &maxTextureUnitsCombined);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal texture units (combined) %d\n", maxTextureUnitsCombined);

    GLint maxUniformsPerProgram;
    glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &maxUniformsPerProgram);
    GL_CHECK_RESULT;
    LogPrintf(LogSeverityLevel::Info, "OpenGL maximal parameter variables per program %d\n", maxUniformsPerProgram);

    // This is always true for GLSL 4.3
    _isSupported_shaderTextureLOD = true;

    // Allocate samplers
    glGenSamplers(SamplerTypesCount, _textureSamplers.data());
    GL_CHECK_RESULT;
    GLuint sampler;

    // ElevationDataTile sampler
    sampler = _textureSamplers[SamplerType::ElevationDataTile];
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    GL_CHECK_RESULT;

    // BitmapTile_Bilinear sampler
    sampler = _textureSamplers[SamplerType::BitmapTile_Bilinear];
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_RESULT;

    // BitmapTile_BilinearMipmap sampler
    sampler = _textureSamplers[SamplerType::BitmapTile_BilinearMipmap];
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_RESULT;

    // BitmapTile_TrilinearMipmap sampler
    sampler = _textureSamplers[SamplerType::BitmapTile_TrilinearMipmap];
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    GL_CHECK_RESULT;
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    GL_CHECK_RESULT;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    GL_CHECK_RESULT;

    glClearDepth(1.0f);
    GL_CHECK_RESULT;

    glEnable(GL_DEPTH_TEST);
    GL_CHECK_RESULT;

    glDepthFunc(GL_LEQUAL);
    GL_CHECK_RESULT;

    glEnable(GL_BLEND);
    GL_CHECK_RESULT;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    GL_CHECK_RESULT;

    return true;
}

bool OsmAnd::RenderAPI_OpenGL::release()
{
    bool ok;

    GL_CHECK_PRESENT(glDeleteSamplers);

    if(_textureSamplers[0] != 0)
    {
        glDeleteSamplers(1, _textureSamplers.data());
        GL_CHECK_RESULT;
        _textureSamplers.fill(0);
    }
    
    ok = RenderAPI_OpenGL_Common::release();
    if(!ok)
        return false;

    return true;
}

uint32_t OsmAnd::RenderAPI_OpenGL::getTileTextureFormat( const std::shared_ptr< IMapTileProvider::Tile >& tile )
{
    GLenum textureFormat = GL_INVALID_ENUM;

    if(tile->type == IMapTileProvider::Bitmap)
    {
        auto bitmapTile = static_cast<IMapBitmapTileProvider::Tile*>(tile.get());

        switch (bitmapTile->bitmap->getConfig())
        {
        case SkBitmap::Config::kARGB_8888_Config:
            textureFormat = GL_RGBA8;
            break;
        case SkBitmap::Config::kARGB_4444_Config:
            textureFormat = GL_RGBA4;
            break;
        case SkBitmap::Config::kRGB_565_Config:
            textureFormat = GL_RGB565;
            break;
        }
    }
    else if(tile->type == IMapTileProvider::ElevationData)
    {
        textureFormat = GL_R32F;
    }

    assert(textureFormat != GL_INVALID_ENUM);

    return static_cast<uint32_t>(textureFormat);
}

void OsmAnd::RenderAPI_OpenGL::allocateTexture2D( GLenum target, GLsizei levels, GLsizei width, GLsizei height, const std::shared_ptr< IMapTileProvider::Tile >& forTile )
{
    GLenum textureFormat = static_cast<GLenum>(getTileTextureFormat(forTile));

    glTexStorage2D(target, levels, textureFormat, width, height);
    GL_CHECK_RESULT;
}

void OsmAnd::RenderAPI_OpenGL::uploadDataToTexture2D(
    GLenum target, GLint level,
    GLint xoffset, GLint yoffset, GLsizei width, GLsizei height,
    const GLvoid *data, GLsizei dataRowLengthInElements,
    const std::shared_ptr< IMapTileProvider::Tile >& tile )
{
    GL_CHECK_PRESENT(glPixelStorei);
    GL_CHECK_PRESENT(glTexSubImage2D);

    GLenum sourceFormat = GL_INVALID_ENUM;
    GLenum sourceFormatType = GL_INVALID_ENUM;
    if(tile->type == IMapTileProvider::Bitmap)
    {
        auto bitmapTile = static_cast<IMapBitmapTileProvider::Tile*>(tile.get());

        switch (bitmapTile->bitmap->getConfig())
        {
        case SkBitmap::Config::kARGB_8888_Config:
            sourceFormat = GL_RGBA;
            sourceFormatType = GL_UNSIGNED_BYTE;
            break;
        case SkBitmap::Config::kARGB_4444_Config:
            sourceFormat = GL_RGBA;
            sourceFormatType = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case SkBitmap::Config::kRGB_565_Config:
            sourceFormat = GL_RGB;
            sourceFormatType = GL_UNSIGNED_SHORT_5_6_5;
            break;
        }
    }
    else if(tile->type == IMapTileProvider::ElevationData)
    {
        sourceFormat = GL_RED;
        sourceFormatType = GL_FLOAT;
    }

    glPixelStorei(GL_UNPACK_ROW_LENGTH, dataRowLengthInElements);
    GL_CHECK_RESULT;

    glTexSubImage2D(target, level,
        xoffset, yoffset, width, height,
        sourceFormat,
        sourceFormatType,
        data);
    GL_CHECK_RESULT;
}

void OsmAnd::RenderAPI_OpenGL::setMipMapLevelsLimit( GLenum target, const uint32_t& mipmapLevelsCount )
{
    GL_CHECK_PRESENT(glTexParameteri);

    glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, mipmapLevelsCount);
    GL_CHECK_RESULT;
}

void OsmAnd::RenderAPI_OpenGL::glGenVertexArrays_wrapper( GLsizei n, GLuint* arrays )
{
    GL_CHECK_PRESENT(glGenVertexArrays);

    glGenVertexArrays(n, arrays);
}

void OsmAnd::RenderAPI_OpenGL::glBindVertexArray_wrapper( GLuint array )
{
    GL_CHECK_PRESENT(glBindVertexArray);

    glBindVertexArray(array);
}

void OsmAnd::RenderAPI_OpenGL::glDeleteVertexArrays_wrapper( GLsizei n, const GLuint* arrays )
{
    GL_CHECK_PRESENT(glDeleteVertexArrays);

    glDeleteVertexArrays(n, arrays);
}

void OsmAnd::RenderAPI_OpenGL::preprocessShader( QString& code )
{
    const auto& shaderSource = QString::fromLatin1(
        // Declare version of GLSL used
        "#version 430 core                                                                                                  ""\n"
        "                                                                                                                   ""\n"
        // General definitions
        "#define INPUT in                                                                                                   ""\n"
        "#define PARAM_OUTPUT out                                                                                           ""\n"
        "#define PARAM_INPUT in                                                                                             ""\n"
        "                                                                                                                   ""\n"
        // Features definitions
        "#define MIPMAPS_SUPPORTED 1                                                                                        ""\n"
        "#define VERTEX_TEXTURE_FETCH_SUPPORTED 1                                                                           ""\n"
        "#define SAMPLE_TEXTURE texture                                                                                     ""\n"
        "#define SAMPLE_TEXTURE_LOD textureLod                                                                              ""\n");

    code.prepend(shaderSource);
}

void OsmAnd::RenderAPI_OpenGL::preprocessVertexShader( QString& code )
{
    preprocessShader(code);
}

void OsmAnd::RenderAPI_OpenGL::preprocessFragmentShader( QString& code )
{
    QString common;
    preprocessShader(common);

    const auto& shaderSource = QString::fromLatin1(
        // Fragment shader output declaration
        "#define FRAGMENT_COLOR_OUTPUT out_FragColor                                                                        ""\n"
        "out vec4 out_FragColor;                                                                                            ""\n");

    code.prepend(shaderSource);
    code.prepend(common);
}

void OsmAnd::RenderAPI_OpenGL::setSampler( GLenum texture, const SamplerType& samplerType )
{
    GL_CHECK_PRESENT(glBindSampler);

    glBindSampler(texture - GL_TEXTURE0, _textureSamplers[samplerType]);
    GL_CHECK_RESULT;
}
