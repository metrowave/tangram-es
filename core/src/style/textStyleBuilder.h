#pragma once

#include "textStyle.h"

#include "labels/textLabel.h"
#include "text/fontContext.h"
#include "style/labelProperty.h"

#include "alfons/alfons.h"
#include "alfons/textBatch.h"
#include "alfons/textShaper.h"
#include "alfons/lineLayout.h"

#include <memory>
#include <vector>
#include <string>

namespace Tangram {

struct LineWrap {
    alfons::LineMetrics metrics;
    unsigned int nbLines;
};

/* Wrap an Alfons line layout, and draw the glyph quads to the TextBatch.
 *
 * This method is not threadsafe!
 *
 * _line the alfons LineLayout containing the glyph shapes and their related codepoints
 * _batch the text batch on which the mesh callback and atlas callback would be triggered
 * _maxChar the maximum line length
 * _minWordLength a parameter to control the minimum word length
 * _alignment align text (center, left, right)
 * _pixelScale the screen pixel density
 */
LineWrap drawWithLineWrapping(const alfons::LineLayout& _line, alfons::TextBatch& _batch,
    size_t _maxChar, size_t _minWordLength, TextLabelProperty::Align _alignment, float _pixelScale);


class TextStyleBuilder : public StyleBuilder {

public:

    TextStyleBuilder(const TextStyle& _style);

    const Style& style() const override { return m_style; }

    // StyleBuilder interface
    void addPolygon(const Polygon& _polygon, const Properties& _props, const DrawRule& _rule) override;
    void addLine(const Line& _line, const Properties& _props, const DrawRule& _rule) override;
    void addPoint(const Point& _line, const Properties& _props, const DrawRule& _rule) override;
    bool checkRule(const DrawRule& _rule) const override;

    virtual void setup(const Tile& _tile) override;

    virtual std::unique_ptr<StyledMesh> build() override;

    TextStyle::Parameters applyRule(const DrawRule& _rule, const Properties& _props) const;

    bool prepareLabel(TextStyle::Parameters& _params, Label::Type _type);
    void addLabel(const TextStyle::Parameters& _params, Label::Type _type,
                  Label::Transform _transform);

    std::string applyTextTransform(const TextStyle::Parameters& _params, const std::string& _string);

protected:

    struct ScratchBuffer : public alfons::MeshCallback {
        void drawGlyph(const alfons::Quad& q, const alfons::AtlasGlyph& altasGlyph) override {}
        void drawGlyph(const alfons::Rect& q, const alfons::AtlasGlyph& atlasGlyph) override;

        void reset();
        void clear();

        std::vector<GlyphQuad> quads;
        std::vector<std::unique_ptr<Label>> labels;

        // label width and height
        glm::vec2 bbox;
        glm::vec2 quadsLocalOrigin;
        int numLines;
        FontMetrics metrics;
        int numQuads;

        uint32_t fill;
        uint32_t stroke;
        uint8_t fontScale;

        float yMin, xMin;
    };

    const TextStyle& m_style;

    float m_tileSize;

    // TextShaper to create <LineLayout> for a given text
    alfons::TextShaper m_shaper;

    // TextBatch to 'draw' <LineLayout>s, i.e. creating glyph textures and glyph quads.
    // It is intialized with a TextureCallback provided by FontContext for adding glyph
    // textures and a MeshCallback provided by ScratchBufferthe for adding glyph quads.
    alfons::TextBatch m_batch;

    bool m_sdf;
    float m_pixelScale = 1;

    ScratchBuffer m_scratch;

    std::unique_ptr<TextLabels> m_textLabels;

};

}
