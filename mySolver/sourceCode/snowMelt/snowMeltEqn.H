// since snowMelt will be run for just 1 iteration we can compute centers, normal and probes 1 meter above center patches here.

double totFacesOfPatch = 0.0;
totFacesOfPatch = mesh.Cf().boundaryField()[patch].size();
reduce(totFacesOfPatch, sumOp<scalar>());


// number of faces of the chosen patch:
//Info<< "\tNumber of faces of patch " << mesh.boundary()[patch].name() << " is " << totFacesOfPatch << nl;

vector sumFacesCenters{0.0, 0.0, 0.0}; 
sumFacesCenters = gSum(mesh.Cf().boundaryField()[patch]);
reduce(sumFacesCenters, sumOp<vector>());

//Info<< "number of processors is " << Pstream::nProcs() << nl;

// compute center of the patch
vector centerPatch = sumFacesCenters / (totFacesOfPatch * Pstream::nProcs()) ;

// center of the chosen patch:
//Info<< "\tCenter of patch " << mesh.boundary()[patch].name() << " is " << centerPatch << nl;

// print velocity of point 1 meter above the center of the patch
vector probePoint = centerPatch;
// the probe point will be 1 meter above the center of the patch therefore 
probePoint.z() = probePoint.z() + 1.0;

//Info<< "\tProbe point position for patch " << mesh.boundary()[patch].name() << " is " << probePoint << nl;

// find cell cointaining the probe:
label cell_i = mesh.findCell(probePoint);

//1mではヒットしなかった場合_暫定
if (cell_i == -1)
{
	probePoint.z() = probePoint.z() + 1.0;//1m追加
	cell_i = mesh.findCell(probePoint);
}

// check at least one processor with cell.
reduce(cell_i, maxOp<label>());

// if invalid location was found, retry slightly higher
int yy = 1;
while (cell_i == -1 && yy < 50)
{
  probePoint.z() = probePoint.z() + 0.002;
  cell_i = mesh.findCell(probePoint);
  //Info<< "repositioning probe 1/10mm up for the " << yy << " time." << nl;
  yy++; 
}

// if the top boundary patch of the boundary domain is set as wall, then if we add 1m to a Z that is already at the top of the domain, we end up finding any valid cell since we are outside the domain. In that case take as reference velocity the velocity 1 meter below.
// UPDATE: after discussion with customer we will use the rule external boundary patches like sky patch, inlet outlet names will start with the word "boundary", so if boundary word found at the beginning of the patch name, the code will disregard them automatically.

if (yy == 49)  // if we are dealing with a ceiling wall
{
  probePoint.z() = probePoint.z() - 2.0; // we were 1 meter outside the domain, let's bring us back 1 meter inside.
  label cell_i = mesh.findCell(probePoint);
  reduce(cell_i, maxOp<label>());
}


//Info<< "Cell ID containing the probe is " << cell_i << nl;
// retrieve velocity magnitude at 1 meter above the center of the patch
Info<< "Velocity magnitude 1m over " << mesh.boundary()[patch].name() << " patch is " << mag(U[cell_i]) << nl;

UoneMetUp = mag(U[cell_i])+ SMALL; 

// print type and name of the patch
Info<< "\tComputing snowMelt on patch " << mesh.boundary()[patch].name() << nl;

// initialize mass of snow remaining on the patch after solar radiation
Mtot = 0.0;
// initialize volume of snow remaining on the patch after solar radiation
Vtot = 0.0;

