float4 ShadeGlass(float4 texColor, float3 N, float3 L, float3 V, float3 H, float3 R)
{
    float fresnel = pow(1.0f - saturate(dot(N, V)), 3.0f);
    float ndotv = saturate(abs(dot(N, V)));
    float alpha = 0.06f + (1.0f - ndotv) * 0.12f;

    float3 tint = float3(0.03f, 0.04f, 0.05f);
    float3 sky = 0; //GetSkyReflection(R) * (0.35f + fresnel * 0.9f);

    float spec = pow(saturate(dot(N, H)), 256.0f) * 1.5f;

    float3 color = tint + sky + spec.xxx;

    return float4(0, 0, 0, alpha);
}