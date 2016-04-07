uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

// New bumpmapping
varying vec3 lightVec;
varying vec3 halfVec;
varying vec3 eyeVec;
varying vec3 pos;

void main() {

    // lookup normal from normal map, move from [0,1] to  [-1, 1] range, normalize
    vec3 normal = 2.0 * texture2D(normalTexture, gl_TexCoord[0].st).rgb - 1.0;
    normal = normalize(normal);

    // compute diffuse lighting
    float lamberFactor = max(dot(lightVec, normal), 0.0);
    vec4 diffuseMaterial = texture2D(diffuseTexture, gl_TexCoord[0].st);
    vec4 diffuseLight = gl_LightSource[0].diffuse;

    // compute specular lighting
    vec4 specularMaterial = vec4(1.0);
    vec4 specularLight = gl_LightSource[0].specular;
    float shininess = 0.001;

    // compute ambient
    vec4 ambientLight = gl_LightSource[0].ambient;

    vec4 final_effect = diffuseMaterial * diffuseLight * lamberFactor;
    final_effect += specularMaterial * specularLight * shininess;
    // final_effect += ambientLight;

    // Apply alpha by distance
    float alpha_distance = sqrt(pos.z * pos.z + pos.x * pos.x + pos.y * pos.y);
    float min_dist = 50;
    float max_dist = 100;
    alpha_distance = min(max(alpha_distance, min_dist), max_dist);
    alpha_distance = (max_dist - alpha_distance) / (max_dist - min_dist);

    gl_FragColor = vec4(final_effect.xyz, alpha_distance);

//    gl_FragColor = diffuseMaterial * diffuseLight * lamberFactor;
//    gl_FragColor += specularMaterial * specularLight * shininess;
//    gl_FragColor += ambientLight;

}
