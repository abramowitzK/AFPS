#pragma once
#include <GL/glew.h>
#include <render_state.hpp>
#include <structures.hpp>
#include <vector>
#include <vector_math.hpp>
namespace Aurora {
struct Renderer;
using namespace std;
struct Texture;
struct Sprite;
struct Shader;
struct Color;
struct RenderBatch {
    RenderBatch(GLuint offset, GLuint numVertices, Texture* tex)
        : offset(offset), numVerts(numVertices), texture(tex){};
    GLuint   offset;
    GLuint   numVerts;
    Texture* texture;
};

struct Glyph {
    Glyph() {}
    Glyph(const Vector4& destRect, const Vector4& uvRect, float depth, Texture* Texture, const Color& color);
    Texture* texture;
    float    depth;
    Vertex2D topLeft;
    Vertex2D bottomLeft;
    Vertex2D topRight;
    Vertex2D bottomRight;
};

struct SpriteBatch {
    SpriteBatch();

    ~SpriteBatch();

    void init();

    void begin(GlyphSortType type = GlyphSortType::Texture);

    void end();

    void draw(const Vector4& destRect, const Vector4& uvRect, float depth, Texture* texture, const Color& color);

    void draw(const Sprite& s);

    void render_batches(Renderer* renderer);

    Shader* default_shader = nullptr;

  private:
    void create_render_batches();

    void sort_glyphs();

    bool                mIsInitialized = false;
    bool                mIsBegun       = false;
    GLuint              mVbo           = 0;
    GLuint              mVao           = 0;
    vector<Glyph>       mGlyphData;
    vector<Glyph*>      mGlyphs;
    vector<RenderBatch> mBatches;
    GlyphSortType       mType;
    const RenderState   sSpriteBatchState{CullState::On,
                                        BlendState::Off,
                                        DepthState::Off,

                                        BlendFunc::SourceAlpha,
                                        BlendFunc::OneMinusSourceAlpha,
                                        DepthFunc::Less,

                                        {0.0, 0.0, 0.0, 1.0},
                                        FrontFace::Ccw,
                                        CullFace::Back,
                                        PolygonMode::Fill};
};
} // namespace Aurora
