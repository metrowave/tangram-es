#pragma once

#include <vector>
#include <set>
#include <cmath>
#include <memory>

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "util/mapProjection.h"
#include "util/tileID.h"

/* ViewModule
 * 1. Stores a representation of the current view into the map world
 * 2. Determines which tiles are visible in the current view
 * 3. Tracks changes in the view state to determine when new rendering is needed
 *
 * TODO: Make this into an interface for different implementations
 * For now, this is a simple implementation of the viewModule responsibilities
 * using a top-down axis-aligned orthographic view
*/

class View {

public:

    View(int _width = 800, int _height = 600, ProjectionType _projType = ProjectionType::mercator);
    
    /* Sets a new map projection with default tileSize */
    void setMapProjection(ProjectionType _projType);
    
    /* Gets the current map projection */
    const MapProjection& getMapProjection();
    
    /* Sets the ratio of hardware pixels to logical pixels (for high-density screens)
     * 
     * If unset, default is 1.0 
     */
    void setPixelScale(float _pixelsPerPoint);

    /* Sets the size of the viewable area in pixels */
    void setSize(int _width, int _height);
    
    /* Sets the position of the view within the world (in projection units) */
    void setPosition(double _x, double _y);
    
    /* Sets the zoom level of the view */
    void setZoom(float _z);
    
    /* Moves the position of the view */
    void translate(double _dx, double _dy);
    
    /* Changes zoom by the given amount */
    void zoom(float _dz);
    
    /* Gets the current zoom */
    float getZoom() const { return m_zoom; };

	/* Get the current zoomState */
	bool getZoomState() const { return m_zoomState; };
    
    /* Updates the view and projection matrices if properties have changed */
    void update();
    
    /* Gets the position of the view in projection units (z is the effective 'height' determined from zoom) */
    const glm::dvec3& getPosition() const { return m_pos; };
    
    const glm::dmat4& getViewMatrix() const { return m_view; }
    const glm::dmat4& getProjectionMatrix() const { return m_proj; };
    const glm::dmat4 getViewProjectionMatrix();

    /* Returns a rectangle of the current view range as [[x_min, y_min], [x_max, y_max]] */
    glm::dmat2 getBoundsRect() const;
    
    /* Returns the set of all tiles visible at the current position and zoom */
    const std::set<TileID>& getVisibleTiles();
    
    /* Returns true if the view properties have changed since this function was last called */
    bool changedSinceLastCheck();

    virtual ~View() {
        m_visibleTiles.clear();
    }
    
    constexpr static const float s_maxZoom = 18.0;

protected:
    
    void updateMatrices();
    void updateTiles();

    std::unique_ptr<MapProjection> m_projection;
    bool m_dirty;
    bool m_changed;
    std::set<TileID> m_visibleTiles;
    glm::dvec3 m_pos;
    glm::dmat4 m_view;
    glm::dmat4 m_proj;
    float m_zoom;
    float m_initZoom = 16.0;
    /* +ve: zoom-in
     * -ve: zoom-out
     */
    bool m_zoomState = false;
    int m_vpWidth;
    int m_vpHeight;
    float m_width;
    float m_height;
    float m_aspect;
    float m_pixelScale = 1.0f;
    float m_pixelsPerTile = 256.0;
    
};

