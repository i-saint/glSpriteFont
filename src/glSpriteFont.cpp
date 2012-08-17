#include "stdafx.h"
#include "Image.h"

#define glIFR_InterModule __declspec(dllexport)
#include "glSpriteFont.h"

namespace ist {

enum I3D_COLOR_FORMAT
{
    I3D_COLOR_UNKNOWN,

    //     CPU  ->    GPU
    I3D_R8,     //    0-255 ->  0.0f-1.0f
    I3D_R8S,    // -128-127 -> -1.0f-1.0f
    I3D_R8U,    //    0-255 ->     0-255
    I3D_R8I,    // -128-127 ->  -128-127
    I3D_R16F,
    I3D_R32F,
    I3D_RG8,
    I3D_RG8S,
    I3D_RG8U,
    I3D_RG8I,
    I3D_RG16F,
    I3D_RG32F,
    I3D_RGB8,
    I3D_RGB8S,
    I3D_RGB8U,
    I3D_RGB8I,
    I3D_RGB16F,
    I3D_RGB32F,
    I3D_RGBA8,
    I3D_RGBA8S,
    I3D_RGBA8U,
    I3D_RGBA8I,
    I3D_RGBA16F,
    I3D_RGBA32F,
    I3D_DEPTH16F,
    I3D_DEPTH32F,
    I3D_DEPTH24_STENCIL8,
    I3D_DEPTH32F_STENCIL8,
    I3D_RGB_DXT1,
    I3D_SRGB_DXT1,
    I3D_RGBA_DXT1,
    I3D_SRGBA_DXT1,
    I3D_RGBA_DXT3,
    I3D_SRGBA_DXT3,
    I3D_RGBA_DXT5,
    I3D_SRGBA_DXT5,
};
struct VertexDesc
{
    GLuint location;        // shader value location
    GLuint type;          // value type
    GLuint num_elements;    // must be 1,2,3,4
    GLuint offset;
    bool normalize;
    GLuint divisor; // 0: per vertex, other: per n instance
};

struct BufferDesc
{
    GLuint type;
    GLuint usage;
    uint32 size;
    void *data;

    // data は NULL でもよく、その場合メモリ確保だけが行われる。
    explicit BufferDesc(GLuint _type, GLuint _usage=GL_DYNAMIC_DRAW, uint32 _size=0, void *_data=NULL)
        : type(_type)
        , usage(_usage)
        , size(_size)
        , data(_data)
    {}
};

struct SamplerDesc
{
    GLuint wrap_s;
    GLuint wrap_t;
    GLuint wrap_r;
    GLuint filter_min;
    GLuint filter_mag;

    explicit SamplerDesc(
        GLuint _s=GL_CLAMP_TO_EDGE, GLuint _t=GL_CLAMP_TO_EDGE, GLuint _r=GL_CLAMP_TO_EDGE,
        GLuint _min=GL_LINEAR, GLuint _mag=GL_LINEAR)
        : wrap_s(_s), wrap_t(_t), wrap_r(_r)
        , filter_min(_min), filter_mag(_mag)
    {}
};

struct Texture2DDesc
{
    I3D_COLOR_FORMAT format;
    uvec2 size;
    uint32 mipmap;
    void *data;
    uint32 datasize;

    explicit Texture2DDesc(I3D_COLOR_FORMAT _format=I3D_RGBA8, uvec2 _size=uvec2(0, 0), uint32 _mipmap=0, void *_data=NULL, uint32 _datasize=0)
        : format(_format)
        , size(_size)
        , mipmap(_mipmap)
        , data(_data)
        , datasize(_datasize)
    {}
};

struct ShaderDesc
{
    const char *source;
    uint32 source_len;

    explicit ShaderDesc(const char *s=NULL, uint32 l=0) : source(s), source_len(l) {}
};
typedef ShaderDesc VertexShaderDesc;
typedef ShaderDesc PixelShaderDesc;

class VertexShader;
class PixelShader;
struct ShaderProgramDesc
{
    VertexShader    *vs;
    PixelShader     *ps;

