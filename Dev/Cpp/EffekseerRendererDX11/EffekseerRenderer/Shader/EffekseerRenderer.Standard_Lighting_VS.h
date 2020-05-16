#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.29.952.3111
//
//
//   fxc /Zpc /Tvs_4_0 /EVS /D __EFFEKSEER_BUILD_VERSION16__=1 /Fh
//    Shader/EffekseerRenderer.Standard_Lighting_VS.h
//    Shader/standard_renderer_lighting_VS.fx
//
//
// Buffer Definitions: 
//
// cbuffer VS_ConstantBuffer
// {
//
//   float4x4 mCamera;                  // Offset:    0 Size:    64
//   float4x4 mProj;                    // Offset:   64 Size:    64
//   float4 mUVInversed;                // Offset:  128 Size:    16
//   float4 mflipbookParameter;         // Offset:  144 Size:    16
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// VS_ConstantBuffer                 cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// POSITION                 0   xyz         0     NONE  float   xyz 
// NORMAL                   0   xyzw        1     NONE  float   xyzw
// NORMAL                   1   xyzw        2     NONE  float   xyz 
// NORMAL                   2   xyzw        3     NONE  float   xyz 
// TEXCOORD                 0   xy          4     NONE  float   xy  
// TEXCOORD                 1   xy          5     NONE  float       
// TEXCOORD                 2   xy          6     NONE  float   xy  
// TEXCOORD                 3   x           7     NONE  float   x   
// TEXCOORD                 4   x           8     NONE  float   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue Format   Used
// -------------------- ----- ------ -------- -------- ------ ------
// SV_POSITION              0   xyzw        0      POS  float   xyzw
// COLOR                    0   xyzw        1     NONE  float   xyzw
// TEXCOORD                 0   xy          2     NONE  float   xy  
// TEXCOORD                 1     zw        2     NONE  float     zw
// TEXCOORD                 2   xyz         3     NONE  float   xyz 
// TEXCOORD                 8      w        3     NONE  float      w
// TEXCOORD                 3   xyz         4     NONE  float   xyz 
// TEXCOORD                 4   xyz         5     NONE  float   xyz 
// TEXCOORD                 5   xyz         6     NONE  float   xyz 
// TEXCOORD                 6   xy          7     NONE  float   xy  
// TEXCOORD                 7     zw        7     NONE  float     zw
// TEXCOORD                 9   xy          8     NONE  float   xy  
// TEXCOORD                10     z         8     NONE  float     z 
//
vs_4_0
dcl_constantbuffer cb0[10], immediateIndexed
dcl_input v0.xyz
dcl_input v1.xyzw
dcl_input v2.xyz
dcl_input v3.xyz
dcl_input v4.xy
dcl_input v6.xy
dcl_input v7.x
dcl_input v8.x
dcl_output_siv o0.xyzw, position
dcl_output o1.xyzw
dcl_output o2.xy
dcl_output o2.zw
dcl_output o3.xyz
dcl_output o3.w
dcl_output o4.xyz
dcl_output o5.xyz
dcl_output o6.xyz
dcl_output o7.xy
dcl_output o7.zw
dcl_output o8.xy
dcl_output o8.z
dcl_temps 7
add r0.xyz, v2.xyzx, l(-0.500000, -0.500000, -0.500000, 0.000000)
add r0.xyz, r0.xyzx, r0.xyzx
add r1.xyz, v3.xyzx, l(-0.500000, -0.500000, -0.500000, 0.000000)
add r1.xyz, r1.xyzx, r1.xyzx
mul r2.xyz, r0.zxyz, r1.yzxy
mad o6.xyz, r0.yzxy, r1.zxyz, -r2.xyzx
mad o2.yw, cb0[8].yyyy, v4.yyyy, cb0[8].xxxx
mad o7.w, cb0[8].y, v6.y, cb0[8].x
lt r0.w, l(0.000000), cb0[9].x
if_nz r0.w
  frc r2.x, v7.x
  round_ni r3.x, v7.x
  add r0.w, v7.x, l(1.000000)
  eq r1.w, cb0[9].y, l(0.000000)
  if_nz r1.w
    mul r1.w, cb0[9].w, cb0[9].z
    ge r1.w, r0.w, r1.w
    mad r3.w, cb0[9].z, cb0[9].w, l(-1.000000)
    add r3.z, v7.x, l(1.000000)
    movc r3.xy, r1.wwww, r3.wwww, r3.xzxx
  else 
    mul r1.w, cb0[9].w, cb0[9].z
    mul r3.z, r0.w, r1.w
    ge r3.z, r3.z, -r3.z
    movc r3.z, r3.z, r1.w, -r1.w
    div r3.w, l(1.000000, 1.000000, 1.000000, 1.000000), r3.z
    mul r3.w, r0.w, r3.w
    frc r3.w, r3.w
    mul r3.z, r3.w, r3.z
    eq r4.xy, cb0[9].yyyy, l(1.000000, 2.000000, 0.000000, 0.000000)
    round_ni r3.w, r0.w
    div r3.w, r3.w, r1.w
    add r4.z, r3.w, r3.w
    ge r4.z, r4.z, -r4.z
    movc r4.zw, r4.zzzz, l(0,0,2.000000,0.500000), l(0,0,-2.000000,-0.500000)
    mul r3.w, r3.w, r4.w
    frc r3.w, r3.w
    mul r3.w, r3.w, r4.z
    eq r3.w, r3.w, l(1.000000)
    round_z r4.z, r0.w
    mul r4.w, r1.w, r4.z
    ge r4.w, r4.w, -r4.w
    movc r1.w, r4.w, r1.w, -r1.w
    div r4.w, l(1.000000, 1.000000, 1.000000, 1.000000), r1.w
    mul r4.z, r4.w, r4.z
    frc r4.z, r4.z
    mul r4.w, r1.w, r4.z
    mad r5.x, cb0[9].z, cb0[9].w, l(-1.000000)
    mad r1.w, -r1.w, r4.z, r5.x
    movc r1.w, r3.w, r1.w, r4.w
    movc r0.w, r4.y, r1.w, r0.w
    movc r3.y, r4.x, r3.z, r0.w
  endif 
  ftoi r3.xy, r3.xyxx
  ftoi r3.zw, cb0[9].zzzw
  and r4.xy, r3.xyxx, l(0x80000000, 0x80000000, 0, 0)
  imax r4.zw, r3.xxxy, -r3.xxxy
  imax r5.xy, r3.zwzz, -r3.zwzz
  udiv null, r5.xz, r4.zzwz, r5.xxxx
  ineg r6.xy, r5.xzxx
  movc r4.xy, r4.xyxx, r6.xyxx, r5.xzxx
  itof r6.xy, r4.xyxx
  xor r3.xy, r3.wwww, r3.xyxx
  udiv r3.zw, null, r4.zzzw, r5.yyyy
  ineg r4.xy, r3.zwzz
  and r3.xy, r3.xyxx, l(0x80000000, 0x80000000, 0, 0)
  movc r3.xy, r3.xyxx, r4.xyxx, r3.zwzz
  itof r6.zw, r3.xxxy
  div r3.xy, l(1.000000, 1.000000, 1.000000, 1.000000), cb0[9].zwzz
  mul r4.xyzw, r3.xyxy, r6.xzyw
  mul r3.zw, -r4.xxxy, cb0[9].zzzw
  mad r3.xy, r3.zwzz, r3.xyxx, r4.zwzz
