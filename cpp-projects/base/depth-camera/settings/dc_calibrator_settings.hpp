
/*******************************************************************************
** Toolset-base                                                               **
** MIT License                                                                **
** Copyright (c) [2018] [Florian Lance]                                       **
**                                                                            **
** Permission is hereby granted, free of charge, to any person obtaining a    **
** copy of this software and associated documentation files (the "Software"), **
** to deal in the Software without restriction, including without limitation  **
** the rights to use, copy, modify, merge, publish, distribute, sublicense,   **
** and/or sell copies of the Software, and to permit persons to whom the      **
** Software is furnished to do so, subject to the following conditions:       **
**                                                                            **
** The above copyright notice and this permission notice shall be included in **
** all copies or substantial portions of the Software.                        **
**                                                                            **
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR **
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,   **
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL    **
** THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER **
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING    **
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER        **
** DEALINGS IN THE SOFTWARE.                                                  **
**                                                                            **
********************************************************************************/

#pragma once

// std
#include <vector>
#include <string>
#include <format>

// local
#include "depth-camera/frame/dc_frame.hpp"

namespace tool::cam {

struct RANSACSettings{

    int nbTries = 100;
    double confidence = 0.999; // Desired probability of success. Used for estimating early termination.
    int N = 3; // ransac with `ransac_n` correspondences.
    int maxIteration = 100000; //  Maximum iteration before iteration stops.
    double maxDistanceCorr = 0.075; // Maximum correspondence points-pair
    double similaritiesThreshold = 0.9; // specify the threshold within which 2 arbitrary edges are similar
    double distanceThreshold = 0.075; // Distance threashold for the check.
    bool mutualFilter = true; // Enables mutual filter such that the correspondence of the source point's correspondence is itself.

    double FPFHFeatureRadius = 0.25; // Specifies the radius of the search.
    int FPFHFeatureMaxNeghbours = 100; // Specifies the max neighbors to be searched.
};

struct DCCalibratorSettings{

    auto initialize(size_t nbClients) -> void{

        models.resize(nbClients);
        sources.resize(nbClients);
        for(size_t idC = 0; idC < nbClients; ++idC){
            auto name = std::format("[{}]", idC);
            models[idC]  = name;
            sources[idC] = name;
        }
        sources.push_back("All");

        if(nbClients > 1){
            sourceSelectionId = 1;
        }
    }

    // # clients to use
    std::vector<std::string> models;
    std::vector<std::string> sources;


    int modelSelectionId = 0;
    int sourceSelectionId = 0;

    // # registering
    std::int64_t durationMs = 5000;
    std::int64_t timeToWaitBeforeRegisteringMs = 0;
    int maxFramesToRecord = 1000;

    // # processing
    bool useProcessed = true;
    bool removeOutliers = true;
    bool computeSphereCenter = true;
    bool downSample = false;
    bool closestPoints = true;
    int maxNbPoints = 20;
    bool meanWithFirstZ = false;
    bool estimateSphere = false;

    float maxDistanceOutlier = 0.15f;
    double downSampleVoxelSize = 0.05;
    float ballRay = 0.025f;
    float rayError = 0.1f;

    // # calibration
    bool gridSearch = false; // ignore calibration parameters and use a grid search for fitting them
    bool advancedParameters = false;
    bool doRansac = true;    // if true uses RANSAC, else ICP
    // ## normal
    double normalRadius = 0.1; // radius of the search
    int normalNeighbours = 30; // max neighbours to be searched
    // ## ransac
    RANSACSettings ransac;
    // ## icp
    double icpRelativeFitness = 0.000001; // If relative change (difference) of fitness score is lower than relative_fitness, the iteration stops.
    double icpRelativeRmse = 0.000001; // If relative change (difference) of inliner RMSE score is lower than relative_rmse, the iteration stops.
    double icpMaxDistanceCorr = 0.075; // max_correspondence_distance Maximum correspondence points-pair
    int icpMaxIteration = 30; // Maximum iteration before iteration stops.
};

struct DCCalibratorDisplaySettings{
    bool displayCalibrationCloud = true;
    bool displayProcessedCloud = true;
};


struct DCCalibratorClientData{

    size_t id;
    std::vector<std::shared_ptr<DCFrame>> frames;
    geo::ColorCloud calibrationCloud;
    geo::ColorCloud processedCloud;

    auto clean() -> void{
        frames.clear();
        calibrationCloud.clear();
        processedCloud.clear();
    }
};

}