    explicit ShaderProgramDesc(VertexShader *v=NULL, PixelShader *p=NULL)
        : vs(v), ps(p)
    {}
};



bool DetectGLFormat(GLint fmt, GLint &internal_format, GLint &format, GLint &type)
{
    switch(fmt)
    {
    case I3D_R8:        internal_format=GL_R8;          format=GL_RED;  type=GL_UNSIGNED_BYTE;  break;
    case I3D_R8S:       internal_format=GL_R8_SNORM;    format=GL_RED;  type=GL_BYTE;           break;
    case I3D_R8U:       internal_format=GL_R8UI;        format=GL_RED;  type=GL_UNSIGNED_BYTE;  break;
    case I3D_R8I:       internal_format=GL_R8I;         format=GL_RED;  type=GL_BYTE;           break;
    case I3D_R16F:      internal_format=GL_R16F;        format=GL_RED;  type=GL_FLOAT;          break;
    case I3D_R32F:      internal_format=GL_R32F;        format=GL_RED;  type=GL_FLOAT;          break;

    case I3D_RG8:       internal_format=GL_RG8;         format=GL_RG;   type=GL_UNSIGNED_BYTE;  break;
    case I3D_RG8S:      internal_format=GL_RG8_SNORM;   format=GL_RG;   type=GL_BYTE;           break;
    case I3D_RG8U:      internal_format=GL_RG8UI;       format=GL_RG;   type=GL_UNSIGNED_BYTE;  break;
    case I3D_RG8I:      internal_format=GL_RG8I;        format=GL_RG;   type=GL_BYTE;           break;
    case I3D_RG16F:     internal_format=GL_RG16F;       format=GL_RG;   type=GL_FLOAT;          break;
    case I3D_RG32F:     internal_format=GL_RG32F;       format=GL_RG;   type=GL_FLOAT;          break;

    case I3D_RGB8:      internal_format=GL_RGB8;        format=GL_RGB;  type=GL_UNSIGNED_BYTE;  break;
    case I3D_RGB8S:     internal_format=GL_RGB8_SNORM;  format=GL_RGB;  type=GL_BYTE;           break;
    case I3D_RGB8U:     internal_format=GL_RGB8UI;      format=GL_RGB;  type=GL_UNSIGNED_BYTE;  break;
    case I3D_RGB8I:     internal_format=GL_RGB8I;       format=GL_RGB;  type=GL_BYTE;           break;
    case I3D_RGB16F:    internal_format=GL_RGB16F;      format=GL_RGB;  type=GL_FLOAT;          break;
    case I3D_RGB32F:    internal_format=GL_RGB32F;      format=GL_RGB;  type=GL_FLOAT;          break;

    case I3D_RGBA8:     internal_format=GL_RGBA8;       format=GL_RGBA; type=GL_UNSIGNED_BYTE;  break;
    case I3D_RGBA8S:    internal_format=GL_RGBA8_SNORM; format=GL_RGBA; type=GL_BYTE;           break;
    case I3D_RGBA8U:    internal_format=GL_RGBA8UI;     format=GL_RGBA; type=GL_UNSIGNED_BYTE;  break;
    case I3D_RGBA8I:    internal_format=GL_RGBA8I;      format=GL_RGBA; type=GL_BYTE;           break;
    case I3D_RGBA16F:   internal_format=GL_RGBA16F;     format=GL_RGBA; type=GL_FLOAT;          break;
    case I3D_RGBA32F:   internal_format=GL_RGBA32F;     format=GL_RGBA; type=GL_FLOAT;          break;

    case I3D_DEPTH16F:          internal_format=GL_DEPTH_COMPONENT16;   format=GL_DEPTH_COMPONENT;  type=GL_FLOAT;                          break;
    case I3D_DEPTH32F:          internal_format=GL_DEPTH_COMPONENT;     format=GL_DEPTH_COMPONENT;  type=GL_FLOAT;                          break;
    case I3D_DEPTH24_STENCIL8:  internal_format=GL_DEPTH24_STENCIL8;    format=GL_DEPTH_STENCIL;    type=GL_UNSIGNED_INT_24_8;              break;
    case I3D_DEPTH32F_STENCIL8: internal_format=GL_DEPTH32F_STENCIL8;   format=GL_DEPTH_STENCIL;    type=GL_FLOAT_32_UNSIGNED_INT_24_8_REV; break;

    case I3D_RGB_DXT1:  internal_format=GL_COMPRESSED_RGB_S3TC_DXT1_EXT;        break;
    case I3D_SRGB_DXT1: internal_format=GL_COMPRESSED_SRGB_S3TC_DXT1_EXT;       break;
    case I3D_RGBA_DXT1: internal_format=GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;       break;
    case I3D_SRGBA_DXT1:internal_format=GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT; break;
    case I3D_RGBA_DXT3: internal_format=GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;       break;
    case I3D_SRGBA_DXT3:internal_format=GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT; break;
    case I3D_RGBA_DXT5: internal_format=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;       break;
    case I3D_SRGBA_DXT5:internal_format=GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT; break;
    default:
        istAssert(false, "unknown format: %d", fmt);
        return false;
    }
    return true;
}



class DeviceResource
{
public:
    DeviceResource() : m_handle(0) {}
    GLuint getHandle() const { return m_handle; }

protected:
    GLuint m_handle;
};


class Buffer : public DeviceResource
{
public:
    Buffer(const BufferDesc &desc)
        : m_desc(desc)
    {
        glGenBuffers(1, &m_handle);
        glBindBuffer(m_desc.type, m_handle);
        glBufferData(m_desc.type, m_desc.size, m_desc.data, m_desc.usage);
        glBindBuffer(m_desc.type, 0);
    }

