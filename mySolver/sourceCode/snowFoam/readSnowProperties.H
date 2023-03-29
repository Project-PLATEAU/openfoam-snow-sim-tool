    Info<< "Reading snow properties" << endl;

    IOdictionary snowProperties
    (
        IOobject
        (
            "snowProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    bool boolVal(false);
    snowProperties.readEntry("ground_and_roofs_only", boolVal);

    // snow density
    const dimensionedScalar rho_s
    (
        "rho_s", dimless, snowProperties  
    );


    // critical friction velocity 
    const dimensionedScalar Utstar
    (
        "Utstar", dimless, snowProperties  
    );

    const dimensionedScalar sigmaS
    (
        "sigmaS", dimless, snowProperties
    );

    const dimensionedScalar Wf
    (
        "Wf", dimLength/dimTime, snowProperties
    );

//    Info<< "ground_and_roofs_only " << boolVal << endl;
//    Info<< "Snow fall velocity" << Wf << " m/s"  << endl;
//    Info<< "Turbulent Schmidt number " << sigmaS << "\n" << endl;    