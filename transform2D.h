#pragma once

#include "utils/glm_utils.h"


namespace transform2D {
    // Translate matrix
    inline glm::mat3 Translate(float translateX, float translateY) {
        // TODO(student): Implement the translation matrix
        return glm::mat3(1, 0, 0,                               // Column 1 in memory
                         0, 1, 0,                               // Column 2 in memory
                         translateX, translateY, 1);            // Column 3 in memory
    }


    // Scale matrix
    inline glm::mat3 Scale(float scaleX, float scaleY) {
        // TODO(student): Implement the scaling matrix
        return glm::mat3(scaleX, 0, 0,                          // Column 1 in memory
                         0, scaleY, 0,                          // Column 2 in memory 
                         0, 0, 1);                              // Column 3 in memory 

    }

    // Rotate matrix
    inline glm::mat3 Rotate(float radians) {
        // TODO(student): Implement the rotation matrix
        return glm::mat3(cos(radians), sin(radians), 0,         // Column 1 in memory 
                         -sin(radians), cos(radians), 0,        // Column 2 in memory 
                         0, 0, 1);                              // Column 3 in memory 

    }
}   // namespace transform2D