    Buffer::~Buffer()
    {
        if(m_handle!=0) {
            glDeleteBuffers(1, &m_handle);
            m_handle = 0;
        }
    }

    void Buffer::bind() const
    {
        glBindBuffer(m_desc.type, m_handle);
    }

    void Buffer::unbind() const
    {
        glBindBuffer(m_desc.type, 0);
    }

    void* Buffer::map(GLuint mode)
    {
        glBindBuffer(m_desc.type, m_handle);
        void *r = glMapBuffer(m_desc.type, mode);
        istAssert(r!=NULL, "BufferObject::map() failed\n");
        glBindBuffer(m_desc.type, 0);
        return r;
    }

    void Buffer::unmap()
    {
        glBindBuffer(m_desc.type, m_handle);
        glUnmapBuffer(m_desc.type);
        glBindBuffer(m_desc.type, 0);
    }

    const BufferDesc& getDesc() const { return m_desc; }

private:
    BufferDesc m_desc;
};


class VertexArray : public DeviceResource
{
public:

    VertexArray()
    {
        glGenVertexArrays(1, &m_handle);
    }

    ~VertexArray()
    {
        if(m_handle!=0) {
            glDeleteVertexArrays(1, &m_handle);
            m_handle = 0;
        }
    }

    void bind() const
    {
        glBindVertexArray(m_handle);
    }

    void unbind() const
    {
        glBindVertexArray(0);
    }

    void setAttributes( Buffer& vb, size_t stride, const VertexDesc *descs, size_t num_descs )
    {
        glBindVertexArray(m_handle);
        vb.bind();
        for(size_t i=0; i<num_descs; ++i) {
            const VertexDesc& desc = descs[i];
            glEnableVertexAttribArray(desc.location);
            // float type
            if(desc.type==GL_HALF_FLOAT || desc.type==GL_FLOAT || desc.type==GL_DOUBLE || desc.normalize) {
                glVertexAttribPointer(desc.location, desc.num_elements, desc.type, desc.normalize, stride, (GLvoid*)desc.offset);
            }
            // integer type
            else {
                glVertexAttribIPointer(desc.location, desc.num_elements, desc.type, stride, (GLvoid*)desc.offset);
            }
            glVertexAttribDivisor(desc.location, desc.divisor);
        }
        vb.unbind();
    }
};


class Sampler : public DeviceResource
{
public:
    Sampler( const SamplerDesc &desc )
        : m_desc(desc)
    {
        glGenSamplers(1, &m_handle);
        glSamplerParameteri(getHandle(), GL_TEXTURE_WRAP_S, m_desc.wrap_s);
        glSamplerParameteri(getHandle(), GL_TEXTURE_WRAP_T, m_desc.wrap_t);
        glSamplerParameteri(getHandle(), GL_TEXTURE_WRAP_R, m_desc.wrap_r);
        glSamplerParameteri(getHandle(), GL_TEXTURE_MIN_FILTER, m_desc.filter_min);
        glSamplerParameteri(getHandle(), GL_TEXTURE_MAG_FILTER, m_desc.filter_mag);
    }

