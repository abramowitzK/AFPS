#pragma once
namespace Aurora {
	struct Color {
		unsigned char r, g, b, a;
	};
	static const Color RED = {255, 0, 0, 255};
	struct Vertex2D {
		Vector2 position;
		Color color;
		Vector2 uvs;
	};
	enum class GlyphSortType {
		None,
		FrontToBack,
		BackToFront,
		Texture
	};
}
