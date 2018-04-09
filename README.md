# SpmdParticle

An Unity plugin for render massive particle,Accelerated by Intel SSE.

SpmdParticle only use GPU to render particle.All physical calculations completed on the CPU

------

### Why Use CPU Accelerated Particle

Becuase CPU particle will have easier to control more flexible. For Example particle self interaction. More suitable for massive if branch.

------

### SPMD Tutorial

The intel® ®SPMD program compiler based on llvm* (commonly referred to as ISPC in previous documents) is not a substitute for gnu* compiler Suite (GCC) or microsoft* C + + compiler; it's more like the C- A PU shader compiler that generates vector directives for a variety of instruction sets, such as the intel® ®SIMD streaming instruction Extension 2 (intel® ®sse2), the intel® ®simd streaming instruction Extension 4 (Intel ®sse4), the intel® Advanced Vector Extensions instruction set (Intel ®AVX), Intel ®avx2, and so on 。 Enter a shader or kernel based on C to output the precompiled object file, and your application will contain a header file. By using a small number of keywords, the compiler can get explicit instructions on how to assign work on a CPU vector unit.

> https://software.intel.com/zh-cn/articles/use-the-intel-spmd-program-compiler-for-cpu-vectorization-in-games