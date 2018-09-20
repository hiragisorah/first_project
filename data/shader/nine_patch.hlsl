Texture2D color_tex : register(t0);

SamplerState own_sampler : register(s0);

cbuffer unique : register(b0)
{
    float2 size  : packoffset(c0.x);
    float2 scale : packoffset(c0.z);
    float4 border : packoffset(c1);
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
    float2 uv_ : TEXCOORD0;
};

struct PsOut
{
    float4 color_ : SV_Target0;
};

float2 UVTransform(float2 origin)
{
    float width = size.x;
    float height = size.y;
    float borderLeft = border.x;
    float borderTop = border.y;
    float borderRight = border.z;
    float borderBottom = border.w;

    float left = borderLeft / width;
    float bottom = borderBottom / height;
    float right = borderRight / width;
    float top = borderTop / height;

    float sx = scale.x;
    float sy = scale.y;

    float2 result = origin;

    if (left + right > sx)
    {
        result.x = origin.x;
    }
    else
    {

        if (origin.x * sx < left)
        {
            result.x = origin.x * sx;
        }
        else
        {
            if ((1 - origin.x) * sx < right)
            {
                result.x = 1 - (1 - origin.x) * sx;
            }
            else
            {
                result.x = (origin.x * sx - left) / (sx - left - right) * (1 - left - right) + left;
            }
        }
    }

    if (top + bottom > sy)
    {
        result.y = origin.y;
    }
    else
    {

        if (origin.y * sy < top)
        {
            result.y = origin.y * sy;
        }
        else
        {
            if ((1 - origin.y) * sy < bottom)
            {
                result.y = 1 - (1 - origin.y) * sy;
            }
            else
            {
                result.y = (origin.y * sy - top) / (sy - top - bottom) * (1 - top - bottom) + top;
            }
        }
    }
    return result;
}

VsOut VS(uint index : SV_VertexID)
{
    VsOut output = (VsOut) 0;
 
    float2 uv = float2(index & 1, index >> 1);
    output.sv_position_ = mul(float4(uv.x * 2 - 1, -(uv.y * 2 - 1), 0, 1), mul(g_world, g_view));

    output.sv_position_.x = (output.sv_position_.x / g_view_port.x) * 2;
    output.sv_position_.y = (output.sv_position_.y / g_view_port.y) * 2;

    output.uv_ = uv;

    return output;
}

PsOut PS(VsOut input)
{
    PsOut output = (PsOut) 0;
    
    float4 tex = color_tex.Sample(own_sampler, UVTransform(input.uv_));

    output.color_ = tex * g_color;

    return output;
}