    ~Sampler()
    {
        glDeleteSamplers(1, &m_handle);
    }

    void bind( uint32 slot ) const
    {
        glBindSampler(slot, getHandle());
    }

    void unbind( uint32 slot ) const
    {
        glBindSampler(slot, 0);
    }

private:
    SamplerDesc m_desc;
};

class Texture2D : public DeviceResource
{
public:

    Texture2D(const Texture2DDesc &desc)
        : m_desc(desc)
    {
        glGenTextures(1, &m_handle);

        GLint internal_format = 0;
        GLint format = 0;
        GLint type = 0;
        DetectGLFormat(m_desc.format, internal_format, format, type);

        bind();
        glTexImage2D( TEXTURE_TYPE, 0, internal_format, m_desc.size.x, m_desc.size.y, 0, format, type, m_desc.data );
        unbind();
    }

    ~Texture2D()
    {
        if(m_handle!=0) {
            glDeleteTextures(1, &m_handle);
            m_handle = 0;
        }
    }

    void bind() const
    {
        glBindTexture(TEXTURE_TYPE, m_handle);
    }
    void unbind() const
    {
        glBindTexture(TEXTURE_TYPE, 0);
    }

    void bind(uint32 slot) const
    {
        glActiveTexture(GL_TEXTURE0+slot);
        glBindTexture(TEXTURE_TYPE, m_handle);
    }

    void unbind(uint32 slot) const
    {
        glActiveTexture(GL_TEXTURE0+slot);
        glBindTexture(TEXTURE_TYPE, 0);
    }

    const Texture2DDesc& getDesc() const { return m_desc; }

private:
    static const GLuint TEXTURE_TYPE = GL_TEXTURE_2D;
    Texture2DDesc m_desc;
};



template<size_t ShaderType>
class ShaderObject : public DeviceResource
{
protected:
    ShaderObject()
    {
        m_handle = glCreateShader(ShaderType);
    }

    ~ShaderObject()
    {
        if(m_handle!=0) {
            glDeleteShader(m_handle);
            m_handle = 0;
        }
    }

    bool compile( const char *src, int length )
    {
        glShaderSource(m_handle, 1, &src, &length);
        glCompileShader(m_handle);

        // get errors
        GLint result;
        glGetShaderiv(m_handle, GL_COMPILE_STATUS, &result);
        if(result==GL_FALSE) {
            int length;
            glGetShaderiv(m_handle, GL_INFO_LOG_LENGTH, &length);
            if(length > 0) {
                int l;
                GLchar *info_log = new GLchar[length];
                glGetShaderInfoLog(m_handle, length, &l, info_log);
                istPrint(info_log);
                istAssert(false, "compile failed.");
                delete[] info_log;
            }
            return false;
        }

        return true;
    }
};

class VertexShader : public ShaderObject<GL_VERTEX_SHADER>
{
public:
    VertexShader::VertexShader( const VertexShaderDesc &desc ) { compile(desc.source, desc.source_len); }
    VertexShader::~VertexShader() {}
};

class PixelShader : public ShaderObject<GL_FRAGMENT_SHADER>
{
public:
    PixelShader::PixelShader( const PixelShaderDesc &desc ) { compile(desc.source, desc.source_len); }
    PixelShader::~PixelShader() {}
};

class ShaderProgram : public DeviceResource
{
public:
    ShaderProgram(const ShaderProgramDesc &desc)
    {
        m_handle = glCreateProgram();

        if(desc.vs) { glAttachShader(m_handle, desc.vs->getHandle()); }
        if(desc.ps) { glAttachShader(m_handle, desc.ps->getHandle()); }
        glLinkProgram(m_handle);

        // get errors
        GLint result;
        glGetProgramiv(m_handle, GL_LINK_STATUS, &result);
        if(result==GL_FALSE) {
            int length;
            glGetProgramiv(m_handle, GL_INFO_LOG_LENGTH, &length);
            if(length > 0) {
                int l;
                GLchar *info_log = new GLchar[length];
                glGetProgramInfoLog(m_handle, length, &l, info_log);
                istPrint(info_log);
                istAssert(false, "link failed.");
                delete[] info_log;
            }
        }
    }