else 
  mov r3.xy, l(0,0,0,0)
  mov r2.x, l(0)
endif 
mul r4.xyzw, v0.yyyy, cb0[1].xyzw
mad r4.xyzw, cb0[0].xyzw, v0.xxxx, r4.xyzw
mad r4.xyzw, cb0[2].xyzw, v0.zzzz, r4.xyzw
add r4.xyzw, r4.xyzw, cb0[3].xyzw
div r4.xyzw, r4.xyzw, r4.wwww
mul r5.xyzw, r4.yyyy, cb0[5].xyzw
mad r5.xyzw, cb0[4].xyzw, r4.xxxx, r5.xyzw
mad r5.xyzw, cb0[6].xyzw, r4.zzzz, r5.xyzw
mad r4.xyzw, cb0[7].xyzw, r4.wwww, r5.xyzw
div r5.xy, r4.xyxx, r4.wwww
mad r5.xy, r5.xyxx, l(1.000000, -1.000000, 0.000000, 0.000000), l(1.000000, 1.000000, 0.000000, 0.000000)
mul o7.xy, r5.xyxx, l(0.500000, 0.500000, 0.000000, 0.000000)
mov o0.xyzw, r4.xyzw
mov o1.xyzw, v1.xyzw
mov o2.xz, v4.xxxx
mov r2.yzw, v0.xxyz
mov o3.xyzw, r2.yzwx
mov o7.z, v6.x
mov o4.xyz, r0.xyzx
mov o5.xyz, r1.xyzx
mov r3.z, v8.x
mov o8.xyz, r3.xyzx
ret 
// Approximately 99 instruction slots used
#endif

