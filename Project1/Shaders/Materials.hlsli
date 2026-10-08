
// ---------------------------------------------------------
// T E S T (asphalt)
// ---------------------------------------------------------

/*if (matType == MATERIAL_ASPHALT)
{
    return ShadeAsphalt(
        texColor,
        N,
        L,
        input.worldPos,
        ambientIntensity,
        headlightIntensity,
        H,
        V,
        shadowFactor
    );
}*/


// ---------------------------------------------------------
// Emissive materials
// ---------------------------------------------------------


if (matType == MATERIAL_BRAKE_LIGHT)
{
    return ShadeBrakeLight(
        texColor
    );
}


if (matType == MATERIAL_HEAD_LIGHT)
{
    return ShadeHeadLight(
        texColor
    );
}


if (matType == MATERIAL_PACE_LIGHT)
{
    return ShadePaceLight(
        texColor
    );
}


if (matType == MATERIAL_LAMP)
{
    return ShadeLampGlow(
        texColor,
        input.texCoord
    );
}


// ---------------------------------------------------------
// Specialized sunlight-driven materials
// ---------------------------------------------------------

if (matType == MATERIAL_SOLID_PAINT)
{
    return ShadeCarPaint(
        texColor,
        N,
        L,
        V,
        H,
        R,
        input.worldPos
    );
}


if (matType == MATERIAL_GLASS)
{
    return ShadeGlass(
        texColor,
        N,
        L,
        V,
        H,
        R
    );
}


if (matType == MATERIAL_RUBBER)
{
    return ShadeRubber(
        texColor,
        N,
        L
    );
}





if (matType == MATERIAL_LIVERY)
{
    return ShadeCarLivery(
        texColor,
        N,
        L,
        V,
        H,
        R,
        localLighting
    );
}


if (matType == MATERIAL_ALCANTARA)
{
    return ShadeAlcantara(
        texColor,
        N,
        L
    );
}


if (matType == MATERIAL_DECAL_TEXT)
{
    return ShadeDecalText(
        texColor,
        N,
        L
    );
}


if (matType == MATERIAL_SAFETYCAR_PAINT)
{
    return ShadeSafetyCarPaint(
        texColor,
        N,
        L,
        V,
        H,
        R,
        input.localPos
    );
}

// ---------------------------------------------------------
// Alpha-tested trees
// ---------------------------------------------------------

if (matType == MATERIAL_TREE)
{
    clip(
        texColor.a - 0.01f
    );

    return float4(texColor.rgb, 1.0f);
}
