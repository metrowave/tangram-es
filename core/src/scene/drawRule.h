#pragma once

#include "scene/styleParam.h"

#include <vector>
#include <deque>

namespace Tangram {

struct Feature;
class Tile;
class Scene;
class SceneLayer;
class StyleContext;

/*
 * A draw rule is a named collection of style parameters. When a draw rule is found to match a
 * feature, the feature's geometry is built into drawable buffers using a style determined from the
 * rule with the parameters contained in the rule.
 *
 * Draw rules are represented in two ways: by a DrawRuleData and by a DrawRule.
 *
 * DrawRuleData represents a named set of style parameters *as they are written in the layer*.
 * This is different from the set of style parameters that is applied to a feature after matching
 * and merging; the merged set of style parameters is represented by DrawRule.
 *
 * DrawRule is a temporary object and only contains style parameters that are defined in at least
 * one DrawRuleData, so it can safely reference any needed parameters by pointers to the original.
 * However, style parameters that need to be evaluated per-feature must also have space to store
 * their evaluated values.
 *
 * When matching and merging draw rules, the name of the rule is frequently copied and compared. To
 * make this process faster, each string used as the name of a draw rule is assigned to an integer
 * index within the scene object and then stored as the id of a draw rule.
 */

struct DrawRuleData {

    std::vector<StyleParam> parameters;
    std::string name;
    int id;

    DrawRuleData(std::string _name, int _id,
                 const std::vector<StyleParam>& _parameters);

    std::string toString() const;

};

struct DrawRule {

    const StyleParam* params[StyleParamKeySize] = { nullptr };

    StyleParam evaluated[StyleParamKeySize];

    const std::string* name = nullptr;

    int id;

    DrawRule(const DrawRuleData& _ruleData);

    void merge(const DrawRuleData& _ruleData);

    bool isJSFunction(StyleParamKey _key) const;

    bool contains(StyleParamKey _key) const;

    const std::string& getStyleName() const;

    const StyleParam& findParameter(StyleParamKey _key) const;

    template<typename T>
    bool get(StyleParamKey _key, T& _value) const {
        auto& param = findParameter(_key);
        if (!param) { return false; }
        if (!param.value.is<T>()) {
            return false;
        }
        _value = param.value.get<T>();
        return true;
    }

private:
    void logGetError(StyleParamKey _expectedKey, const StyleParam& _param) const;

};

struct Styling {

    /* Determine and apply DrawRules for a @_feature and add
     * the result to @_tile
     */
    void apply(const Feature& _feature, const Scene& _scene,
               const SceneLayer& _sceneLayer,
               StyleContext& _ctx, Tile& _tile);

    // internal
    bool match(const Feature& _feature, const SceneLayer& _layer, StyleContext& _ctx);

    // internal
    void mergeRules(const std::vector<DrawRuleData>& rules);

    // Reusable containers 'matchedRules' and 'queuedLayers'
    std::vector<DrawRule> matchedRules;
    // NB: Minimal memory usage of deque:
    // http://info.prelert.com/blog/stl-container-memory-usage
    // libdc++   4096 + 8 bytes heap
    // libstdc++ 512 + 64 bytes heap
    std::deque<std::vector<SceneLayer>::const_iterator> queuedLayers;

};

}
