//====================================================================
//  lcgeo - LC detector models in DD4hep 
//--------------------------------------------------------------------
//  DD4hep Geometry driver for SiWEcalEndcaps
//  Ported from Mokka
//--------------------------------------------------------------------
//  S.Lu, DESY
//  $Id: SEcal04Hybrid_Endcaps.cpp 962 2016-04-27 10:06:41Z shaojun $
//====================================================================

 /* History:  
  
// *******************************************************
// *                                                     *
// *                      Mokka                          * 
// *   - the detailed geant4 simulation for ILC   -      *
// *                                                     *
// * For more information about Mokka, visit the         *
// *                                                     *
// *  Mokka.in2p3.fr  Mokka home page.                   *
// *                                                     *
// *******************************************************
//
// $Id: SEcal04Hybrid_Endcaps.cpp 962 2016-04-27 10:06:41Z shaojun $
// $Name: mokka-07-00 $
//
// 
//
// SEcal04.cc
//
   Shaojun Lu:  Ported from Mokka SEcal04 Endcaps part. Read the constants from XML
                instead of the DB. Then build the Endcap in the same way with DD4hep
		construct.
		Inside SEcal04, some parameters, which used by Ecal Endcaps, come from
		Ecal Barrel. They can be ssen here again.
 */

#include "DD4hep/DetFactoryHelper.h"
#include "XML/Layering.h"
#include "DD4hep/Shapes.h"
#include "XML/Utilities.h"
#include "DDRec/DetectorData.h"
//#include "DDSegmentation/WaferGridXY.h"
#include "DDSegmentation/MegatileLayerGridXY.h"

using namespace std;
using namespace DD4hep;
using namespace DD4hep::Geometry;

//#define VERBOSE 1
//#define VERBOSEcoterra 1

// workaround for DD4hep v00-14 (and older) 
#ifndef DD4HEP_VERSION_GE
#define DD4HEP_VERSION_GE(a,b) 0 
#endif

