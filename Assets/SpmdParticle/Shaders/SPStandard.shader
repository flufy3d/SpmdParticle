Shader "SpmdParticle/Standard" {

    Properties {
        _MainTex ("Base (RGB)", 2D) = "white" {}
        _Color ("Color", Color) = (0.8, 0.8, 0.8, 1.0)
        _Glossiness ("Smoothness", Range(0,1)) = 0.5
        _Metallic ("Metallic", Range(0,1)) = 0.0

        _HeatColor("Heat Color", Color) = (0.25, 0.05, 0.025, 0.0)
        _HeatThreshold("Heat Threshold", Float) = 2.0
        _HeatIntensity("Heat Intensity", Float) = 1.0

        g_size ("Particle Size", Float) = 0.2
        g_fade_time ("Fade Time", Float) = 0.3
        g_spin ("Spin", Float) = 0.0
    }

    SubShader {
        Tags { "RenderType"="Opaque" "Queue"="Geometry+1" }
        //ZWrite Off ZTest Equal // for depth prepass

        CGPROGRAM
            #pragma surface surf Standard fullforwardshadows vertex:vert addshadow 
            #pragma target 3.0

            #include "UnityCG.cginc"
            #include "SPParticleTrans.cginc"

            sampler2D _MainTex;
            fixed4 _Color;

            struct Input {
                float2 uv_MainTex;
                float4 velocity;
            };

            void vert(inout appdata_full v, out Input data)
            {
                UNITY_INITIALIZE_OUTPUT(Input,data);

                float4 pos;
                float4 vel;
                float4 params;
                ParticleTransform(v, pos, vel, params);

                data.velocity = vel;

            }


            half _Glossiness;
            half _Metallic;

            void surf(Input IN, inout SurfaceOutputStandard o)
            {
                fixed4 c = tex2D(_MainTex, IN.uv_MainTex) * _Color;
                o.Albedo = c.rgb;
                o.Metallic = _Metallic;
                o.Smoothness = _Glossiness;
                o.Alpha = c.a;


                float speed = IN.velocity.w;
                float ei = max(speed - _HeatThreshold, 0.0) * _HeatIntensity;
                o.Emission += _HeatColor.rgb*ei;

            }



        ENDCG
    }

    FallBack Off
}