    ~ShaderProgram()
    {
        if(m_handle!=0) {
            glDeleteProgram(m_handle);
            m_handle = 0;
        }
    }

    void bind()
    {
        glUseProgram(m_handle);
    }

    void unbind()
    {
        glUseProgram(0);
    }

    GLint ShaderProgram::getUniformBlockIndex(const char *name) const
    {
        GLint ul = glGetUniformBlockIndex(m_handle, name);
        if(ul == -1) {
            istPrint("no such uniform block named %s\n", name);
        }
        return ul;
    }

    void ShaderProgram::setUniformBlock(GLuint uniformBlockIndex, GLuint uniformBindingIndex, GLuint uniformBufferHandle)
    {
        glBindBufferBase(GL_UNIFORM_BUFFER, uniformBindingIndex, uniformBufferHandle);
        glUniformBlockBinding(m_handle, uniformBlockIndex, uniformBindingIndex);
    }
};



template<class BufferObjectType>
inline bool MapAndWrite(BufferObjectType& bo, const void *data, size_t data_size)
{
    istAssert(data_size<=bo.getDesc().size, "exceeded buffer size.\n");

    if(void *p = bo.map(GL_WRITE_ONLY)) {
        ::memcpy(p, data, data_size);
        bo.unmap();
        return true;
    }
    return false;
}

template<class ShaderType> inline ShaderType* CreateShaderFromString(const stl::string &source);

template<> inline VertexShader* CreateShaderFromString<VertexShader>(const stl::string &source)
{
    VertexShaderDesc desc = VertexShaderDesc(source.c_str(), source.size());
    return new VertexShader(desc);
}
template<> inline PixelShader* CreateShaderFromString<PixelShader>(const stl::string &source)
{
    PixelShaderDesc desc = PixelShaderDesc(source.c_str(), source.size());
    return new PixelShader(desc);
}

VertexShader*   CreateVertexShaderFromString(const stl::string &source)    { return CreateShaderFromString<VertexShader>(source); }
PixelShader*    CreatePixelShaderFromString(const stl::string &source)     { return CreateShaderFromString<PixelShader>(source); }


Texture2D* CreateTexture2DFromFile(const char *filename)
{
    Image img;
    if(!img.load(filename)) {
        istPrint("file load failed: %s\n", filename);
        return false;
    }
    Texture2DDesc desc(I3D_RGBA8, uvec2(img.width(), img.height()), 0, img.data());
    return new Texture2D(desc);
}
Texture2D* CreateTexture2DFromStream(IBinaryStream &st)
{
    Image img;
    if(!img.load(st)) {
        istPrint("file load failed\n");
        return false;
    }
    Texture2DDesc desc(I3D_RGBA8, uvec2(img.width(), img.height()), 0, img.data());
    return new Texture2D(desc);
}

Texture2D* CreateTexture2DFromImage(Image &img, I3D_COLOR_FORMAT format=I3D_COLOR_UNKNOWN)
{
    if(format==I3D_COLOR_UNKNOWN) {
        switch(img.getFormat()) {
        case IF_R8U:        format=I3D_R8; break;
        case IF_R8I:        format=I3D_R8S; break;
        case IF_R32F:       format=I3D_R32F; break;
        case IF_RG8U:       format=I3D_RG8; break;
        case IF_RG8I:       format=I3D_RG8S; break;
        case IF_RG32F:      format=I3D_RG32F; break;
        case IF_RGB8U:      format=I3D_RGB8; break;
        case IF_RGB8I:      format=I3D_RGB8S; break;
        case IF_RGB32F:     format=I3D_RGB32F; break;
        case IF_RGBA8U:     format=I3D_RGBA8; break;
        case IF_RGBA8I:     format=I3D_RGBA8S; break;
        case IF_RGBA32F:    format=I3D_RGBA32F; break;
        case IF_RGBA_DXT1:  format=I3D_RGBA_DXT1; break;
        case IF_RGBA_DXT3:  format=I3D_RGBA_DXT3; break;
        case IF_RGBA_DXT5:  format=I3D_RGBA_DXT5; break;
        }
    }
    Texture2DDesc desc(format, uvec2(img.width(), img.height()), 0, img.data(), img.size());
    return new Texture2D(desc);
}




#define UCS2_CODE_MAX			65536

struct SFF_HEAD
{
    uint32 Guid;
    uint32 Version;
    int32 FontSize;
    int32 FontWidth;
    int32 FontHeight;
    int32 SheetMax;
    int32 FontMax;
    wchar_t SheetName[64];
    struct {
        uint32 IsVertical	: 1;
        uint32 Pad		: 31;
    } Flags;
    uint16 IndexTbl[UCS2_CODE_MAX];
};
struct SFF_DATA
{
    uint16 u;
    uint16 v;
    uint8 w;
    uint8 h;
    uint8 No;
    uint8 Offset;
    uint8 Width;
};

struct FontQuad
{
    vec2 pos;
    vec2 size;
    vec2 uv_pos;
    vec2 uv_size;
    vec4 color;
};

class FSS
{
public:
    FSS()
        : m_header(NULL)
        , m_data(NULL)
        , m_color(1.0f, 1.0f, 1.0f, 1.0f)
        , m_size(0.0f)
        , m_spacing(1.0f)
        , m_monospace(false)
    {}

