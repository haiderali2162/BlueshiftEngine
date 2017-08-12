shader "Lit/PhongDirectLit" {
    litSurface
    inheritProperties "Phong.shader"
    
    generatePerforatedVersion
    generatePremulAlphaVersion
    generateGpuSkinningVersion
    generateParallelShadowVersion
    generateSpotShadowVersion
    generatePointShadowVersion

    glsl_vp {
        #define LEGACY_PHONG_LIGHTING
        #define DIRECT_LIGHTING 1
        $include "StandardCore.vp"
    }
    glsl_fp {
        #define LEGACY_PHONG_LIGHTING
        #define DIRECT_LIGHTING 1
        $include "StandardCore.fp"
    }
}
