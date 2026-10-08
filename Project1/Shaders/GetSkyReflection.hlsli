float3 GetSkyReflection(float3 R)
{
    float skyFactor = saturate(R.y * 0.5f + 0.5f);
    return lerp(float3(0.04f, 0.04f, 0.06f), float3(0.65f, 0.78f, 1.0f), skyFactor);
}