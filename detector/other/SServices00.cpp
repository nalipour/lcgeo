//====================================================================
//  DDSim - LC simulation based on DD4hep 
//--------------------------------------------------------------------
// DD4hep Geometry driver for SService00
// Ported from Mokka
//--------------------------------------------------------------------
//  S.Lu, DESY
//  $Id:$
//====================================================================
//
//*******************************************************
//*                                                     *
//*                      Mokka                          * 
//*   - the detailed geant4 simulation for ILC   -      *
//*                                                     *
//* For more information about Mokka, visit the         *
//*                                                     *
//*  Mokka.in2p3.fr  Mokka home page.                   *
//*                                                     *
//*******************************************************
//
// $Id: SServices00.cc,v 1.1 2009/06/04 11:40:42 musat Exp $
// $Name: mokka-07-00 $
//
// 
//
// SServices00.cc
//

#include "DD4hep/DetFactoryHelper.h"
#include "DD4hep/DD4hepUnits.h"
#include "XMLHandlerDB.h"

#include "SServices00.h"
 
using namespace std;
using namespace DD4hep;
using namespace dd4hep;
using namespace DD4hep::Geometry;

/*
bool BuildTPCEndplateServices(PlacedVolume *pVol,Assembly *envelope)
{
  //Todo: change the hard code numbers, and read them from parameters XML file.
  Material copper;
  double TPC_Ecal_Hcal_barrel_halfZ=235.0;
  double tpcEndplateServicesRing_R[8] = {45.0, 65.0, 85.0, 105.0, 125.0, 145.0, 165.0, 175.0};
  double tpcEndplateServicesRing_ro[8] = {0.1555, 0.1555, 0.1555, 0.1555, 0.1555, 0.1555, 0.2985, 0.2453};
  for(int i=0;i<8;i++){
    Torus solidTube(tpcEndplateServicesRing_R[i], 0, tpcEndplateServicesRing_ro[i], 0, 2*M_PI);
    double z_position = TPC_Ecal_Hcal_barrel_halfZ + tpcEndplateServicesRing_ro[i];
    Volume EnvTube("service_tube",solidTube,copper);
    Position pos1(0,0,z_position);
    *pVol = envelope->placeVolume(EnvTube,pos1);
    Position pos2(0,0,-z_position);
    *pVol = envelope->placeVolume(EnvTube,pos2);
  } 

  return true;
}

bool BuildEcalBarrelServices(PlacedVolume *pVol,Assembly *envelope)
{

  return true;
}

bool BuildEcalBarrel_EndCapServices(PlacedVolume *pVol,Assembly *envelope)
{

  return true;
}

bool BuildHcalBarrel_EndCapServices(PlacedVolume *pVol,Assembly *envelope)
{

  return true;
}

bool BuildSitCables(PlacedVolume *pVol,Assembly *envelope)
{

  return true;
}
*/

static Ref_t create_element(LCDD& lcdd, xml_h element, SensitiveDetector sens)  {

  static double tolerance = 0e0;

  xml_det_t x_det = element;
  string det_name = x_det.nameStr();
  
  int det_id = x_det.id();
  DetElement sdet (det_name,det_id);
  Volume motherVol = lcdd.pickMotherVolume(sdet);
  
  Assembly envelope_assembly( det_name + "assembly" ) ;
  PlacedVolume pv;


//====================================================================
// build all services
//====================================================================
  cout << "\nBuilding SServices00"<< endl;

  //==================================================
  //BuildTPCEndplateServices(&pv,&envelope_assembly);
  //==================================================
  
  // start to prepare the Material and geometry as Mokka
  Material copper;
  double TPC_Ecal_Hcal_barrel_halfZ =lcdd.constant<double>("TPC_Ecal_Hcal_barrel_halfZ");

  const int MAX_TPC_RINGS = 8;
  double tpcEndplateServices_R[MAX_TPC_RINGS];
  double tpcEndplateServices_r[MAX_TPC_RINGS];

  tpcEndplateServices_R[0] = lcdd.constant<double>("tpcEndplateServicesRing1_R");
  tpcEndplateServices_r[0] = lcdd.constant<double>("tpcEndplateServicesRing1_ro");

  tpcEndplateServices_R[1] = lcdd.constant<double>("tpcEndplateServicesRing2_R");
  tpcEndplateServices_r[1] = lcdd.constant<double>("tpcEndplateServicesRing2_ro");

  tpcEndplateServices_R[2] = lcdd.constant<double>("tpcEndplateServicesRing3_R");
  tpcEndplateServices_r[2] = lcdd.constant<double>("tpcEndplateServicesRing3_ro");

  tpcEndplateServices_R[3] = lcdd.constant<double>("tpcEndplateServicesRing4_R");
  tpcEndplateServices_r[3] = lcdd.constant<double>("tpcEndplateServicesRing4_ro");

  tpcEndplateServices_R[4] = lcdd.constant<double>("tpcEndplateServicesRing5_R");
  tpcEndplateServices_r[4] = lcdd.constant<double>("tpcEndplateServicesRing5_ro");

  tpcEndplateServices_R[5] = lcdd.constant<double>("tpcEndplateServicesRing6_R");
  tpcEndplateServices_r[5] = lcdd.constant<double>("tpcEndplateServicesRing6_ro");

  tpcEndplateServices_R[6] = lcdd.constant<double>("tpcEndplateServicesRing7_R");
  tpcEndplateServices_r[6] = lcdd.constant<double>("tpcEndplateServicesRing7_ro");

  tpcEndplateServices_R[7] = lcdd.constant<double>("tpcEndplateServicesRing8_R");
  tpcEndplateServices_r[7] = lcdd.constant<double>("tpcEndplateServicesRing8_ro");
  
  // start to build TPC cooling rings with the class BuildTPCEndplateServices
  BuildTPCEndplateServices TPCEndplateServices;
  TPCEndplateServices.setMaterial(copper);
  TPCEndplateServices.sethalfZ(TPC_Ecal_Hcal_barrel_halfZ);
  for(int i=0;i<MAX_TPC_RINGS;i++)
    TPCEndplateServices.settpcEndplateServicesRing_R_ro(tpcEndplateServices_R[i],tpcEndplateServices_r[i]);
  
  TPCEndplateServices.DoBuildTPCEndplateServices(&pv,&envelope_assembly);
 
  //BuildTPCEndplateServices(&pv,&envelope_assembly);

  //BuildEcalBarrelServices(&pv,&envelope_assembly);

  //BuildEcalBarrel_EndCapServices(&pv,&envelope_assembly);

  //BuildHcalBarrel_EndCapServices(&pv,&envelope_assembly);

  //BuildSitCables(&pv,&envelope_assembly);


//====================================================================
// Place services into the world volume
//====================================================================

  pv = motherVol.placeVolume(envelope_assembly);
  pv.addPhysVolID("system", det_id);
  sdet.setVisAttributes( lcdd, x_det.visStr(),  envelope_assembly);
  sdet.setPlacement(pv);

  return sdet;
}

DECLARE_DETELEMENT(SServices00,create_element)