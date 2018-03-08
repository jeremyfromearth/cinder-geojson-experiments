#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"
#include "cinder/Shape2d.h"
#include "projections.h"

using namespace ci;
using namespace ci::app;
using namespace std;

typedef std::function<vec2(float lon, float lat)> projection_fn;

class MapProjectionsApp : public App {
  public:
	void setup() override;
	void draw() override;
  Shape2d create_shape(JsonTree coords, projection_fn fn);
  std::vector<Shape2d> shapes;
  projection_fn projfn;
};

void MapProjectionsApp::setup() {
  // Setup the app
  setWindowSize(960, 540);
  
  // Initialize the projection function to a standard mercator
  projfn = projections::mercator;
  
  // Load the data
  ci::JsonTree data(loadAsset("../../assets/data/countries.geo.json"));
  ci::JsonTree features = data.getChild("features");
  
  // Parse the data
  for(auto f : features) {
    ci::JsonTree props = f.getChild("properties");
    std::string name = props.getValueForKey("name");
    ci::JsonTree geom = f.getChild("geometry");
    std::string type = geom.getValueForKey("type");
    
    // Handle Polygon geometries
    if(type == "Polygon") {
      shapes.push_back(
        create_shape(geom.getChild("coordinates")[0], projfn));
    }
    
    // Handle MultiPolygon geometries
    if(type == "MultiPolygon") {
      ci::JsonTree polygons = geom.getChild("coordinates");
      for(auto p : polygons) {
        ci::JsonTree coords = p[0];
        shapes.push_back(create_shape(coords, projfn));
      }
    }
  }
}

// Create shape objects
Shape2d MapProjectionsApp::create_shape(JsonTree coordinates, projection_fn fn) {
  Shape2d s;
  for(auto ll : coordinates) {
    float lon = ll.getValueAtIndex<float>(0);
    float lat = ll.getValueAtIndex<float>(1);
    vec2 p = fn(lon, lat);
    s.empty() ? s.moveTo(p) : s.lineTo(p);
  }
  s.close();
  return s;
}

void MapProjectionsApp::draw() {
	gl::clear( Color( 0, 0, 0 ) );
  ci::gl::ScopedMatrices m;
  gl::translate(getWindowCenter());
  
  // Invert the y-axis
  gl::scale(vec2(1.5f, -1.5f));
  
  // Draw solid shapes
  gl::color(Color(0.1, 0.1, 0.1));
  for(auto s : shapes) gl::drawSolid(s);
  
  // Draw outlines
  gl::color(Color(0.3, 0.3, 0.3));
  for(auto s : shapes) gl::draw(s);
}

CINDER_APP( MapProjectionsApp, RendererGl )


