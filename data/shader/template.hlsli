cbuffer per_camera : register(b1)
{
    row_major matrix g_view : packoffset(c0);
    row_major matrix g_proj : packoffset(c4);
    float4 g_color : packoffset(c8);
    float2 g_view_port : packoffset(c9);
};

cbuffer per_mesh : register(b2)
{
    row_major matrix g_world : packoffset(c0);
    float4 g_diffuse : packoffset(c4);
};