static Ref_t create_detector(LCDD& lcdd, xml_h element, SensitiveDetector sens)  {
  static double tolerance = 0e0;

  xml_det_t     x_det     = element;
  string        det_name  = x_det.nameStr();
  Layering      layering (element);

  Material      air       = lcdd.air();
  //unused:  Material      vacuum    = lcdd.vacuum();

  int           det_id    = x_det.id();
  xml_comp_t    x_staves  = x_det.staves();
  DetElement    sdet      (det_name,det_id);
  Volume        motherVol = lcdd.pickMotherVolume(sdet);

  // --- create an envelope volume and position it into the world ---------------------

  Volume envelope = XML::createPlacedEnvelope( lcdd,  element , sdet ) ;

  XML::setDetectorTypeFlag( element, sdet ) ;

  if( lcdd.buildType() == BUILD_ENVELOPE ) return sdet ;
  //-----------------------------------------------------------------------------------

  sens.setType("calorimeter");

  Material stave_material  = lcdd.material(x_staves.materialStr());

  DetElement    module_det("module0",det_id);

  Readout readout = sens.readout();
  Segmentation seg = readout.segmentation();

  // check if we have a WaferGridXY segmentation :
  DD4hep::DDSegmentation::MegatileLayerGridXY* EBUSeg = 
//    dynamic_cast< DD4hep::DDSegmentation::WaferGridXY*>( seg.segmentation() ) ;
		dynamic_cast< DD4hep::DDSegmentation::MegatileLayerGridXY*>( seg.segmentation() ) ;
  
  std::vector<double> cellSizeVector = seg.segmentation()->cellDimensions(0); //Assume uniform cell sizes, provide dummy cellID
  double cell_sizeX      = cellSizeVector[0];
  double cell_sizeY      = cellSizeVector[1];

//====================================================================
//
// Read all the constant from ILD_o1_v05.xml
// Use them to build Ecal endcaps
//
//====================================================================

  int N_FIBERS_W_STRUCTURE = 2; 
  int N_FIBERS_ALVOULUS = 3;

  //  read parametere from compact.xml file
  double Ecal_Alveolus_Air_Gap              = lcdd.constant<double>("Ecal_Alveolus_Air_Gap");
  double Ecal_Slab_shielding                = lcdd.constant<double>("Ecal_Slab_shielding");
  double Ecal_Slab_copper_thickness         = lcdd.constant<double>("Ecal_Slab_copper_thickness");
  double Ecal_Slab_PCB_thickness            = lcdd.constant<double>("Ecal_Slab_PCB_thickness");
  double Ecal_Slab_glue_gap                 = lcdd.constant<double>("Ecal_Slab_glue_gap");
  double Ecal_Slab_ground_thickness         = lcdd.constant<double>("Ecal_Slab_ground_thickness");
  double Ecal_fiber_thickness               = lcdd.constant<double>("Ecal_fiber_thickness");
  double Ecal_Si_thickness                  = lcdd.constant<double>("Ecal_Si_thickness");
  
  double Ecal_inner_radius                  = lcdd.constant<double>("TPC_outer_radius") +lcdd.constant<double>("Ecal_Tpc_gap");
  double Ecal_radiator_thickness1           = lcdd.constant<double>("Ecal_radiator_layers_set1_thickness");
  double Ecal_radiator_thickness2           = lcdd.constant<double>("Ecal_radiator_layers_set2_thickness");
  double Ecal_radiator_thickness3           = lcdd.constant<double>("Ecal_radiator_layers_set3_thickness");
  double Ecal_Barrel_halfZ                  = lcdd.constant<double>("Ecal_Barrel_halfZ");
  
  double Ecal_support_thickness             = lcdd.constant<double>("Ecal_support_thickness");
  double Ecal_front_face_thickness          = lcdd.constant<double>("Ecal_front_face_thickness");
  double Ecal_lateral_face_thickness        = lcdd.constant<double>("Ecal_lateral_face_thickness");
  double Ecal_Slab_H_fiber_thickness        = lcdd.constant<double>("Ecal_Slab_H_fiber_thickness");

  double Ecal_Slab_Sc_PCB_thickness         = lcdd.constant<double>("Ecal_Slab_Sc_PCB_thickness");
  double Ecal_Sc_thickness                  = lcdd.constant<double>("Ecal_Sc_thickness");
  double Ecal_Sc_reflector_thickness        = lcdd.constant<double>("Ecal_Sc_reflector_thickness");

  double Ecal_endcap_extra_size             = lcdd.constant<double>("Ecal_endcap_extra_size");
  double Ecal_cables_gap                    = lcdd.constant<double>("Ecal_cables_gap");
  //unused:  double Lcal_outer_radius                  = lcdd.constant<double>("Lcal_outer_radius");
  //unused:  double Ecal_Lcal_ring_gap                 = lcdd.constant<double>("Ecal_Lcal_ring_gap");
  double Ecal_endcap_center_box_size        = lcdd.constant<double>("Ecal_endcap_center_box_size");

  int    Ecal_nlayers1                      = lcdd.constant<int>("Ecal_nlayers1");
  int    Ecal_nlayers2                      = lcdd.constant<int>("Ecal_nlayers2");
  int    Ecal_nlayers3                      = lcdd.constant<int>("Ecal_nlayers3");
  int    Ecal_barrel_number_of_towers       = lcdd.constant<int>("Ecal_barrel_number_of_towers");
  
  double Ecal_guard_ring_size               = lcdd.constant<double>("Ecal_guard_ring_size");

  //double      Ecal_cells_size                  = lcdd.constant<double>("Ecal_cells_size");
  double      EcalEndcap_inner_radius          = lcdd.constant<double>("EcalEndcap_inner_radius");
  double      EcalEndcap_outer_radius          = lcdd.constant<double>("EcalEndcap_outer_radius");
  double      EcalEndcap_min_z                 = lcdd.constant<double>("EcalEndcap_min_z");
  double      EcalEndcap_max_z                 = lcdd.constant<double>("EcalEndcap_max_z");

// 2016/0710 for strip
	int 	 Ecal_Sc_N_strips_across_module     = lcdd.constant<int>("Ecal_Sc_N_strips_across_module");
	int 	 Ecal_Sc_number_of_virtual_cells	  = lcdd.constant<int>("Ecal_Sc_number_of_virtual_cells");
	// 20160710 for ScECAL
	int 	 Ecal_n_wafers_per_tower; //					  = lcdd.constant<int>("Ecal_n_wafers_per_tower");

//====================================================================
//
// general calculated parameters
//
//====================================================================
  
  double Ecal_total_SiSlab_thickness = 
    Ecal_Slab_shielding + 
    Ecal_Slab_copper_thickness + 
    Ecal_Slab_PCB_thickness +
    Ecal_Slab_glue_gap + 
    Ecal_Si_thickness + 
    Ecal_Slab_ground_thickness +
    Ecal_Alveolus_Air_Gap / 2;
#ifdef VERBOSE
  std::cout << " Ecal_total_SiSlab_thickness = " << Ecal_total_SiSlab_thickness  << std::endl;
#endif
  
  double Ecal_total_ScSlab_thickness = 
    Ecal_Slab_shielding + 
    Ecal_Slab_copper_thickness + 
    Ecal_Slab_Sc_PCB_thickness +
    Ecal_Sc_thickness + 
    Ecal_Sc_reflector_thickness * 2 +
    Ecal_Alveolus_Air_Gap / 2;
#ifdef VERBOSE
  std::cout << " Ecal_total_ScSlab_thickness = " << Ecal_total_ScSlab_thickness  << std::endl;
#endif

    int Number_of_Si_Layers_in_Barrel = 0;
    int Number_of_Sc_Layers_in_Barrel = 0;

#ifdef VERBOSE
  std::cout << " Ecal total number of Silicon layers = " << Number_of_Si_Layers_in_Barrel  << std::endl;
  std::cout << " Ecal total number of Scintillator layers = " << Number_of_Sc_Layers_in_Barrel  << std::endl;
#endif
  
  // In this release the number of modules is fixed to 5
  double Ecal_Barrel_module_dim_z = 2 * Ecal_Barrel_halfZ / 5. ;
#ifdef VERBOSE
  std::cout << "Ecal_Barrel_module_dim_z  = " << Ecal_Barrel_module_dim_z  << std::endl;
#endif

  // The alveolus size takes in account the module Z size
  // but also 4 fiber layers for the alveoulus wall, the all
  // divided by the number of towers
  double alveolus_dim_z = 
    (Ecal_Barrel_module_dim_z - 2. * Ecal_lateral_face_thickness) /
    Ecal_barrel_number_of_towers - 
    2 * N_FIBERS_ALVOULUS  * Ecal_fiber_thickness  - 
    2 * Ecal_Slab_H_fiber_thickness -
    2 * Ecal_Slab_shielding;


#ifdef VERBOSE
  std::cout << "alveolus_dim_z = " <<  alveolus_dim_z << std::endl;
#endif

  int n_total_layers = Ecal_nlayers1 + Ecal_nlayers2 + Ecal_nlayers3;
  //TODO xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxNumber_of_Si_Layers_in_Barrel = n_total_layers+1;
  Number_of_Sc_Layers_in_Barrel = n_total_layers+1;
  //int total_number_of_layers = Ecal_nlayers1 + Ecal_nlayers2 + Ecal_nlayers3;

  double module_thickness = 
    Ecal_nlayers1 * Ecal_radiator_thickness1 +
    Ecal_nlayers2 * Ecal_radiator_thickness2 +
    Ecal_nlayers3 * Ecal_radiator_thickness3 +
    
  int(n_total_layers/2) * // fiber around W struct layers
    (N_FIBERS_W_STRUCTURE * 2 *  Ecal_fiber_thickness) +
    
    Number_of_Si_Layers_in_Barrel * // Silicon slabs plus fiber around and inside
    (Ecal_total_SiSlab_thickness +
     (N_FIBERS_ALVOULUS + 1 ) * Ecal_fiber_thickness) +
    
    Number_of_Sc_Layers_in_Barrel * // Scintillator slabs plus fiber around and inside
    (Ecal_total_ScSlab_thickness +
     (N_FIBERS_ALVOULUS + 1 ) * Ecal_fiber_thickness) +
    
    Ecal_support_thickness + Ecal_front_face_thickness;

  //------------------------------------------------------------------------------------

  DDRec::LayeredCalorimeterData::Layer caloLayer ;
  caloLayer.cellSize0 = cell_sizeX;
  caloLayer.cellSize1 = cell_sizeY;

  //== For Wafer === 
// for Sc
// how many rows on a slab. 1 or 2. so far, only 1
	Ecal_n_wafers_per_tower = EBUSeg->NMegaY(); 
//////////////
///  double cell_dim_x = caloLayer.cellSize0;
  double total_Si_dim_z = alveolus_dim_z;

  double util_SI_wafer_dim_z = 
    total_Si_dim_z/2 -  2 * Ecal_guard_ring_size;

	int nStripX = EBUSeg->NStripsX();
	int nStripY = EBUSeg->NStripsY();
	//Number of minumun cell unit (~5mm) in an EBU.
  int Num_of_minCellUnitInEBU   = std::max( nStripX, nStripY );
	//Number of minumun cell unit (~5mm) in a strip.
  int Num_of_minCellUnitInStrip = Num_of_minCellUnitInEBU / std::min( nStripX, nStripY );

	double residualT_L =  (double)Num_of_minCellUnitInEBU / std::min( nStripX, nStripY) - Num_of_minCellUnitInStrip;
	if ( residualT_L > 0 ) {
		std::cout << "WARNING, strip length/strip width is not a integer" << std::endl;
	}

#ifdef VERBOSEcoterra
	std::cout << "Num_of_minCellUnitInEBU = " << Num_of_minCellUnitInEBU << ", nStripX(Y) = " << nStripX << ", " << nStripY << std::endl;
#endif

  double cell_dim_z =  util_SI_wafer_dim_z/ Num_of_minCellUnitInEBU;

/////////////////////////////////
  double cell_dim_x = cell_dim_z;  /// cell_dim_x/z is the granularitiy size. coterra
/////////////////////////////////
  
#ifdef VERBOSE
  std::cout << " module_thickness = " << module_thickness  << std::endl;
#endif
  
// ========= Create Ecal endcaps   ====================================
//  It will be the volume for palcing the Ecal endcaps alveolus(i.e. Layers).
//  And the structure W plate.
//  Itself will be placed into the world volume.
// ==========================================================================

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//~              EndcapStandardModule                 ~
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  double EC_module_z_offset = Ecal_Barrel_module_dim_z * 2.5 + Ecal_cables_gap + module_thickness /2;

  double Ecal_endcap_rmax  = Ecal_inner_radius 
    + module_thickness
    + Ecal_endcap_extra_size;

  double rInner = 0;
  double rOuter = Ecal_endcap_rmax;
  double zPlane = module_thickness;

  //unused:  double Ecal_endcap_Tube_rmax = Lcal_outer_radius + Ecal_Lcal_ring_gap;
  
  PolyhedraRegular ECPolyHedra(8, M_PI/8.,rInner, rOuter, zPlane);
  //Tube CenterECTub(0., Ecal_endcap_Tube_rmax, module_thickness+0.0001);
  //SubtractionSolid EndCapSolid( ECPolyHedra, CenterECTub);

  Box  CenterECBox(Ecal_endcap_center_box_size/2.,Ecal_endcap_center_box_size/2.,module_thickness+0.0001);
  SubtractionSolid EndCapSolid( ECPolyHedra, CenterECBox);

  Volume EnvLogEndCap("endcap",EndCapSolid,air);
 


  //-------------------------------------------------------
  // Radiator and towers placements inside the Endcap module
  //-------------------------------------------------------

  // We count the layers starting from IP and from 1,
  // so odd layers should be inside slabs and
  // even ones on the structure.
  //
  double z_floor = 
    - module_thickness/2 +
    Ecal_front_face_thickness + 
    N_FIBERS_ALVOULUS * Ecal_fiber_thickness;

  double EC_y_bottom =
    + Ecal_endcap_center_box_size / 2
    + Ecal_lateral_face_thickness;

  double y_middle =
    (Ecal_inner_radius + module_thickness + Ecal_endcap_extra_size) 
    * tan(M_PI/8)
    - Ecal_lateral_face_thickness;

  double EC_y_top =
    Ecal_inner_radius + module_thickness + Ecal_endcap_extra_size
    - Ecal_lateral_face_thickness;

  double x_left =
    -(Ecal_inner_radius + module_thickness + Ecal_endcap_extra_size)
    + Ecal_lateral_face_thickness; 
 
  double x_right =
    + Ecal_endcap_center_box_size / 2
    - Ecal_lateral_face_thickness;

  double fiber_inter_alveolus =
    2 * (Ecal_Slab_H_fiber_thickness 
	 + Ecal_Slab_shielding
	 + N_FIBERS_ALVOULUS * Ecal_fiber_thickness);
  
  double EC_alveolus_dim_y = alveolus_dim_z;
  double EC_alveolus_x_left = 0;
  double EC_alveolus_dim_x = 0 ;
  double alv_upper_y = 0;
  double inc = (-y_middle -x_left ) / (EC_y_top - y_middle);

  double l_pos_z = z_floor;

  //========== fill data for reconstruction ============================
  DDRec::LayeredCalorimeterData* caloData = new DDRec::LayeredCalorimeterData ;
  caloData->layoutType = DDRec::LayeredCalorimeterData::EndcapLayout ;
  caloData->inner_symmetry = 4  ; // hard code cernter box hole
  caloData->outer_symmetry = 8  ; // outer Octagun
  caloData->phi0 = 0 ;

  /// extent of the calorimeter in the r-z-plane [ rmin, rmax, zmin, zmax ] in mm.
  caloData->extent[0] = EcalEndcap_inner_radius ;
  caloData->extent[1] = EcalEndcap_outer_radius ;
  caloData->extent[2] = EcalEndcap_min_z ;
  caloData->extent[3] = EcalEndcap_max_z ;

  //-------------------- start loop over ECAL layers ----------------------
  // Loop over the sets of layer elements in the detector.
  double radiator_dim_y = Ecal_radiator_thickness1 ; //to be updated with slice radiator thickness 

  double nRadiationLengths=0.;
  double nInteractionLengths=0.;
  double thickness_sum=0;
  
  nRadiationLengths   = Ecal_radiator_thickness1/(stave_material.radLength())
    +(Ecal_front_face_thickness + N_FIBERS_ALVOULUS * Ecal_fiber_thickness)/air.radLength();
  nInteractionLengths = Ecal_radiator_thickness1/(stave_material.intLength())
    +(Ecal_front_face_thickness + N_FIBERS_ALVOULUS * Ecal_fiber_thickness)/air.intLength();
  thickness_sum       = Ecal_radiator_thickness1
    +(Ecal_front_face_thickness + N_FIBERS_ALVOULUS * Ecal_fiber_thickness);

  int l_num = 1;
  bool isFirstSens = true;
  int myLayerNum = 0 ;
  for(xml_coll_t li(x_det,_U(layer)); li; ++li)  {
    xml_comp_t x_layer = li;
    int repeat = x_layer.repeat();
    // Loop over number of repeats for this layer.
    for (int j=0; j<repeat; j++)    {
      string l_name = _toString(l_num,"layer%d");
      double l_thickness = layering.layer(l_num-1)->thickness();  // Layer's thickness.
      l_pos_z  += l_thickness/2.;
      
      int EC_Number_of_towers = 0; 
      double y_floor = EC_y_bottom;

      bool fill_DDRec = true; //Fill DDRec only once within loop while.
      int increase_LayerID = 0;

// coterrag721, wafer_num should isolate also among alveoluses.
// wafer_num should a global number for a Endcap module.
// Henna Eigo kamo ... nanka imi wakatta?
			int wafer_num = 0;
      while ( ( y_floor + EC_alveolus_dim_y) < EC_y_top ) {
				int InSlabWafer_num = 0;
	  		alv_upper_y = y_floor + EC_alveolus_dim_y;
	  
	  		EC_alveolus_dim_x = x_right- x_left;
	  
	  		if( alv_upper_y <= y_middle ) {
	      	EC_alveolus_dim_x = x_right- x_left; // coterra x_left has negative value.
	    	}
	 			else
	    	{
	      	EC_alveolus_x_left = 
					(alv_upper_y - y_middle) * inc + x_left; // exseed from y_meddle x slope(=inc)
	      	EC_alveolus_dim_x = x_right
					- EC_alveolus_x_left;
#if VERBOSEcoterra
		cout << "y_floor = " << y_floor << ", EC_alveolus_dim_y = " << EC_alveolus_dim_y << ", EC_alveolus_dim_x = " << EC_alveolus_dim_x << endl; 
		cout << "EC_alveolus_x_left = " << EC_alveolus_x_left << endl; 
#endif
	    	}
	  
	  // We use the same method able to create the Barrel
	  // Slabs, so we have to rotate it later when placing 
	  // into the EC modules.
	  //
	  // We use the same method able to create the Barrel
	  // radiator plates between slabs, but with the good
	  // dimensions to avoid to rotate it later when placing 
	  // into the EC modules.
	  
	  // While the towers have the same shape use the same 
	  // logical volumes and parameters.
	  
	  
	  string tower_name = _toString(EC_Number_of_towers,"tower%d");
	  
	  Box        l_box(alveolus_dim_z/2.-tolerance,EC_alveolus_dim_x/2.-tolerance,l_thickness/2.0-tolerance);
	  Volume     l_vol(det_name+"_"+l_name+"_"+tower_name,l_box,air);
	  DetElement layer(module_det, l_name+tower_name, det_id);
	  
	  l_vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));
	  
	  
	  // Loop over the sublayers or slices for this layer.
	  int s_num = 1;
	  double s_pos_z = -(l_thickness / 2);
	  
	  //--------------------------------------------------------------------------------
	  // BuildEndcapAlveolus: BuildSiliconSlab:
	  //--------------------------------------------------------------------------------

		int sensLayer_num = 0;
	  increase_LayerID = 0;
	  for(xml_coll_t si(x_layer,_U(slice)); si; ++si)  {
	    xml_comp_t x_slice = si;
	    string     s_name  =  _toString(s_num,"slice%d");
	    double     s_thick = x_slice.thickness();
	    Material slice_material  = lcdd.material(x_slice.materialStr());
 
	    double slab_dim_x = alveolus_dim_z/2.-tolerance;  // short--slab x
	    double slab_dim_y = s_thick/2.-tolerance;
	    double slab_dim_z = EC_alveolus_dim_x/2.-tolerance; // long--slab z
	   
			//Note that 1st ax is short and "2nd is long", 3rd is thickness. 
	    Box        s_box(slab_dim_x,slab_dim_z,slab_dim_y);
	    Volume     s_vol(det_name+"_"+l_name+"_"+s_name,s_box,slice_material);
	    DetElement slice(layer,s_name,det_id);
	    
	    s_vol.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));

#ifdef VERBOSE	    
	    std::cout<<"x_slice.materialStr(): "<< x_slice.materialStr() <<std::endl;
#endif
	    if (x_slice.materialStr().compare(x_staves.materialStr()) == 0){
	      radiator_dim_y = s_thick;
	    // W StructureLayer has the same thickness as W radiator layer in the Alveolus layer

	      if (fill_DDRec) {
#if DD4HEP_VERSION_GE( 0, 15 )
		caloLayer.outer_nRadiationLengths   = nRadiationLengths;
		caloLayer.outer_nInteractionLengths = nInteractionLengths;
		caloLayer.outer_thickness           = thickness_sum; 
		
		//Only fill the layers information into DDRec after first layer as Mokka Gear.
		caloLayer.thickness = caloLayer.inner_thickness + caloLayer.outer_thickness ;
		if (!isFirstSens){ caloData->layers.push_back( caloLayer ) ;
#ifdef VERBOSE		  
		  std::cout<<" caloLayer.distance: "<< caloLayer.distance <<std::endl;
		  
		  std::cout<<" caloLayer.inner_nRadiationLengths: "<< caloLayer.inner_nRadiationLengths <<std::endl;
		  std::cout<<" caloLayer.inner_nInteractionLengths: "<< caloLayer.inner_nInteractionLengths <<std::endl;
		  std::cout<<" caloLayer.inner_thickness: "<< caloLayer.inner_thickness <<std::endl;
		  std::cout<<" caloLayer.sensitive_thickness: "<< caloLayer.sensitive_thickness <<std::endl;
		  
		  std::cout<<" caloLayer.outer_nRadiationLengths: "<< caloLayer.outer_nRadiationLengths <<std::endl;
		  std::cout<<" caloLayer.outer_nInteractionLengths: "<< caloLayer.outer_nInteractionLengths <<std::endl;
		  std::cout<<" caloLayer.outer_thickness: "<< caloLayer.outer_thickness <<std::endl;
		  
		  std::cout<<" EcalEndcaps[1]==>caloLayer.inner_thickness + caloLayer.outer_thickness: "
			   << caloLayer.inner_thickness + caloLayer.outer_thickness <<std::endl;
#endif
		}
#endif
		nRadiationLengths   = 0. ;
		nInteractionLengths = 0. ;
		thickness_sum       = 0. ;	    
		isFirstSens         = false;
	      }
	    }

	    if (fill_DDRec) {
	      nRadiationLengths   += s_thick/(2.*slice_material.radLength());
	      nInteractionLengths += s_thick/(2.*slice_material.intLength());
	      thickness_sum       += s_thick/2;
	    }


	    if ( x_slice.isSensitive() ) {
				sensLayer_num++; // start from  1 (2,3,4...)
	      //s_vol.setSensitiveDetector(sens);

	    // Normal squared wafers
	    	double wafer_dim_z = alveolus_dim_z / Ecal_n_wafers_per_tower;//TODO move to more global place.
				wafer_dim_z = wafer_dim_z - 2 * Ecal_guard_ring_size; // wafer_dim_x/y does not include guard rings.

	    	double wafer_dim_x = wafer_dim_z;

				double  wafer_dim_x_half = wafer_dim_x / 2.;
				double  wafer_dim_z_half = wafer_dim_z / 2.;

	    	//g802 coterra Box WaferSiSolid( wafer_dim_x_half, wafer_dim_z_half, slab_dim_y);
	    	Box WaferSiSolid( wafer_dim_z_half, wafer_dim_x_half, slab_dim_y);
      
				double stripLength = cell_dim_x * Num_of_minCellUnitInStrip; // cell_dim_x is square cell's dimension. 

				// we need regulating EBU size larger than one strip length at least. 
				// Therefore, "stripLength" is subtracted from the total length of slab.
		    int n_wafers_x = 
	      int( floor( (slab_dim_z*2 - stripLength) 
						 			/ ( wafer_dim_z + 2 * Ecal_guard_ring_size) ));
				//note that we use slab_dim_z not slab_dim_x.
				//This complicated definition of demension is a heritage from mokka.

			// first position is the out of Slab
	    double wafer_pos_x =
	      - slab_dim_z + 	// not slab_dim_x ; slab_dim_z--> long
	      - Ecal_guard_ring_size 
	      - wafer_dim_x /2 ;  ///// note that start position is diff from barrel.

	    int n_wafer_x;
	    for (n_wafer_x = 0; n_wafer_x < n_wafers_x ; n_wafer_x++) { 
			// messssy!
			// EC_dim_x parallel with global x, 
			// slab_z parallel with global x,
			// wafer x parallel with global x and wafer z parallel with global y,
			// implement wafer position.
				wafer_pos_x += wafer_dim_x + 2 * Ecal_guard_ring_size;

				double wafer_pos_z =
		  	-slab_dim_x + 
		  	Ecal_guard_ring_size +
		  	//wafer_dim_x /2;
		  	wafer_dim_z /2;

				for (int n_wafer_z = 0; n_wafer_z < Ecal_n_wafers_per_tower; n_wafer_z++) {
					InSlabWafer_num++;
					wafer_num++;
		    	string Wafer_name  =  _toString(wafer_num,"wafer%d");
		    	Volume WaferSiLog(det_name+"_"+l_name+"_"+s_name+"_"+Wafer_name,WaferSiSolid,slice_material);
		    	WaferSiLog.setSensitiveDetector(sens);
		    	WaferSiLog.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));
					// Again, note that 1st ax is short and "2nd is long", 3rd is thickness. 
		    	PlacedVolume wafer_phv = s_vol.placeVolume(WaferSiLog,Position(wafer_pos_z,
										   wafer_pos_x,
										   0));
		    	wafer_phv.addPhysVolID("wafer", wafer_num);
					EBUSeg->setIsRegulatingEBU( myLayerNum+increase_LayerID, wafer_num, false );
					EBUSeg->setWaferOffsetX( myLayerNum+increase_LayerID, wafer_num, wafer_dim_x_half ); //////////////// coterra 
					EBUSeg->setWaferOffsetY( myLayerNum+increase_LayerID, wafer_num, wafer_dim_z_half ); //////////////// coterra 
					EBUSeg->setTotalSizeY( alveolus_dim_z );
#if VERBOSEcoterra
			cout << "Set EBUSeg parameters for normal EBU" << endl;
			cout << " wafer_num <=" << wafer_num << endl;
			cout << " setWaferOffsetX <= " << wafer_dim_x_half << endl;
			cout << " setWaferOffsetY <= " << wafer_dim_z_half << endl;
			cout << " setTotalSizeY <= " << alveolus_dim_z << endl;
#endif

		    	wafer_pos_z +=
		      	wafer_dim_z +
		      	2 * Ecal_guard_ring_size;
		  	}
	    }
      
	    // Magic wafers to complete the slab...
	    // (wafers with variable number of cells just
	    // to complete the slab. in reality we think that
	    // we'll have just a few models of special wafers
	    // for that.
	    //
	    // slab_dim_z is longitudinal.
	    double resting_dim_x =
	      slab_dim_z*2 - 
	      (wafer_dim_x + 2 * Ecal_guard_ring_size) * 
	      n_wafers_x;
      