const BYTE g_VS[] =
{
     68,  88,  66,  67,  68, 239, 
      7, 200, 177,  13, 179, 131, 
     90,  27, 169, 186, 174, 235, 
     22, 121,   1,   0,   0,   0, 
    244,  16,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    132,   1,   0,   0, 136,   2, 
      0,   0, 236,   3,   0,   0, 
    120,  16,   0,   0,  82,  68, 
     69,  70,  72,   1,   0,   0, 
      1,   0,   0,   0,  80,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   0,   0,   0,   0,   4, 
    254, 255,  16,   1,   0,   0, 
     23,   1,   0,   0,  60,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     86,  83,  95,  67, 111, 110, 
    115, 116,  97, 110, 116,  66, 
    117, 102, 102, 101, 114,   0, 
    171, 171,  60,   0,   0,   0, 
      4,   0,   0,   0, 104,   0, 
      0,   0, 160,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 200,   0,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    208,   0,   0,   0,   0,   0, 
      0,   0, 224,   0,   0,   0, 
     64,   0,   0,   0,  64,   0, 
      0,   0,   2,   0,   0,   0, 
    208,   0,   0,   0,   0,   0, 
      0,   0, 230,   0,   0,   0, 
    128,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    244,   0,   0,   0,   0,   0, 
      0,   0,   4,   1,   0,   0, 
    144,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    244,   0,   0,   0,   0,   0, 
      0,   0, 109,  67,  97, 109, 
    101, 114,  97,   0,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 109,  80, 114, 111, 
    106,   0, 109,  85,  86,  73, 
    110, 118, 101, 114, 115, 101, 
    100,   0, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 109, 102, 108, 105, 
    112,  98, 111, 111, 107,  80, 
     97, 114,  97, 109, 101, 116, 
    101, 114,   0,  77, 105,  99, 
    114, 111, 115, 111, 102, 116, 
     32,  40,  82,  41,  32,  72, 
     76,  83,  76,  32,  83, 104, 
     97, 100, 101, 114,  32,  67, 
    111, 109, 112, 105, 108, 101, 
    114,  32,  57,  46,  50,  57, 
     46,  57,  53,  50,  46,  51, 
     49,  49,  49,   0,  73,  83, 
     71,  78, 252,   0,   0,   0, 
      9,   0,   0,   0,   8,   0, 
      0,   0, 224,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   7,   7, 
      0,   0, 233,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  15,  15, 
      0,   0, 233,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,  15,   7, 
      0,   0, 233,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      3,   0,   0,   0,  15,   7, 
      0,   0, 240,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      4,   0,   0,   0,   3,   3, 
      0,   0, 240,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      5,   0,   0,   0,   3,   0, 
      0,   0, 240,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      6,   0,   0,   0,   3,   3, 
      0,   0, 240,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   0,   0,   0,   1,   1, 
      0,   0, 240,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   0,   0,   0,   1,   1, 
      0,   0,  80,  79,  83,  73, 
     84,  73,  79,  78,   0,  78, 
     79,  82,  77,  65,  76,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171, 171, 171, 
     79,  83,  71,  78,  92,   1, 
      0,   0,  13,   0,   0,   0, 
      8,   0,   0,   0,  64,   1, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  76,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0,  82,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,  12,   0,   0,  82,   1, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     12,   3,   0,   0,  82,   1, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      7,   8,   0,   0,  82,   1, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
      8,   7,   0,   0,  82,   1, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
      7,   8,   0,   0,  82,   1, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   5,   0,   0,   0, 
      7,   8,   0,   0,  82,   1, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   6,   0,   0,   0, 
      7,   8,   0,   0,  82,   1, 
      0,   0,   6,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   7,   0,   0,   0, 
      3,  12,   0,   0,  82,   1, 
      0,   0,   7,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   7,   0,   0,   0, 
     12,   3,   0,   0,  82,   1, 
      0,   0,   9,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
      3,  12,   0,   0,  82,   1, 
      0,   0,  10,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   8,   0,   0,   0, 
      4,  11,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  67,  79, 
     76,  79,  82,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0, 171,  83,  72,  68,  82, 
    132,  12,   0,   0,  64,   0, 
      1,   0,  33,   3,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
     10,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      0,   0,   0,   0,  95,   0, 
      0,   3, 242,  16,  16,   0, 
      1,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      2,   0,   0,   0,  95,   0, 
      0,   3, 114,  16,  16,   0, 
      3,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      4,   0,   0,   0,  95,   0, 
      0,   3,  50,  16,  16,   0, 
      6,   0,   0,   0,  95,   0, 
      0,   3,  18,  16,  16,   0, 
      7,   0,   0,   0,  95,   0, 
      0,   3,  18,  16,  16,   0, 
      8,   0,   0,   0, 103,   0, 
      0,   4, 242,  32,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    194,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    130,  32,  16,   0,   3,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   4,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   5,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   6,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   7,   0, 
      0,   0, 101,   0,   0,   3, 
    194,  32,  16,   0,   7,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   8,   0, 
      0,   0, 101,   0,   0,   3, 
     66,  32,  16,   0,   8,   0, 
      0,   0, 104,   0,   0,   2, 
      7,   0,   0,   0,   0,   0, 
      0,  10, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,  18, 
     16,   0,   2,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 191,   0,   0,   0, 191, 
      0,   0,   0, 191,   0,   0, 
      0,   0,   0,   0,   0,   7, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,  10, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  18,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0, 191,   0,   0, 
      0, 191,   0,   0,   0, 191, 
      0,   0,   0,   0,   0,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  38,   9,  16,   0, 
      0,   0,   0,   0, 150,   4, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  10, 114,  32, 
     16,   0,   6,   0,   0,   0, 
    150,   4,  16,   0,   0,   0, 
      0,   0,  38,   9,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16, 128,  65,   0,   0,   0, 
      2,   0,   0,   0,  50,   0, 
      0,  11, 162,  32,  16,   0, 
      2,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      8,   0,   0,   0,  86,  21, 
     16,   0,   4,   0,   0,   0, 
      6, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     50,   0,   0,  11, 130,  32, 
     16,   0,   7,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     26,  16,  16,   0,   6,   0, 
      0,   0,  10, 128,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  49,   0,   0,   8, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  31,   0, 
      4,   3,  58,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
      0,   5,  18,   0,  16,   0, 
      2,   0,   0,   0,  10,  16, 
     16,   0,   7,   0,   0,   0, 
     65,   0,   0,   5,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     10,  16,  16,   0,   7,   0, 
      0,   0,   0,   0,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      7,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     24,   0,   0,   8, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  31,   0,   4,   3, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   9, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  29,   0,   0,   7, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  11, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     42, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,   9,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128, 191,   0,   0,   0,   7, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  10,  16,  16,   0, 
      7,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,   9,  50,   0, 
     16,   0,   3,   0,   0,   0, 
    246,  15,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      3,   0,   0,   0, 134,   0, 
     16,   0,   3,   0,   0,   0, 
     18,   0,   0,   1,  56,   0, 
      0,   9, 130,   0,  16,   0, 
      1,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  56,   0, 
      0,   7,  66,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  29,   0,   0,   8, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  55,   0, 
      0,  10,  66,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  14,   0,   0,  10, 
    130,   0,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  26,   0, 
      0,   5, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      3,   0,   0,   0,  24,   0, 
      0,  11,  50,   0,  16,   0, 
      4,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0,   0,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     65,   0,   0,   5, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   7, 
    130,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   7,  66,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  29,   0, 
      0,   8,  66,   0,  16,   0, 
      4,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16, 128,  65,   0, 
      0,   0,   4,   0,   0,   0, 
     55,   0,   0,  15, 194,   0, 
     16,   0,   4,   0,   0,   0, 
    166,  10,  16,   0,   4,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,  64, 
      0,   0,   0,  63,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0, 192,   0,   0,   0, 191, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
      0,   5, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   3,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,  24,   0, 
      0,   7, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  67,   0,   0,   5, 
     66,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  29,   0,   0,   8, 
    130,   0,  16,   0,   4,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  58,   0, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,  55,   0, 
      0,  10, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,  14,   0,   0,  10, 
    130,   0,  16,   0,   4,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7,  66,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   4,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,  26,   0, 
      0,   5,  66,   0,  16,   0, 
      4,   0,   0,   0,  42,   0, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,   7, 130,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  42,   0,  16,   0, 
      4,   0,   0,   0,  50,   0, 
      0,  11,  18,   0,  16,   0, 
      5,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128, 191, 
     50,   0,   0,  10, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
     42,   0,  16,   0,   4,   0, 
      0,   0,  10,   0,  16,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9, 130,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   3,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      4,   0,   0,   0,  55,   0, 
      0,   9, 130,   0,  16,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   4,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  55,   0, 
      0,   9,  34,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   4,   0,   0,   0, 
     42,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  21,   0, 
      0,   1,  27,   0,   0,   5, 
     50,   0,  16,   0,   3,   0, 
      0,   0,  70,   0,  16,   0, 
      3,   0,   0,   0,  27,   0, 
      0,   6, 194,   0,  16,   0, 
      3,   0,   0,   0, 166, 142, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,   1,   0, 
      0,  10,  50,   0,  16,   0, 
      4,   0,   0,   0,  70,   0, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0, 128,   0,   0,   0, 128, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  36,   0,   0,   8, 
    194,   0,  16,   0,   4,   0, 
      0,   0,   6,   4,  16,   0, 
      3,   0,   0,   0,   6,   4, 
     16, 128,  65,   0,   0,   0, 
      3,   0,   0,   0,  36,   0, 
      0,   8,  50,   0,  16,   0, 
      5,   0,   0,   0, 230,  10, 
     16,   0,   3,   0,   0,   0, 
    230,  10,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     78,   0,   0,   8,   0, 208, 
      0,   0,  82,   0,  16,   0, 
      5,   0,   0,   0, 166,  11, 
     16,   0,   4,   0,   0,   0, 
      6,   0,  16,   0,   5,   0, 
      0,   0,  40,   0,   0,   5, 
     50,   0,  16,   0,   6,   0, 
      0,   0, 134,   0,  16,   0, 
      5,   0,   0,   0,  55,   0, 
      0,   9,  50,   0,  16,   0, 
      4,   0,   0,   0,  70,   0, 
     16,   0,   4,   0,   0,   0, 
     70,   0,  16,   0,   6,   0, 
      0,   0, 134,   0,  16,   0, 
      5,   0,   0,   0,  43,   0, 
      0,   5,  50,   0,  16,   0, 
      6,   0,   0,   0,  70,   0, 
     16,   0,   4,   0,   0,   0, 
     87,   0,   0,   7,  50,   0, 
     16,   0,   3,   0,   0,   0, 
    246,  15,  16,   0,   3,   0, 
      0,   0,  70,   0,  16,   0, 
      3,   0,   0,   0,  78,   0, 
      0,   8, 194,   0,  16,   0, 
      3,   0,   0,   0,   0, 208, 
      0,   0, 166,  14,  16,   0, 
      4,   0,   0,   0,  86,   5, 
     16,   0,   5,   0,   0,   0, 
     40,   0,   0,   5,  50,   0, 
     16,   0,   4,   0,   0,   0, 
    230,  10,  16,   0,   3,   0, 
      0,   0,   1,   0,   0,  10, 
     50,   0,  16,   0,   3,   0, 
      0,   0,  70,   0,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0, 128, 
      0,   0,   0, 128,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     55,   0,   0,   9,  50,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   0,  16,   0,   3,   0, 
      0,   0,  70,   0,  16,   0, 
      4,   0,   0,   0, 230,  10, 
     16,   0,   3,   0,   0,   0, 
     43,   0,   0,   5, 194,   0, 
     16,   0,   6,   0,   0,   0, 
      6,   4,  16,   0,   3,   0, 
      0,   0,  14,   0,   0,  11, 
     50,   0,  16,   0,   3,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
      0,   0, 128,  63, 230, 138, 
     32,   0,   0,   0,   0,   0, 
      9,   0,   0,   0,  56,   0, 
      0,   7, 242,   0,  16,   0, 
      4,   0,   0,   0,  70,   4, 
     16,   0,   3,   0,   0,   0, 
    134,  13,  16,   0,   6,   0, 
      0,   0,  56,   0,   0,   9, 
    194,   0,  16,   0,   3,   0, 
      0,   0,   6,   4,  16, 128, 
     65,   0,   0,   0,   4,   0, 
      0,   0, 166, 142,  32,   0, 
      0,   0,   0,   0,   9,   0, 
      0,   0,  50,   0,   0,   9, 
     50,   0,  16,   0,   3,   0, 
      0,   0, 230,  10,  16,   0, 
      3,   0,   0,   0,  70,   0, 
     16,   0,   3,   0,   0,   0, 
    230,  10,  16,   0,   4,   0, 
      0,   0,  18,   0,   0,   1, 
     54,   0,   0,   8,  50,   0, 
     16,   0,   3,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
     18,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  21,   0, 
      0,   1,  56,   0,   0,   8, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  86,  21,  16,   0, 
      0,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  10, 242,   0,  16,   0, 
      4,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   6,  16, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0, 166,  26,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
      0,   0,   0,   8, 242,   0, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   4,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,  14,   0,   0,   7, 
    242,   0,  16,   0,   4,   0, 
      0,   0,  70,  14,  16,   0, 
      4,   0,   0,   0, 246,  15, 
     16,   0,   4,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   5,   0,   0,   0, 
     86,   5,  16,   0,   4,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  50,   0,   0,  10, 
    242,   0,  16,   0,   5,   0, 
      0,   0,  70, 142,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   6,   0,  16,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   5,   0,   0,   0, 
     50,   0,   0,  10, 242,   0, 
     16,   0,   5,   0,   0,   0, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
    166,  10,  16,   0,   4,   0, 
      0,   0,  70,  14,  16,   0, 
      5,   0,   0,   0,  50,   0, 
      0,  10, 242,   0,  16,   0, 
      4,   0,   0,   0,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      7,   0,   0,   0, 246,  15, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   5,   0, 
      0,   0,  14,   0,   0,   7, 
     50,   0,  16,   0,   5,   0, 
      0,   0,  70,   0,  16,   0, 
      4,   0,   0,   0, 246,  15, 
     16,   0,   4,   0,   0,   0, 
     50,   0,   0,  15,  50,   0, 
     16,   0,   5,   0,   0,   0, 
     70,   0,  16,   0,   5,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
    128, 191,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,  10,  50,  32, 
     16,   0,   7,   0,   0,   0, 
     70,   0,  16,   0,   5,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,  63,   0,   0, 
      0,  63,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   4,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  30,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     82,  32,  16,   0,   2,   0, 
      0,   0,   6,  16,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5, 226,   0,  16,   0, 
      2,   0,   0,   0,   6,  25, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 242,  32, 
     16,   0,   3,   0,   0,   0, 
    150,   3,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
     66,  32,  16,   0,   7,   0, 
      0,   0,  10,  16,  16,   0, 
      6,   0,   0,   0,  54,   0, 
      0,   5, 114,  32,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 114,  32, 
     16,   0,   5,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  54,   0,   0,   5, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  10,  16,  16,   0, 
      8,   0,   0,   0,  54,   0, 
      0,   5, 114,  32,  16,   0, 
      8,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     99,   0,   0,   0,   7,   0, 
      0,   0,   0,   0,   0,   0, 
     21,   0,   0,   0,  45,   0, 
      0,   0,   4,   0,   0,   0, 
      5,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  22,   0, 
      0,   0,   9,   0,   0,   0, 
     11,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
