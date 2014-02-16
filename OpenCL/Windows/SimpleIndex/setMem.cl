__kernel void setMem(__global float *buffer, float value, uint numFloats)
{
   if(get_global_id(0) < numFloats)
       buffer[get_global_id(0)] = value;
}