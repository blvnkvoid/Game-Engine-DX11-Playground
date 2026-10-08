float4 ShadeBrakeLight(float4 texColor)
{
    float3 base = texColor.rgb;
    float brake = saturate(brakeAmount);
    float3 running = base * 0.6f + float3(0.10f, 0.0f, 0.0f);
    float3 brakeTint = lerp(base, float3(1.0f, 0.05f, 0.02f), brake);
    float3 brakeGlow = float3(6.0f, 0.2f, 0.08f) * brake;
    float3 color = lerp(running, brakeTint + brakeGlow, brake);
    return float4(color, 0.5f);
}