// Loop over all faces of boundary patch
forAll(mesh.boundary()[patch], facei)
{

  //Current face cell
  //label faceCelli = mesh.boundary()[patch].faceCells()[facei];

  // Boundary cell index
  //const label& bCell = boundaryMesh[patch].faceCells()[facei];    

  // Face index
  //const label& face = boundaryMesh[patch].start() + facei;        

  // ------------------------------------------------------- //
  // debug check on first face 
  /*
  if (facei == 27)
  {
    // field value at the cell adjacent to current face cell
    Info<< "T at faceCell " << T << " is " << T[faceCelli] << nl;
  }
  */
  // ------------------------------------------------------- //

  // input radiation to snow surface 
  R_in = (1 - albedoSnow) * 
         // heat flux from visible spectral region 
         (
			qprimaryRad_0.boundaryFieldRef()[patch][facei] 
			+ 
			qprimaryRad_1.boundaryFieldRef()[patch][facei]
		 )
         +
         // heat flux from infrared spectral region
         qprimaryRad_1.boundaryFieldRef()[patch][facei] 
         * eps_Snow; // *  eps_atmosphere;  
  //Info<< "R_in " << R_in << nl;

  e_sat = 6.108 * Foam::pow(
                              10, 
                              (
                                7.5 * (T_ref-273.15)
                                /
                                (237.5 + (T_ref-273.15))
                              )
                            );  

  p_s = 101325; 

            // saturated specific humidity 
            q_sat = 0.622 * e_sat / ( p_s - 0.378 * e_sat);
   //Info<< "q_sat " << q_sat << nl;  

            // rate of change of saturated specific humidity with temperature change
            deltaSatHum = 4098.03 
                         /
                         Foam::pow(
                                    (237.3 + (T_ref-273.15) + VSMALL), 2
                                  )
                         * e_sat; 
   //Info<< "deltaSatHum " << deltaSatHum << nl;           

  // snow surface temperature
  T_s = (
          R_in - eps_Snow * sigmaSB * Foam::pow(T_ref, 4)
          - i_subl * rho_air * C_h * UoneMetUp * (1-reHu) * q_sat
        )
      /
        (
          4 * eps_Snow * sigmaSB * Foam::pow(T_ref, 3)
          + (i_subl * deltaSatHum + Cp_air) 
          * rho_air * C_h * UoneMetUp
        )
      + T_ref;
  //Info<< "T_s " << T_s << nl;

  // upward long-wavelength radiation
  L_out = eps_Snow * sigmaSB * Foam::pow(T_s, 4);
  //Info<< "L_out " << L_out << nl;


  // sensible heat flux 
  H_sensFlux = Cp_air * rho_air * C_h * UoneMetUp * (T_s-T_ref);
  //Info<< "H_sensFlux " << H_sensFlux << nl;

  // latent heat flux 
  E_latentFlux = i_subl * rho_air * C_e * UoneMetUp *
  ( (1-reHu) * q_sat + deltaSatHum * (T_s - T_ref) );
  //Info<< "E_latentFlux " << E_latentFlux << nl;

  /*
  //debug preliminary test version
  deltaZ = (5e-6) * 
            (
                qprimaryRad_0.boundaryFieldRef()[patch][facei]
              + qprimaryRad_1.boundaryFieldRef()[patch][facei] 
            );
  */

  // snow depth change due to the snow evaporation occurring during a full hour of solar radiation
  deltaZ = (R_in - L_out + H_sensFlux + E_latentFlux) * 3600
           / ( rho_s * I_c );


  //Info<< "deltaZ is " << deltaZ.value() << nl;

  // bound deltaZ
  if (deltaZ.value() < 0.0)
  {
    // Info<< "deltaZ is " << deltaZ.value() << nl;    
    deltaZ.value() = 0.0;
  }



  snowDepth.boundaryFieldRef()[patch][facei] = snowDepth.boundaryFieldRef()[patch][facei] - deltaZ.value();

  if (snowDepth.boundaryFieldRef()[patch][facei] < 0.0)
  {
    snowDepth.boundaryFieldRef()[patch][facei] = 0.0;
  }

  // compute snow mass on facei
  snowMass.boundaryFieldRef()[patch][facei] = 
    // depth of snow 
    snowDepth.boundaryFieldRef()[patch][facei] 
    // area of each face belonging to the patch
    * mesh.magSf().boundaryField()[patch][facei]
    // density of the snow
    * rho_s.value(); 

  // testing with Atot that we get the total area of the patch 
  //Atot = Atot + mesh.magSf().boundaryField()[patch][facei];

  // total mass of snow remaining on the patch after a full hour of solar radiation
  Mtot = Mtot + snowMass.boundaryFieldRef()[patch][facei]; 
  // total volume of snow remaining on the patch
  Vtot = Mtot / rho_s.value();

} 

// collect contribution from all processors
//reduce(Atot, sumOp<scalar>());
reduce(Mtot, sumOp<scalar>());
reduce(Vtot, sumOp<scalar>());
