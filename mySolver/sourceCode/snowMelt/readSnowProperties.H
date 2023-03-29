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

// user input constant in alphabetical order

    // albedo of the snow
    dimensionedScalar albedoSnow
    (
        "albedoSnow", dimless, snowProperties
    );

    // bulk transport coefficient of sensible heat 
    dimensionedScalar C_e
    (
        "C_e", dimless, snowProperties
    );
    dimensionedScalar C_h
    (
        "C_h", dimless, snowProperties
    );

    // specific heat of air
    dimensionedScalar Cp_air
    (
        "Cp_air", dimless, snowProperties
    );

/*
    // emissivity of atmosphere
    dimensionedScalar eps_atmosphere
    (
        "eps_atmosphere", dimless, snowProperties
    );
*/

    // emissivity of snow surface
    dimensionedScalar eps_Snow
    (
        "eps_Snow", dimless, snowProperties
    );

    bool boolVal(false);
    snowProperties.readEntry("ground_and_roofs_only", boolVal);

    // heat of fusion of ice 
    const dimensionedScalar I_c
    (
        "I_c", dimless, snowProperties  
    );

    // Latent heat for ice sublimation 
    const dimensionedScalar i_subl
    (
        "i_subl", dimless, snowProperties  
    );
    //Info<< "Latent heat for ice sublimation " << i_subl << "\n" << endl;

    // Turbulent Schmidt number
    dimensionedScalar sigmaS
    (
        "sigmaS", dimless, snowProperties
    );
//    Info<< "Turbulent Schmidt number " << sigmaS << "\n" << endl;    

    // relative humidity 
    dimensionedScalar reHu
    (
        "reHu", dimless, snowProperties
    );

    // air density
    const dimensionedScalar rho_air
    (
        "rho_air", dimless, snowProperties  
    );

    // snow density
    const dimensionedScalar rho_s
    (
        "rho_s", dimless, snowProperties  
    );

    // reference temperature of the air 
    const dimensionedScalar T_ref
    (
        "T_ref", dimless, snowProperties  
    );


    // Snow fall velocity
    const dimensionedScalar Wf
    (
        "Wf", dimLength/dimTime, snowProperties
    );
//    Info<< "Snow fall velocity" << Wf << " m/s"  << endl;
