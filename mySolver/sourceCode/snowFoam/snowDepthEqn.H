// print type and name of the patch
Info<< "Computing snowDepth on patch " << mesh.boundary()[patch].name() << nl;

// initialize area of the patch
//Atot = 0.0;
// initialize mass of snow on the patch
Mtot = 0.0;
// initialize volume of snow on the patch
Vtot = 0.0;


// Loop over all faces of boundary patch
forAll(mesh.boundary()[patch], facei)
{

//Current face cell
label faceCelli = mesh.boundary()[patch].faceCells()[facei];

sqrt_k = Foam::pow(t_k[faceCelli], 0.5);

Ustar = Foam::pow
        (
          mag(
              0.4187 * Foam::pow(0.09,0.25) *
              sqrt_k * mag(UMean_[faceCelli])
            /
              (
                Foam::log
                (
                    9.793 * Foam::pow(0.09,0.25) *
                    sqrt_k * yp[patch][facei]
                  /
                    (t_nu[faceCelli] + SMALL)
                )
                + SMALL
              )
          )
        ,
          0.5
        );

if (Ustar <= Utstar.value())
{
  // deposition
  q_dep = 0.5
             * mag(snowDriftDensity.boundaryFieldRef()[patch][facei])
             * (
                 (Foam::pow(Utstar.value(), 2) - Foam::pow(Ustar, 2))
                  /
                 Foam::pow(Utstar.value(), 2)
               );
  // total
  q_tot = q_dep;

  // change in snowDepth
  deltaZ = q_tot * dtt /  rho_s.value();

  //Info<< "snowDriftDensity " << snowDriftDensity.boundaryFieldRef()[patch][facei] << nl;
}
else  
{
  // No snow deposition, snow erosion occur
  // erosion  (Ustar > Utstar.value())
  q_er = -(1.0e-5) * 916.7 * Ustar
             * ( 1 - Foam::pow(Utstar.value(), 2) /
                 (Foam::pow(Ustar, 2) + SMALL)
               );

    // Decrease in snow depth due to erosion
	deltaZ_er = abs(q_er * dtt /  rho_s.value());

	//If the amount of erosion is greater than the amount of existing snow, 
	// only the amount of existing snow is eroded.

	if ( deltaZ_er <  snowDepth.boundaryFieldRef()[patch][facei]) 
	{
		q_er = q_er;
	}
	else   
	{
		q_er = -1 * snowDepth.boundaryFieldRef()[patch][facei] * rho_s.value();
	}

   // total
  q_tot = q_er;

  // change in snowDepth
  deltaZ = q_tot * dtt /  rho_s.value();

	//reintroduce the snow contribution from erosion in the snowDrift field  
  snowDriftDensity.boundaryFieldRef()[patch][facei] = 
	snowDriftDensity.boundaryFieldRef()[patch][facei]
  + q_tot * dtt /(Foam::pow(mesh.magSf().boundaryField()[patch][facei], 0.5));

}


// test snowDriftDensity patch variation

snowDepth.boundaryFieldRef()[patch][facei] = snowDepth.boundaryFieldRef()[patch][facei] + deltaZ;

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

// total mass of snow on patch
Mtot = Mtot + snowMass.boundaryFieldRef()[patch][facei];
// total volume of snow on patch
Vtot = Mtot / rho_s.value();

}

// collect contribution from all processors
//reduce(Atot, sumOp<scalar>());
reduce(Mtot, sumOp<scalar>());
reduce(Vtot, sumOp<scalar>());
//Info<< "\tTotal Area for patch " << mesh.boundary()[patch].name() << " is " << Atot <<  nl << nl;
