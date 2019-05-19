
float4x4 viewMatrix;
float4x4 projectionMatrix;

float4x4 worldMatrix;

float3 viewPosition;

struct VSINPUT
{
	float4 Pos : POSITION;
	float4 Normal : NORMAL;
	float4 Color : COLOR;
	float2 UV : TEXCOORD0;
	// インスタンシング
	float4 worldPos : TEXCOORD1;
};

texture Texture;
sampler DecaleSamp = sampler_state
{
	Texture = <Texture>;
	MinFilter = LINEAR;
	MagFilter = LINEAR;
	MipFilter = NONE;

	AddressU = WRAP;
	AddressV = WRAP;
};

VSINPUT VS( VSINPUT In )
{
	VSINPUT Out = ( VSINPUT ) 0;

	Out.Pos = mul( In.Pos, worldMatrix );
	Out.Pos += In.worldPos;

	float4x4 VPMatrix;
	VPMatrix = mul( viewMatrix, projectionMatrix );

	Out.Pos = mul( Out.Pos, VPMatrix );

	Out.UV = In.UV;
	Out.Normal = In.Normal;
	Out.Color = In.Color;
	return Out;
}

float4 PS( VSINPUT In ) : COLOR
{
	float4 Out = 1;
	In.Color = min( 1, In.Color );
	Out.rgb *= tex2D( DecaleSamp, In.UV );
	Out.rgb *= In.Color;
	return In.Color;
}

technique linecopy
{
	pass P0
	{
		AlphaBlendEnable = true;
		BlendOp = Add;
		SrcBlend = SrcAlpha;
		DestBlend = InvSrcAlpha;
		CullMode = CCW;

		// シェーダ
		VertexShader = compile vs_3_0 VS();
		PixelShader = compile ps_3_0 PS();
	}
}
