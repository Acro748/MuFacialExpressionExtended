RWStructuredBuffer<float4> DynamicVertices : register(u0);
StructuredBuffer<float4> MorphVertices : register(t0);

cbuffer Constants : register(b0) {
    float multiplier;
    uint vertexCount;
    
    float _padding[2];
};

[numthreads(64, 1, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    if (dispatchThreadID.x >= vertexCount)
        return;
    DynamicVertices[dispatchThreadID.x].xyz += MorphVertices[dispatchThreadID.x].xyz * multiplier;
}
