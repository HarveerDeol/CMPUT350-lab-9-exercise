#ifndef DYNAMIC_CAMERA_H
#define DYNAMIC_CAMERA_H

#include <iostream>
#include <limits>
#include <cmath>
#include <algorithm>
#include "camera.h"

constexpr float INF = std::numeric_limits<float>::infinity();

// AABB in 1D
struct Interval {
    float min;
    float max;

    constexpr bool contains(const Interval& other) const {
        return (min <= other.min) && (max >= other.max);
    }

    constexpr bool contains(float x) const { return (min <= x) && (x <= max); }

    // signedDistance returns a positive smallest distance of x from the interval boundary
    // if it is not contained, else it returns a smallest (in magnitude) negative distance
    // from the interval boundary.
    constexpr float signedDistance(float x) const {
        if (contains(x)) {
            // 0 or negative
            return std::max(x - max, min - x);
        }
        // positive
        return std::max(x - max, min - x);
    }
};

struct DynamicCamera {
    void update(Camera& camera,                           // the camera to update
                const sf::Vector2f& trackedPosWorld,      // world coords of player to track
                const sf::Vector2f& trackedVelocityWorld  // world velocity of player to track
    ) {
        float sign; 
        if (trackedVelocityWorld.x == 0.0f){
            sign = 0;
        } else {
        sign = (0.0f < trackedVelocityWorld.x) - (trackedVelocityWorld.x < 0.0f);
        }
        sf::Vector2f lookAheadVec = {lookAhead,0};
        sf::Vector2f lookAheadVecPerc =  camera.windowPercentToWorldDist(lookAheadVec);
        float targetOffsetWorld = lookAheadVecPerc.x*sign;
        lookaheadState = lookaheadState + alpha *(targetOffsetWorld - lookaheadState);

        camera.setCameraCenter(cameraUnadjustedCenter);
        sf::Vector2f trackedPosWorldPerc = camera.worldToWindowPercent(trackedPosWorld);
        float playerLoc = trackedPosWorldPerc.x; 

        float deltaPercent = 0.f; 

        if (deadZoneX.contains(playerLoc)) {
        } else if (softZoneX.contains(playerLoc)) {
            if (playerLoc < deadZoneX.min){
                deltaPercent = (playerLoc - deadZoneX.min)*alpha;
            } else{
                deltaPercent = (playerLoc - deadZoneX.max)*alpha;
            }
            
        } else {
            if (playerLoc < softZoneX.min){
                deltaPercent = (playerLoc - softZoneX.min);
            } else{
                deltaPercent = (playerLoc - softZoneX.max);
            }
        }

        sf::Vector2f deltaPercentVec = {deltaPercent, 0};
        sf::Vector2f deltaWorld = camera.windowPercentToWorldDist(deltaPercentVec);
        cameraUnadjustedCenter = {cameraUnadjustedCenter.x + deltaWorld.x, cameraUnadjustedCenter.y};
        sf::Vector2f newView = {cameraUnadjustedCenter.x + lookaheadState, cameraUnadjustedCenter.y};
        camera.setCameraCenter(newView);




        // TODO: (HINTS/outline)
        // collect statistics on the player position and velocity in (normalized, if you like)
        // window coordinates. Compute lookAhead distance in world coords, adjusting for
        // if the player is moving left or right.
        // Now update lookAhead state using an EMA over its previous state and the lookAhead world
        // vector. Then determine if the player is in the dead zone, soft rejection zone, or hard
        // rejection zone. For each, perform EMA on the delta of the camera displacement (ignoring
        // lookahead) appropriately. Next,
        // set the camera center by offsetting the cameraUnadjustedCenter, combining lookAhead.
        // Finally update cameraUnadjustedCenter, but without lookahead.
    }

    float alpha;  // alpha to use for damping of camera position, *as well as* for smoothing of
                  // lookahead offset.
    Interval deadZoneX;  // percentage interval along x-axis to use for dead zone in [0, 1]
    // percentage interval along x-axis to use for soft rejection zone in [0, 1]
    Interval softZoneX = {-INF, INF};
    // (everything beyond softZone is considered hard zone).
    float lookAhead;  // screen-percentage lookahead
    mutable sf::Vector2f
        cameraUnadjustedCenter;      // the camera world center from last frame, ignoring lookahead
    mutable float lookaheadState{};  // the EMA of the accumulated lookahead offset in world coords
};

#endif // DYNAMIC_CAMERA_H
