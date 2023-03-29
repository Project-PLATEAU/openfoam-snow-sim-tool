{

  volScalarField t_nu = turbulence->nu(); // temp nu field
  volScalarField t_k = turbulence->k();
  const nearWallDist nwDis(mesh);
  // distance between the wall and the center of the cell adjacent to the wall
  const volScalarField::Boundary& yp = nwDis.y();
  double Ustar = 0.0;

  // collector variable for total mass of snow on the patch
  double Mtot = 0.0;

  // collector variable for total mass of volume on the patch
  double Vtot = 0.0;

  // collector for the total area of the patch
  //double Atot = 0.0; 
  double sqrt_k = 0.0;
  // erosion 
  double q_er = 0.0;
  double deltaZ_er = 0.0;
  // deposition
  double q_dep = 0.0;
  // total
  double q_tot = 0.0;
  double deltaZ = 0.0;


  // Store boundary mesh information
  //const polyBoundaryMesh& boundaryMesh = mesh.boundaryMesh();

  // get current Time 
  //scalar tt = runTime.value();
  //Info<< "CURRENT TIME " << tt <<  nl;
  
  // get timeStep
  scalar dtt = runTime.deltaTValue();
  //Info<< "TIME STEP " << dtt <<  nl;

  // Loop over boundary patches
  forAll(mesh.boundary(), patch)
  {
    // Boundary patch name
    //const word& patchName = mesh.boundary()[patch].name(); 
    // DEBUG TEST comment and switch to all wall patches for final version
    //if(patchName == "ground")

    const fvPatch& snowPatch = mesh.boundary()[patch];
    //Info<< "String TEST " << snowPatch.name().substr(0, 6) <<  nl;
  
    if(boolVal) // if ground_and_roofs_only is true
      // then compute only ground and roofs patches
    {
       if ( isType<wallFvPatch>(snowPatch) 
           && 
           (
              // check if the first 6 characters coincide to ground
              (snowPatch.name().substr(0, 6) == "ground")
           ||
              // or check if the first 4 characters coincide to "roof"
              (snowPatch.name().substr(0, 4) == "roof")
           )
         )
      {
        #include "snowDepthEqn.H"
        Info<< "\tTotal mass of snow on patch " << mesh.boundary()[patch].name() << " is " << Mtot << " kg"  << nl;
        Info<< "\tTotal volume of snow on patch " << mesh.boundary()[patch].name() << " is " << Vtot << " m^3" <<  nl << nl;
      }      
    } else {
      // compute snowDepth and snowMass on all wall patches, except the external boundary patches (like inlet, outlet, upper sky patch) whose names begins with "boundary" 
      if ( isType<wallFvPatch>(snowPatch) 
           &&
           // and if the patch is not a boundary patch
           !(snowPatch.name().substr(0, 8) == "boundary")
        )   
      {
        #include "snowDepthEqn.H"
        Info<< "\tTotal mass of snow on patch " << mesh.boundary()[patch].name() << " is " << Mtot << " kg"  << nl;
        Info<< "\tTotal volume of snow on patch " << mesh.boundary()[patch].name() << " is " << Vtot << " m^3" <<  nl << nl;
      }         
    }    
  }
  t_nu.clear(); 
  t_k.clear(); 
}

