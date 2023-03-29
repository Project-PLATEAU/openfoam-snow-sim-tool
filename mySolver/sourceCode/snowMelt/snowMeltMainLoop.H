{

  volScalarField t_nu = turbulence->nu(); // temp nu field
  volScalarField t_k = turbulence->k();
  const nearWallDist nwDis(mesh);
  
  // distance between the wall and the center of the cell adjacent to the wall
  //const volScalarField::Boundary& yp = nwDis.y();
  
  // collector variable for total mass of snow remaining on the patch after radiation
  double Mtot = 0.0;

  // collector variable for total volume of snow remaining on the patch after radiation
  double Vtot = 0.0;

  // temperature of the snow surface
  dimensionedScalar T_s (dimless, Zero);
  
  // variation of the snow depth
  dimensionedScalar deltaZ (dimless, Zero);
  
  dimensionedScalar e_sat (dimless, Zero);
  
  // rate of change of saturated specific humidity with temperature change
  dimensionedScalar deltaSatHum (dimless, Zero);
  
  // reference velocity magnitude @1m height
  double UoneMetUp = 0.0;  
  
  // input radiation to snow surface 
  dimensionedScalar R_in (dimless, Zero);
  
  // upward long-wavelength radiation
  dimensionedScalar L_out (dimless, Zero);
  
  // emissivity of the atmosphere
  dimensionedScalar eps_atmosphere (dimless, 0.8);
  
  // sensible heat flux 
  dimensionedScalar H_sensFlux (dimless, Zero);
  
  // latent heat flux 
  dimensionedScalar E_latentFlux (dimless, Zero);


  dimensionedScalar p_s (dimless, Zero);

  // saturated specific humidity
  dimensionedScalar q_sat (dimless, Zero);
  

  // Store boundary mesh information
  //const polyBoundaryMesh& boundaryMesh = mesh.boundaryMesh();

  // get current Time 
  //scalar tt = runTime.value();
  //Info<< "CURRENT TIME " << tt <<  nl;
  
  // get timeStep
  //scalar dtt = runTime.deltaTValue();
  //Info<< "TIME STEP " << dtt <<  nl;

  // accessing endTime 
  //scalar finalTime = rho.db().time().endTime().value();
  word finalTime =Foam::name(rho.db().time().endTime().value());
  // endTime check 
  //Info<< "finalTime is " << finalTime << nl;

  // Stefan-Boltzmann constant
  double sigmaSB = (5.670374419 / Foam::pow(10, 8));

  if (runTime.timeName() == finalTime)
  {
    // Loop over boundary patches
    forAll(mesh.boundary(), patch)
    {
 
      const fvPatch& snowPatch = mesh.boundary()[patch];

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
          #include "snowMeltEqn.H"
          Info<< "\tRemaining snow mass on patch " << mesh.boundary()[patch].name() << " is " << Mtot << " kg" << nl;   
          Info<< "\tRemaining snow volume on patch " << mesh.boundary()[patch].name() << " is " << Vtot << " m^3" <<  nl << nl;  
        }      
      } else {
        // compute snowDepth and snowMass on all wall patches, except the external boundary patches (like inlet, outlet, upper sky patch) whose names begins with "boundary" 
        if ( isType<wallFvPatch>(snowPatch) 
           &&
           // and if the patch is not a boundary patch
           !(snowPatch.name().substr(0, 8) == "boundary")
        )     
        {
          #include "snowMeltEqn.H"
          Info<< "\tRemaining snow mass on patch " << mesh.boundary()[patch].name() << " is " << Mtot << " kg" << nl;   
          Info<< "\tRemaining snow volume on patch " << mesh.boundary()[patch].name() << " is " << Vtot << " m^3" <<  nl << nl;  
        }         
      } 

    }
  }

  t_nu.clear(); 
  t_k.clear(); 
}

