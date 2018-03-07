#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Json.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class CountryShapesApp : public App {
  public:
	void setup() override;
	void draw() override;
  Shape2d create_shape(JsonTree coordinates);
  std::vector<Shape2d> shapes;
};

void CountryShapesApp::setup() {
  setWindowSize(960, 540);
  
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
        create_shape(geom.getChild("coordinates")[0]));
    }
    
    // Handle MultiPolygon geometries
    if(type == "MultiPolygon") {
      ci::JsonTree polygons = geom.getChild("coordinates");
      for(auto p : polygons) {
        ci::JsonTree coords = p[0];
        shapes.push_back(create_shape(coords));
      }
    }
  }
}

// Create shape objects
Shape2d CountryShapesApp::create_shape(JsonTree coordinates) {
  Shape2d s;
  for(auto ll : coordinates) {
    float lat = ll.getValueAtIndex<float>(0);
    float lng = ll.getValueAtIndex<float>(1);
    vec2 p(lat, lng);
    s.empty() ? s.moveTo(p) : s.lineTo(p);
  }
  s.close();
  return s;
}

void CountryShapesApp::draw() {
	gl::clear(Color(0, 0, 0));
  
  ci::gl::ScopedMatrices m;
  gl::translate(getWindowCenter());
  
  // Invert the y-axis
  gl::scale(vec2(2.5f, -2.5f));
  
  // Draw solid shapes
  gl::color(Color(0.1, 0.1, 0.1));
  for(auto s : shapes) gl::drawSolid(s);
  
  // Draw outlines
  gl::color(Color(0.3, 0.3, 0.3));
  for(auto s : shapes) gl::draw(s);
}

CINDER_APP( CountryShapesApp, RendererGl )