#if VERBOSEcoterra
			if ( 192 < wafer_num && wafer_num < 196 ) {
				std::cout << " at regular: wafer_num = " << wafer_num 
									<< ", resting_dim_x = " << resting_dim_x
									<< ", 2 * Ecal_guard_ring_size + stripLength = " << 2 * Ecal_guard_ring_size + stripLength 
									<< std::endl;
			}
#endif
	    if(resting_dim_x > (2 * Ecal_guard_ring_size + stripLength )) {
#if VERBOSEcoterra
		cout << "resting_dim_x = " << resting_dim_x << endl;
#endif
				int N_cells_x_remaining = int(floor((resting_dim_x - 
			     2 * Ecal_guard_ring_size)
			    	/cell_dim_x));
				//Here we need to make a Magic wafer!
				double regulatingWafer_dim_x = N_cells_x_remaining * cell_dim_x;
				double regulatingWafer_dim_x_half = regulatingWafer_dim_x / 2.0;

				// in wafer, 1st ax longitudinal, 2nd ax transverse, 3rd ax thickness.
//g802 coterra 				Box MagicWaferSiSolid(  regulatingWafer_dim_x_half,
				Box MagicWaferSiSolid(  wafer_dim_z/2,
//																wafer_dim_z/2,
																regulatingWafer_dim_x_half,
															 slab_dim_y);

				wafer_pos_x += ( wafer_dim_x + regulatingWafer_dim_x )/2. 
												+ 2 * Ecal_guard_ring_size;

				double wafer_pos_z = -slab_dim_x + Ecal_guard_ring_size + wafer_dim_z /2;
				for (int n_wafer_z = 0; n_wafer_z < Ecal_n_wafers_per_tower; n_wafer_z++) {
					InSlabWafer_num++;
					wafer_num++;
#if VERBOSEcoterra
					if ( 192 < wafer_num && wafer_num < 196 ) {
					std::cout << " at regulating: wafer_num = " << wafer_num 
										<< ", resting_dim_x = " << resting_dim_x
										<< ", 2 * Ecal_guard_ring_size + stripLength = " << 2 * Ecal_guard_ring_size + stripLength 
										<< std::endl;
					}
#endif
		    	string MagicWafer_name  =  _toString(wafer_num,"MagicWafer%d");
		    	Volume MagicWaferSiLog(det_name+"_"+l_name+"_"+s_name+"_"+MagicWafer_name,MagicWaferSiSolid,slice_material);
		    	MagicWaferSiLog.setSensitiveDetector(sens);
		    	MagicWaferSiLog.setVisAttributes(lcdd.visAttributes(x_slice.visStr()));

					// Again, again,  note that 1st ax is short and "2nd is long", 3rd is thickness. 
		    	PlacedVolume wafer_phv = s_vol.placeVolume(MagicWaferSiLog,Position(wafer_pos_z,
											wafer_pos_x,
											0));
			    wafer_phv.addPhysVolID("wafer", wafer_num);
					EBUSeg->setIsRegulatingEBU( myLayerNum+increase_LayerID, wafer_num, true );
					EBUSeg->setWaferOffsetX( myLayerNum+increase_LayerID, wafer_num, regulatingWafer_dim_x_half ); //////////////// coterra 
					EBUSeg->setWaferOffsetY( myLayerNum+increase_LayerID, wafer_num, wafer_dim_z_half ); //////////////// coterra 
					EBUSeg->setTotalSizeY( alveolus_dim_z );
#if VERBOSEcoterra
			cout << "Set EBUSeg parameters for regulatingEBU" << endl;
			cout << " wafer_num <=" << wafer_num << endl;
			cout << " setWaferOffsetX <=" << regulatingWafer_dim_x_half << endl;
			cout << " setWaferOffsetY <=" << wafer_dim_z_half << endl;
			cout << " setTotalSizeY <=" << alveolus_dim_z << endl;
#endif
		    // Magic wafers, set the waferOffsetX for this layer this wafer.
		    	wafer_pos_z += wafer_dim_z + 2 * Ecal_guard_ring_size;

					// Only in last iteration of the first sens layer in pair, we should do this.
					if ( sensLayer_num%2 == 1 ) wafer_num -= InSlabWafer_num;

		  	}
	    } // The end of Magic Wafer
	    

	    if (fill_DDRec) {
#if DD4HEP_VERSION_GE( 0, 15 )
	      //Store "inner" quantities
	      caloLayer.inner_nRadiationLengths   = nRadiationLengths ;
	      caloLayer.inner_nInteractionLengths = nInteractionLengths ;
	      caloLayer.inner_thickness           = thickness_sum ;
	      //Store sensitive slice thickness
	      caloLayer.sensitive_thickness       = s_thick ;
#ifdef VERBOSE	      
	      std::cout<<" l_num: "<<l_num <<std::endl;
	      std::cout<<" s_num: "<<s_num <<std::endl;
	      std::cout<<" Ecal_inner_radius: "<< Ecal_inner_radius <<std::endl;
	      std::cout<<" module_thickness: "<< module_thickness <<std::endl;
	      std::cout<<" l_pos_z: "<< l_pos_z <<std::endl;
	      std::cout<<" l_thickness: "<< l_thickness <<std::endl;
	      std::cout<<" s_pos_z: "<< s_pos_z <<std::endl;
	      std::cout<<" s_thick: "<< s_thick <<std::endl;
	      std::cout<<" radiator_dim_y: "<< radiator_dim_y <<std::endl;
#endif	    
	      //-----------------------------------------------------------------------------------------
	      caloLayer.distance  =  EC_module_z_offset + l_pos_z + (s_pos_z+s_thick/2)
		- caloLayer.inner_thickness ;
	      caloLayer.absorberThickness = radiator_dim_y ;

	      //-----------------------------------------------------------------------------------------
#endif

	      nRadiationLengths   = 0. ;
	      nInteractionLengths = 0. ;
	      thickness_sum       = 0. ;
	    }
	    
	    }

	    if (fill_DDRec) {
	      nRadiationLengths   += s_thick/(2.*slice_material.radLength());
	      nInteractionLengths += s_thick/(2.*slice_material.intLength());
	      thickness_sum       += s_thick/2.;
	    }
	    
	    slice.setAttributes(lcdd,s_vol,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());
	    
	    // Slice placement.
	    PlacedVolume slice_phv = l_vol.placeVolume(s_vol,Position(0,0,s_pos_z+s_thick/2));
	    
	    if ( x_slice.isSensitive() ) {
	      slice_phv.addPhysVolID("layer", myLayerNum+increase_LayerID);
#ifdef VERBOSE
	      std::cout << "current layer number = " << myLayerNum+increase_LayerID << std::endl;
#endif	      
	      increase_LayerID++;
	    }


	    slice.setPlacement(slice_phv);
	    // Increment Z position of slice.
	    s_pos_z += s_thick;
	    
	    // Increment slice number.
	    ++s_num;
	  }        
	  

	  if(radiator_dim_y <= 0) {
	    stringstream err;
	    err << " \n ERROR: The subdetector " << x_det.nameStr() << " geometry parameter -- radiator_dim_y = " << radiator_dim_y ;
	    err << " \n Please check the radiator material name in the subdetector xml file";
	    throw runtime_error(err.str());
	  }
	  
	  // #########################
	  // BuildEndcapStructureLayer
	  // #########################
	  
	  double radiator_dim_x = alveolus_dim_z;
	  
