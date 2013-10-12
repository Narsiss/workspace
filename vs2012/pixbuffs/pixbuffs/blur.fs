#version 150

in vec2 vTexCoord;
uniform sample2D textureUnit0;
uniform sample2D textureUnit1;
uniform sample2D textureUnit2;
uniform sample2D textureUnit3;
uniform sample2D textureUnit4;
uniform sample2D textureUnit5;

void main()
{
    vec4 blur0 = texture(textureUnit0,vTexCoord);
    vec4 blur1 = texture(textureUnit1,vTexCoord);
    vec4 blur2 = texture(textureUnit2,vTexCoord);
    vec4 blur3 = texture(textureUnit3,vTexCoord);
    vec4 blur4 = texture(textureUnit4,vTexCoord);
    vec4 blur5 = texture(textureUnit5,vTexCoord);
    
    vec4 blur = blur0 + blur1 + blur2 + blur3 + blur4 + blur5;
    gl_FragColor = blur/6;
}