{

	const volVectorField gradN = fvc::grad(turbulence->nut());
	const volVectorField gradS = fvc::grad(snowDriftDensity);
	const volTensorField tensorS = fvc::grad(gradS);

  // test if tensor is symmetric:
  //Info<< "tensorS is " << tensorS << endl;
  // it is non symmetric

    fvScalarMatrix snowDriftDensityEqn
    (
	    fvm::ddt(snowDriftDensity) 
	    + 1 / rho * fvm::div(phi, snowDriftDensity)
	    + Wf*gradS.component(vector::Z)
	  ==
	    1.0 / sigmaS *
	    (
            gradN.component(vector::X) * gradS.component(vector::X) + turbulence->nut() * tensorS.component(tensor::XX)
          + gradN.component(vector::X) * gradS.component(vector::Y) + turbulence->nut() * tensorS.component(tensor::XY)
          + gradN.component(vector::X) * gradS.component(vector::Z) + turbulence->nut() * tensorS.component(tensor::XZ)

          + gradN.component(vector::Y) * gradS.component(vector::X) + turbulence->nut() * tensorS.component(tensor::YX)
          + gradN.component(vector::Y) * gradS.component(vector::Y) + turbulence->nut() * tensorS.component(tensor::YY)
          + gradN.component(vector::Y) * gradS.component(vector::Z) + turbulence->nut() * tensorS.component(tensor::YZ)

          + gradN.component(vector::Z) * gradS.component(vector::X) + turbulence->nut() * tensorS.component(tensor::ZX)
          + gradN.component(vector::Z) * gradS.component(vector::Y) + turbulence->nut() * tensorS.component(tensor::ZY)
          + gradN.component(vector::Z) * gradS.component(vector::Z) + turbulence->nut() * tensorS.component(tensor::ZZ)
	    )   
	  );

    snowDriftDensityEqn.relax();
    snowDriftDensityEqn.solve();

    forAll (mesh.cells(),celli)
    {
      if (snowDriftDensity[celli] < 0.0)
      snowDriftDensity[celli] = 0.0;
    }

    /*
    forAll(mesh.boundary(), patchID) 
    {
        forAll (mesh.boundary()[patchID],facei) 
        {
             if (snowDriftDensity.boundaryFieldRef()[patchID][facei] < 0.0)
             {
                snowDriftDensity.boundaryFieldRef()[patchID][facei] = 0.0;
             }           
        }
    }
    */

}
