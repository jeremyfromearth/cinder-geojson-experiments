//
//  projections.h
//  MapProjections
//
//  Created by @jeremyfromearth on 3/7/18.
//

#pragma once

#include "cinder/CinderMath.h"

class projections {
  public:
    static ci::vec2 mercator(float lon, float lat) {
      return ci::vec2(lon, ci::toDegrees(log(tan(ci::toRadians(lat) / 2 +  M_PI/4))));
    }
};