    void setTextureSize(const vec2 &v)
    {
        m_tex_size = v;
        m_rcp_tex_size = vec2(1.0f, 1.0f) / m_tex_size;
    }

    void setColor(const vec4 &v){ m_color=v; }
    void setSize(float32 v)     { m_size=v; }
    void setSpace(float32 v)    { m_spacing=v; }
    void setMonospace(bool v)   { m_monospace=v; }

    float getFontSize() const
    {
        return m_header!=NULL ? (float32)m_header->FontSize : 0.0f;
    }

    bool load(IBinaryStream &bf)
    {
        bf.setReadPos(0, IBinaryStream::Seek_End);
        m_buf.resize((size_t)bf.getReadPos());
        bf.setReadPos(0);
        bf.read(&m_buf[0], m_buf.size());
        if(m_buf.size()<4 || !(m_buf[0]=='F', m_buf[1]=='F', m_buf[2]=='S')) { return false; }

        m_header = (const SFF_HEAD*)&m_buf[0];
        m_data = (const SFF_DATA*)(&m_buf[0]+sizeof(SFF_HEAD));
        if(m_size==0.0f) { m_size=getFontSize(); }
        return true;
    }

    void makeQuads(const vec2 &pos, const wchar_t *text, size_t len, stl::vector<FontQuad> &quads) const
    {
        if(m_header==NULL) { return; }

        const float32 base_size = (float32)m_header->FontSize;
        const float32 scale = m_size / base_size;
        vec2 base = pos;
        for(size_t i=0; i<len; ++i) {
            uint32 di = m_header->IndexTbl[text[i]];
            float advance = (text[i] <= 0xff ? base_size*0.5f : base_size) * scale * m_spacing;
            if(di!=0xffff) {
                const SFF_DATA &cdata = m_data[di];
                vec2 uv = vec2(cdata.u, cdata.v);
                vec2 wh = vec2(cdata.w, cdata.h);
                vec2 scaled_wh = wh * scale;
                vec2 scaled_offset = vec2(float32(cdata.Offset) * scale, 0.0f);
                vec2 uv_pos = uv*m_rcp_tex_size;
                vec2 uv_size = wh * m_rcp_tex_size;
                FontQuad q = {base+scaled_offset, scaled_wh, uv_pos, uv_size, m_color};
                quads.push_back(q);
                if(!m_monospace) { advance = (scaled_wh.x + scaled_offset.x) * m_spacing; }
            }
            base.x += advance;
        }
    }

private:
    stl::vector<char> m_buf;
    const SFF_HEAD *m_header;
    const SFF_DATA *m_data;
    vec2 m_tex_size;
    vec2 m_rcp_tex_size;

