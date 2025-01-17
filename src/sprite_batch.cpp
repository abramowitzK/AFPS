#include <sprite_batch.hpp>
#include <rendering.hpp>
#include <algorithm>
#include <resource_manager.hpp>

namespace Aurora {

	SpriteBatch::SpriteBatch() {
		default_shader = ResourceManager::instance()->get_shader("spriteBatch");
	}

	SpriteBatch::~SpriteBatch() {
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
	}

	void SpriteBatch::init() {
		if(mIsInitialized)
			return;
		glGenVertexArrays(1, &mVao);
		glBindVertexArray(mVao);
		glGenBuffers(1, &mVbo);
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*) offsetof(Vertex2D, position));
		glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex2D), (void*) offsetof(Vertex2D, color));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*) offsetof(Vertex2D, uvs));
		glBindVertexArray(0);
		mIsInitialized = true;
	}

	void SpriteBatch::begin(GlyphSortType type) {
		mBatches.clear();
		mGlyphData.clear();
		this->mType = type;
	}

	void SpriteBatch::end() {
		mGlyphs.resize(mGlyphData.size());
		for(int i = 0; i < mGlyphData.size(); i++) {
			mGlyphs[i] = &mGlyphData[i];
		}
		sort_glyphs();
		create_render_batches();
	}

	void SpriteBatch::draw(const Vector4 &destRect, const Vector4 &uvRect, float depth, Texture *texture,
	                       const Color &color) {
		mGlyphData.emplace_back(destRect, uvRect, depth, texture, color);
	}

	void SpriteBatch::draw(Sprite s) {
		mGlyphData.emplace_back(Vector4(s.Pos.x, s.Pos.y, s.Dim.x, s.Dim.y), Vector4(0, 0, 1, 1), s.depth, s.Tex, s.Col);
	}

	void SpriteBatch::render_batches(Renderer *renderer) {
		glBindVertexArray(mVao);
		default_shader->bind();
		RenderState oldState = renderer->current_render_state;
		renderer->current_render_state = sSpriteBatchState;
		apply_render_state(renderer->current_render_state, &oldState);
		for(int i = 0; i < mBatches.size(); i++) {
			glBindTexture(GL_TEXTURE_2D, mBatches[i].texture->Tex);
			glDrawArrays(GL_TRIANGLES, mBatches[i].offset, mBatches[i].numVerts);
		}
		apply_render_state(oldState, &renderer->current_render_state);
		renderer->current_render_state = oldState;
	}

	void SpriteBatch::create_render_batches() {
		vector<Vertex2D> vertices;
		vertices.resize(mGlyphs.size() * 6);
		if(mGlyphs.empty())
			return;
		int offset = 0;
		int cv = 0;
		mBatches.emplace_back(offset, 6, mGlyphs[0]->texture);
		vertices[cv++] = mGlyphs[0]->topLeft;
		vertices[cv++] = mGlyphs[0]->bottomLeft;
		vertices[cv++] = mGlyphs[0]->bottomRight;
		vertices[cv++] = mGlyphs[0]->bottomRight;
		vertices[cv++] = mGlyphs[0]->topRight;
		vertices[cv++] = mGlyphs[0]->topLeft;
		offset += 6;
		for(int cg = 1; cg < mGlyphs.size(); cg++) {
			if(mGlyphs[cg]->texture != mGlyphs[cg - 1]->texture)
				mBatches.emplace_back(offset, 6, mGlyphs[cg]->texture);
			else
				mBatches.back().numVerts += 6;
			vertices[cv++] = mGlyphs[cg]->topLeft;
			vertices[cv++] = mGlyphs[cg]->bottomLeft;
			vertices[cv++] = mGlyphs[cg]->bottomRight;
			vertices[cv++] = mGlyphs[cg]->bottomRight;
			vertices[cv++] = mGlyphs[cg]->topRight;
			vertices[cv++] = mGlyphs[cg]->topLeft;
			offset += 6;
		}
		glBindBuffer(GL_ARRAY_BUFFER, mVbo);
		//orphan buffer
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), nullptr, GL_DYNAMIC_DRAW);
		//uploadData
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex2D), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void SpriteBatch::sort_glyphs() {
		switch(mType) {
			case GlyphSortType::BackToFront: {
				std::stable_sort(mGlyphs.begin(), mGlyphs.end(),
				                 [](const Glyph *a, const Glyph *b) { return a->depth < b->depth; });
				break;
			}
			case GlyphSortType::FrontToBack: {
				std::stable_sort(mGlyphs.begin(), mGlyphs.end(),
				                 [](const Glyph *a, const Glyph *b) { return a->depth > b->depth; });
				break;
			}
			case GlyphSortType::Texture: {
				std::stable_sort(mGlyphs.begin(), mGlyphs.end(),
				                 [](const Glyph *a, const Glyph *b) { return a->texture->Tex < b->texture->Tex; });
				break;
			}
			default:
				break;

		}
	}
}
