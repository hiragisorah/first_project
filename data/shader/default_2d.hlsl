#include "template.hlsli"

Texture2D color_tex : register(t0);

SamplerState own_sampler : register(s0);

struct VsOut
{
    float4 sv_position_ : SV_Position;
    float2 uv_ : TEXCOORD0;
};

struct PsOut
{
    float4 color_ : SV_Target0;
};

VsOut VS(uint index : SV_VertexID)
{
    VsOut output = (VsOut) 0;
 
    float2 uv = float2(index & 1, index >> 1);
    output.sv_position_ = mul(float4(uv.x * 2 - 1, -(uv.y * 2 - 1), 0, 1), mul(g_world, g_view));

    output.sv_position_.x = (output.sv_position_.x / g_view_port.x) * .5f;
    output.sv_position_.y = (output.sv_position_.y / g_view_port.y) * .5f;

    output.uv_ = uv;

    return output;
}

PsOut PS(VsOut input)
{
    PsOut output = (PsOut) 0;
    
    float4 tex = color_tex.Sample(own_sampler, input.uv_);

    output.color_ = tex * g_diffuse * g_color;
    return output;
}