#ifdef VERBOSE
	  std::cout << "radiator_dim_x = " << radiator_dim_x << std::endl;
#endif  
	  
	  double radiator_dim_z = EC_alveolus_dim_x;
	  
	  string bs_name="bs";
	  
	  Box        EndcapStructureLayer_box(radiator_dim_x/2.,radiator_dim_z/2.,radiator_dim_y/2.);
	  Volume     EndcapStructureLayer_vol(det_name+"_"+l_name+"_"+bs_name,EndcapStructureLayer_box,lcdd.material(x_staves.materialStr()));
	  
	  EndcapStructureLayer_vol.setVisAttributes(lcdd.visAttributes(x_layer.visStr()));	
	  
	  
	  int limit_staves;
	  limit_staves = 4;
	  // **********************
	  for (int i_stave = 1;
	       i_stave <= limit_staves;
	       i_stave ++)
	    {
	      double angle_module = M_PI/2. * ( i_stave - 1 );
	      Position l_pos(-(y_floor + EC_alveolus_dim_y/2),-(-EC_alveolus_dim_x/2 + x_right),l_pos_z);
	      RotationZ rotz(angle_module);
	      Position l_new = rotz*l_pos;   // The position of EC_alveolus in each stave, place same EC_alveolus into 4 stave.
	      
	      RotationZYX rot(angle_module,0,0);
	      Transform3D tran3D(rot,l_new); // The rotation of EC_alveolus in each stave, place same EC_alveolus into 4 stave.
	      
	      PlacedVolume layer_phv = EnvLogEndCap.placeVolume(l_vol,tran3D);
	      //layer_phv.addPhysVolID("layer", l_num);
	      layer_phv.addPhysVolID("tower", EC_Number_of_towers);
	      layer_phv.addPhysVolID("stave", i_stave);
	      layer.setPlacement(layer_phv);
	      
#ifdef VERBOSE
	      std::cout<< "Ecal_endcap_towers: "<< EC_Number_of_towers
		       << " has been placed into EnvLogEndCap stave "<<i_stave<<"."
		       << std::endl;
#endif	    
	      
	      // Without last W StructureLayer, the last part is Si SD even layer.
	      // the last number of  Ecal_nlayers1, Ecal_nlayers2 and  Ecal_nlayers3 is odd.
	      int even_layer = l_num*2;
	      if(even_layer > Ecal_nlayers1 + Ecal_nlayers2 + Ecal_nlayers3) continue;
	      
	      double bsl_pos_z = l_pos_z + l_thickness/2. + (radiator_dim_y/2. + Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE));
	      
	      Position   bsl_pos(-(y_floor + EC_alveolus_dim_y/2),-(-EC_alveolus_dim_x/2 + x_right),bsl_pos_z);      // Position of the layer.
	      Position bsl_new = rotz*bsl_pos;
	      Transform3D bsl_tran3D(rot,bsl_new);
	      //unused:  PlacedVolume  EndcapStructureLayer_phv = 
	      EnvLogEndCap.placeVolume(EndcapStructureLayer_vol,bsl_tran3D);
	      
	    }
	  
	  y_floor += EC_alveolus_dim_y + fiber_inter_alveolus;
	  
	  EC_Number_of_towers++;

	  fill_DDRec = false; 
	}

      myLayerNum = myLayerNum + increase_LayerID;

      // Increment to next layer Z position.
      l_pos_z +=   (l_thickness/2. +(radiator_dim_y/2. + Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE))*2.);
      
      ++l_num;
 
