float4 ShadeHeadLight(float4 texColor)
{
    float3 base = texColor.rgb;

    // Dim DRL / parking light
    float3 running = base * 0.35f;

    // Bright white projector
    float3 beam = float3(25.0f, 24.0f, 20.0f);

    // Slightly blue HID tint
    beam *= float3(0.95f, 0.98f, 1.05f);

    float3 color = running + beam;

    return float4(color, 1.0f);
}