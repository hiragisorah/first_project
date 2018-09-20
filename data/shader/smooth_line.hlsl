Texture2D color_tex : register(t0);

SamplerState own_sampler : register(s0);

cbuffer unique : register(b0)
{
    float2 g_start : packoffset(c0.x);
    float2 g_end : packoffset(c0.z);
    float2 g_size : packoffset(c1.x);

    float g_vtx_cnt : packoffset(c2.x);
};

cbuffer main : register(b1)
{
    row_major matrix g_world : packoffset(c0);
    row_major matrix g_view : packoffset(c4);
    row_major matrix g_proj : packoffset(c8);
    float2 g_view_port : packoffset(c12);
    float4 g_color : packoffset(c13);
};

struct VsOut
{
    float4 sv_position_ : SV_Position;
};

struct GsOut
{
    float4 sv_position_ : SV_Position;
    float2 uv_ : TEXCOORD;
};

struct PsOut
{
    float4 color_ : SV_Target0;
};

VsOut VS(uint index : SV_VertexID)
{
    VsOut output = (VsOut) 0;

    float4 position = float4(0, 0, 0, 1);

    float progress = index / (g_vtx_cnt - 1);
    
    float2 st = smoothstep(float2(.5f, 0.2f), float2(0.8f, 0.6f), float2(progress, progress));

    position.xy = g_start + (g_end - g_start) * st;

    output.sv_position_ = position;

    return output;
}

[maxvertexcount(4)]
void GS(line VsOut input[2], inout TriangleStream<GsOut> out_stream)
{
    GsOut output = (GsOut)0;
    
    float w = 0;

    float2 dif = normalize(input[0].sv_position_.xy - input[1].sv_position_.xy);
    float ang = atan2(dif.y, dif.x) + 1.57f;
    dif = float2(cos(ang), sin(ang));
    float2 up = normalize(dif) * g_size;

    output.sv_position_ = input[0].sv_position_ + float4(-up, 0, 0);
    output.sv_position_ = mul(output.sv_position_, g_view);
    output.sv_position_.x = (output.sv_position_.x / g_view_port.x) * 2;
    output.sv_position_.y = (output.sv_position_.y / g_view_port.y) * 2;
    w = output.sv_position_.w;
    output.uv_ = float2(0, 0);
    out_stream.Append(output);

    output.sv_position_ = input[1].sv_position_ + float4(-up, 0, 0);
    output.sv_position_ = mul(output.sv_position_, g_view);
    output.sv_position_.x = (output.sv_position_.x / g_view_port.x) * 2;
    output.sv_position_.y = (output.sv_position_.y / g_view_port.y) * 2;
    output.sv_position_.w = w;
    output.uv_ = float2(1, 0);
    out_stream.Append(output);

    output.sv_position_ = input[0].sv_position_ + float4(+up, 0, 0);
    output.sv_position_ = mul(output.sv_position_, g_view);
    output.sv_position_.x = (output.sv_position_.x / g_view_port.x) * 2;
    output.sv_position_.y = (output.sv_position_.y / g_view_port.y) * 2;
    output.sv_position_.w = w;
    output.uv_ = float2(0, 1);
    out_stream.Append(output);

    output.sv_position_ = input[1].sv_position_ + float4(+up, 0, 0);
    output.sv_position_ = mul(output.sv_position_, g_view);
    output.sv_position_.x = (output.sv_position_.x / g_view_port.x) * 2;
    output.sv_position_.y = (output.sv_position_.y / g_view_port.y) * 2;
    output.sv_position_.w = w;
    output.uv_ = float2(1, 1);
    out_stream.Append(output);

    out_stream.RestartStrip();
}

PsOut PS(GsOut input)
{
    PsOut output = (PsOut) 0;
    
    float4 tex = color_tex.Sample(own_sampler, input.uv_) * g_color;

    output.color_ = tex;

    return output;
}