#if DD4HEP_VERSION_GE( 0, 15 )
	caloLayer.outer_nRadiationLengths   = nRadiationLengths
	  + (Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE))/air.radLength();
	caloLayer.outer_nInteractionLengths = nInteractionLengths
	  + (Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE))/air.intLength();
	caloLayer.outer_thickness           = thickness_sum
	  + (Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE)); 

	//Only fill the layers information into DDRec after second layer as Mokka Gear.
	caloLayer.thickness = caloLayer.inner_thickness + caloLayer.outer_thickness ;
	if (!isFirstSens) caloData->layers.push_back( caloLayer ) ;
#ifdef VERBOSE
	std::cout<<" caloLayer.distance: "<< caloLayer.distance <<std::endl;
	
	std::cout<<" caloLayer.inner_nRadiationLengths: "<< caloLayer.inner_nRadiationLengths <<std::endl;
	std::cout<<" caloLayer.inner_nInteractionLengths: "<< caloLayer.inner_nInteractionLengths <<std::endl;
	std::cout<<" caloLayer.inner_thickness: "<< caloLayer.inner_thickness <<std::endl;
	std::cout<<" caloLayer.sensitive_thickness: "<< caloLayer.sensitive_thickness <<std::endl;

	std::cout<<" caloLayer.outer_nRadiationLengths: "<< caloLayer.outer_nRadiationLengths <<std::endl;
	std::cout<<" caloLayer.outer_nInteractionLengths: "<< caloLayer.outer_nInteractionLengths <<std::endl;
	std::cout<<" caloLayer.outer_thickness: "<< caloLayer.outer_thickness <<std::endl;

	std::cout<<" EcalEndcaps[2]==>caloLayer.inner_thickness + caloLayer.outer_thickness: "
		 << caloLayer.inner_thickness + caloLayer.outer_thickness <<std::endl;