    vec4 m_color;
    float32 m_size;
    float32 m_spacing;
    bool m_monospace;
};


const char *g_font_vssrc = "\
#version 330 core\n\
struct RenderStates\
{\
    mat4 ViewProjectionMatrix;\
};\
layout(std140) uniform render_states\
{\
    RenderStates u_RS;\
};\
layout(location=0) in vec2 ia_VertexPosition;\
layout(location=1) in vec2 ia_VertexTexcoord;\
layout(location=2) in vec4 ia_VertexColor;\
out vec2 vs_Texcoord;\
out vec4 vs_Color;\
\
void main(void)\
{\
    vs_Texcoord = ia_VertexTexcoord;\
    vs_Color    = ia_VertexColor;\
    gl_Position = u_RS.ViewProjectionMatrix * vec4(ia_VertexPosition, 0.0, 1.0);\
}\
";

const char *g_font_pssrc = "\
#version 330 core\n\
struct RenderStates\
{\
    mat4 ViewProjectionMatrix;\
};\
layout(std140) uniform render_states\
{\
    RenderStates u_RS;\
};\
uniform sampler2D u_Font;\
in vec2 vs_Texcoord;\
in vec4 vs_Color;\
layout(location=0) out vec4 ps_FragColor;\
\
void main()\
{\
    vec4 color = vs_Color;\
    color.a *= texture(u_Font, vs_Texcoord).r;\
    ps_FragColor = vec4(color);\
}\
";


class SpriteFontRenderer : public glIFontRenderer
{
public:
    struct VertexT
    {
        vec2 pos;
        vec2 texcoord;
        vec4 color;

        VertexT() {}
        VertexT(const vec2 &p, const vec2 &t, const vec4 &c) : pos(p), texcoord(t), color(c) {}
    };
    struct RenderState
    {
        mat4 matrix;
    };

    static const size_t MaxCharsPerDraw = 1024;

public:
    SpriteFontRenderer()
        : m_sampler(NULL)
        , m_texture(NULL)
        , m_vbo(NULL)
        , m_ubo(NULL)
        , m_vs(NULL)
        , m_ps(NULL)
        , m_shader(NULL)
        , m_uniform_loc(0)
    {}

    ~SpriteFontRenderer()
    {
        delete m_shader;
        delete m_ps;
        delete m_vs;
        delete m_va;
        delete m_ubo;
        delete m_vbo;
        delete m_sampler;
        delete m_texture;
    }

    bool initialize(IBinaryStream &fss_stream, IBinaryStream &img_stream)
    {
        {
            Image img, alpha;
            if(!img.load(img_stream)) { return false; }
            // alpha だけ抽出。RGBA ではない画像であれば red だけ抽出
            if(!ExtractAlpha(img, alpha)) { ExtractRed(img, alpha); }
            m_texture = CreateTexture2DFromImage(alpha);
        }
        if(!m_fss.load(fss_stream)) {
            return false;
        }
        m_fss.setTextureSize(vec2(m_texture->getDesc().size));
        m_sampler = new Sampler(SamplerDesc(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_LINEAR, GL_LINEAR));
        m_vbo = new Buffer(BufferDesc(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW, sizeof(VertexT)*4*MaxCharsPerDraw));
        m_ubo = new Buffer(BufferDesc(GL_UNIFORM_BUFFER, GL_DYNAMIC_DRAW, sizeof(RenderState)));
        {
            m_va = new VertexArray();
            const VertexDesc descs[] = {
                {0, GL_FLOAT, 2,  0, false, 0},
                {1, GL_FLOAT, 2,  8, false, 0},
                {2, GL_FLOAT, 4, 16, false, 0},
            };
            m_va->setAttributes(*m_vbo, sizeof(VertexT), descs, _countof(descs));
        }
        m_vs = CreateVertexShaderFromString(g_font_vssrc);
        m_ps = CreatePixelShaderFromString(g_font_pssrc);
        m_shader = new ShaderProgram(ShaderProgramDesc(m_vs, m_ps));
        m_uniform_loc = m_shader->getUniformBlockIndex("render_states");

        return true;
    }

    virtual void release() { delete this; }

    virtual void setScreen(float32 left, float32 right, float32 bottom, float32 top)
    {
        m_renderstate.matrix = glm::ortho(left, right, bottom, top);
    }
    virtual void setColor(float r, float g, float b, float a)   { m_fss.setColor(vec4(r,g,b,a)); }
    virtual void setSize(float32 v)         { m_fss.setSize(v); }
    virtual void setSpacing(float32 v)      { m_fss.setSpace(v); }
    virtual void setMonospace(bool v)       { m_fss.setMonospace(v); }

