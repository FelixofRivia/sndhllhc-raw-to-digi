#ifndef SNDHLLHC_SISTRIP_POSITION_H
#define SNDHLLHC_SISTRIP_POSITION_H

#include <cmath>
#include "Math/Point3D.h"
#include "TGeoNavigator.h"
#include "TGeoManager.h"
#include "TGeoBBox.h"

#include "SiStripHardwareConstants.h"

ROOT::Math::XYZPoint GetSiStripPosition(uint32_t detector_id) {

    const int strip = detector_id & 0x3FF;                // actual strip ID
    const int geofile_detID = detector_id - strip + 999;   // the det id number needed to read the geometry
 
    const int layer = (geofile_detID >> 13);
    const int row = (geofile_detID >> 11) & 0x3;
    const int column = (geofile_detID >> 10) & 0x1;
    const int module_id = geofile_detID;

    double local_pos[3] = {0, 0, 0};
    TString path = TString::Format("/cave_1/"
                                   "Detector_0/"
                                   "volAdvTarget_0/"
                                   "Target_Layer_%d/"
                                   "Row_%d_Column_%d_0/"
                                   "Target_DoubleSensorVolume_%d",
                                   layer,
                                   row,
                                   column,
                                   module_id);
                                   
    TGeoNavigator *nav = gGeoManager->GetCurrentNavigator();
    if (nav->CheckPath(path)) {
        nav->cd(path);
    } else {
        std::cerr<<"Wrong path " << path << "\n";
        return ROOT::Math::XYZPoint(std::nan(""), std::nan(""), std::nan(""));  
    }

    // Get the corresponding node, which is a sensor made of strips
    TGeoNode *W = nav->GetCurrentNode();
    TGeoBBox *S = dynamic_cast<TGeoBBox *>(W->GetVolume()->GetShape());
    // Knowing the strip, get the postion along the sensor
    local_pos[1] = (strip - (MAX_SISTRIPS_PER_MODULE / 2)) * (SENSOR_LENGHT_CM / MAX_SISTRIPS_PER_MODULE);
    double left_pos[3] = {S->GetDX(), local_pos[1], 0};
    double right_pos[3] = {-(S->GetDX()), local_pos[1], 0};
    double global_left_pos[3], global_right_pos[3];
    nav->LocalToMaster(left_pos, global_left_pos);
    nav->LocalToMaster(right_pos, global_right_pos);
    // Return SiStrip midpoint
    return ROOT::Math::XYZPoint(
        (global_left_pos[0] + global_right_pos[0]) * 0.5,
        (global_left_pos[1] + global_right_pos[1]) * 0.5,
        (global_left_pos[2] + global_right_pos[2]) * 0.5
    );
}

#endif