#endif
#endif

      nRadiationLengths   = radiator_dim_y/(stave_material.radLength())
	+ (Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE))/air.radLength();
      nInteractionLengths = radiator_dim_y/(stave_material.intLength())
	+ (Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE))/air.intLength();
      thickness_sum       = radiator_dim_y
	+ (Ecal_fiber_thickness * (N_FIBERS_ALVOULUS + N_FIBERS_W_STRUCTURE));     
    }
  }
    
  
    // Set stave visualization.
    if (x_staves)   {
      EnvLogEndCap.setVisAttributes(lcdd.visAttributes(x_staves.visStr()));
    }
 








  //====================================================================
  // Place Ecal Endcap module into the assembly envelope volume
  //====================================================================
   
  for(int module_num=0;module_num<2;module_num++) {
    int module_id = ( module_num == 0 ) ? 0:6;
    double this_module_z_offset = ( module_id == 0 ) ? - EC_module_z_offset : EC_module_z_offset; 
    double this_module_rotY = ( module_id == 0 ) ? M_PI:0; 

    double this_module_rotZ = ( module_id == 0 ) ? M_PI/8 : -M_PI/8  ; 
    
    Position xyzVec(0,0,this_module_z_offset);
    RotationZYX rot( this_module_rotZ ,this_module_rotY, 0);
    Rotation3D rot3D(rot);
    Transform3D tran3D(rot3D,xyzVec);

    PlacedVolume pv = envelope.placeVolume(EnvLogEndCap,tran3D);

    pv.addPhysVolID("module",module_id); // z: -/+ 0/6

    DetElement sd = (module_num==0) ? module_det : module_det.clone(_toString(module_num,"module%d"));
    sd.setPlacement(pv);

  }
  
  sdet.addExtension< DDRec::LayeredCalorimeterData >( caloData ) ; 

  return sdet;
  
}



DECLARE_DETELEMENT(SEcal04Hybrid_Endcaps, create_detector)