    virtual void addText(float x, float y, const char *text, size_t len)
    {
        // _alloca() で一時領域高速に取りたいところだが、_alloca() はマルチスレッド非対応っぽいので素直な実装で
        if(len==0) { len = strlen(text); }
        stl::string tmp(text, len);

        size_t wlen = mbstowcs(NULL, tmp.c_str(), 0);
        if(wlen==size_t(-1)) { return; }

        stl::wstring wtext;
        wtext.resize(wlen);
        mbstowcs(&wtext[0], tmp.c_str(), wlen);
        addText(x,y, wtext.c_str(), wlen);
    }

    virtual void addText(float x, float y, const wchar_t *text, size_t len)
    {
        if(len==0) { len=wcslen(text); }
        m_fss.makeQuads(vec2(x,y), text, len, m_quads);
    }

    virtual void flush()
    {
        if(m_quads.empty()) { return; }

        m_shader->setUniformBlock(m_uniform_loc, 0, m_ubo->getHandle());
        m_va->bind();
        m_shader->bind();
        m_sampler->bind(0);
        m_texture->bind(0);

        size_t drawn_quads = 0;
        for(;;) {
            size_t num_quad = stl::min<size_t>(m_quads.size()-drawn_quads, MaxCharsPerDraw);
            size_t num_vertex = num_quad*4;
            {
                VertexT *vertex = (VertexT*)m_vbo->map(GL_WRITE_ONLY);
                for(size_t qi=0; qi<num_quad; ++qi) {
                    const FontQuad &quad = m_quads[qi+drawn_quads];
                    VertexT *v = &vertex[qi*4];
                    const vec2 pos_min = quad.pos;
                    const vec2 pos_max = quad.pos + quad.size;
                    const vec2 tex_min = quad.uv_pos;
                    const vec2 tex_max = quad.uv_pos + quad.uv_size;
                    v[0] = VertexT(vec2(pos_min.x, pos_min.y), vec2(tex_min.x, tex_min.y), quad.color);
                    v[1] = VertexT(vec2(pos_min.x, pos_max.y), vec2(tex_min.x, tex_max.y), quad.color);
                    v[2] = VertexT(vec2(pos_max.x, pos_max.y), vec2(tex_max.x, tex_max.y), quad.color);
                    v[3] = VertexT(vec2(pos_max.x, pos_min.y), vec2(tex_max.x, tex_min.y), quad.color);
                }
                m_vbo->unmap();
            }
            MapAndWrite(*m_ubo, &m_renderstate, sizeof(m_renderstate));

            glDrawArrays(GL_QUADS, 0, num_vertex);

            drawn_quads += num_quad;
            if(drawn_quads==m_quads.size()) { break; }
        }
        m_quads.clear();
    }

private:
    FSS m_fss;
    stl::vector<FontQuad> m_quads;
    Sampler *m_sampler;
    Texture2D *m_texture;
    Buffer *m_vbo;
    Buffer *m_ubo;
    VertexArray *m_va;
    VertexShader *m_vs;
    PixelShader *m_ps;
    ShaderProgram *m_shader;
    GLint m_uniform_loc;
    RenderState m_renderstate;
};
} // namespace ist


glIFontRenderer* CreateGLSpriteFont(ist::IBinaryStream &sff, ist::IBinaryStream &img)
{
    static bool s_glew_initialized = false;
    if(!s_glew_initialized) {
        glewInit();
        ::setlocale(LC_ALL, ""); // mbstowcs() のため
    }

    ist::SpriteFontRenderer *r = new ist::SpriteFontRenderer();
    if(!r->initialize(sff, img)) {
        r->release();
        return NULL;
    }
    return r;
}

glIFontRenderer* CreateGLSpriteFont(const char *path_to_sff, const char *path_to_img)
{
    ist::FileStream sff(path_to_sff, "rb");
    ist::FileStream img(path_to_img, "rb");
    if(!sff.isOpened()) { istPrint("%s load failed\n", path_to_sff); return NULL; }
    if(!img.isOpened()) { istPrint("%s load failed\n", path_to_img); return NULL; }
    return CreateGLSpriteFont(sff, img);
}
