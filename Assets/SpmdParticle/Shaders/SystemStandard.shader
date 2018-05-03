Shader "SpmdParticle/SystemStandard" {

    Properties {
        _MainTex ("Base (RGB)", 2D) = "white" {}
        _Color ("Color", Color) = (0.8, 0.8, 0.8, 1.0)
        _Glossiness ("Smoothness", Range(0,1)) = 0.5
        _Metallic ("Metallic", Range(0,1)) = 0.0

        _Power ("Power", Range(0,10)) = 2.0


        _HeatColor("Heat Color", Color) = (0.25, 0.05, 0.025, 0.0)



    }

    SubShader {
        Tags { "RenderType"="Opaque" "Queue"="Geometry+1" }
        //ZWrite Off ZTest Equal // for depth prepass

        CGPROGRAM
            #pragma surface surf Standard fullforwardshadows vertex:vert addshadow 
            #pragma target 3.0

            #include "UnityCG.cginc"


            sampler2D _MainTex;
            fixed4 _Color;
            fixed4 _HeatColor;


            struct Input {
                float2 uv_MainTex;
                float4 color;
            };

            void vert(inout appdata_full v, out Input data)
            {
                UNITY_INITIALIZE_OUTPUT(Input,data);


                data.color = v.color;

            }


            half _Glossiness;
            half _Metallic;
            half _Power;

            void surf(Input IN, inout SurfaceOutputStandard o)
            {
                fixed4 c = tex2D(_MainTex, IN.uv_MainTex) * _Color;
                o.Albedo = c.rgb;
                o.Metallic = _Metallic;
                o.Smoothness = _Glossiness;
                o.Alpha = c.a;

                o.Emission += _HeatColor.rgb*IN.color.z*_Power;

            }



        ENDCG
    }

    FallBack Off
}
