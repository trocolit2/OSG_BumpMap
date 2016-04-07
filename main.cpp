#include <osg/Group>
#include <osg/Image>
#include <osg/MatrixTransform>
#include <osg/Node>
#include <osg/StateSet>
#include <osg/TexMat>
#include <osg/TexEnv>
#include <osg/Texture2D>
#include <osg/ShapeDrawable>
#include <osg/Program>
#include <osg/Shader>
#include <osg/Uniform>

#include <osgFX/BumpMapping>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/FileUtils>

#include <osgGA/TrackballManipulator>
#include <osgViewer/Viewer>

#include <iostream>
#include <string>

bool writeOSGFileFromNode(osg::Node &node, std::string out_file = "out.osg") {

    return osgDB::writeNodeFile(node, out_file);
}

osg::Geometry* createSquare(float textureCoordMax = 1.0f) {
    // set up the Geometry.
    osg::Geometry* geom = new osg::Geometry;

    osg::Vec3Array* coords = new osg::Vec3Array(4);
    (*coords)[0].set(-1.0f, 0.0f, 1.0f);
    (*coords)[1].set(-1.0f, 0.0f, -1.0f);
    (*coords)[2].set(1.0f, 0.0f, -1.0f);
    (*coords)[3].set(1.0f, 0.0f, 1.0f);
    geom->setVertexArray(coords);

    osg::Vec3Array* norms = new osg::Vec3Array(1);
    (*norms)[0].set(0.0f, -1.0f, 0.0f);
    geom->setNormalArray(norms, osg::Array::BIND_OVERALL);

    osg::Vec2Array* tcoords = new osg::Vec2Array(4);
    (*tcoords)[0].set(0.0f, 0.0f);
    (*tcoords)[1].set(0.0f, textureCoordMax);
    (*tcoords)[2].set(textureCoordMax, textureCoordMax);
    (*tcoords)[3].set(textureCoordMax, 0.0f);
    geom->setTexCoordArray(0, tcoords);

    geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4));

    return geom;
}

osg::ref_ptr<osg::StateSet> bumpMapShader(osg::Image *color_image, osg::Image *normal_image, osg::Image *specular_image, double scale_x,
        double scale_y) {

    if (!normal_image) {
        std::cout << "NORMAL IMAGE FAIL" << std::endl;
        exit(0);
    }

    if (!color_image) {
        std::cout << "COLOR IMAGE FAIL" << std::endl;
        exit(0);
    }

    if (!specular_image) {
        std::cout << "SPECULAR IMAGE FAIL" << std::endl;
        exit(0);
    }

    osg::StateSet* bumpState = new osg::StateSet();

    // Stateset shader program
    osg::Program* bumpMapProgramObject = new osg::Program;

    // Set shader
    osg::Shader* bumpVertexObject = new osg::Shader(osg::Shader::VERTEX);
    osg::Shader* bumpFragmentObject = new osg::Shader(osg::Shader::FRAGMENT);

    std::string bumpVertexFileName = osgDB::findDataFile("bump_map.vert");
    bumpVertexObject->loadShaderSourceFromFile(bumpVertexFileName.c_str());
    std::string bumpFragmentFileName = osgDB::findDataFile("bump_map.frag");
    bumpFragmentObject->loadShaderSourceFromFile(bumpFragmentFileName.c_str());

    bumpMapProgramObject->addShader(bumpVertexObject);
    bumpMapProgramObject->addShader(bumpFragmentObject);

    // Set textures
    osg::Texture2D *normal = new osg::Texture2D();
    osg::Texture2D *color = new osg::Texture2D();
    osg::Texture2D *specular = new osg::Texture2D();

    color->setImage(color_image);
    color->setDataVariance(osg::Object::DYNAMIC);
    color->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    color->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    color->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    color->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    color->setResizeNonPowerOfTwoHint(false);
    color->setMaxAnisotropy(8.0f);

    normal->setImage(normal_image);
    normal->setDataVariance(osg::Object::DYNAMIC);
    normal->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    normal->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    normal->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    normal->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    normal->setResizeNonPowerOfTwoHint(false);
    normal->setMaxAnisotropy(8.0f);

    specular->setImage(specular_image);
    specular->setDataVariance(osg::Object::DYNAMIC);
    specular->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    specular->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    specular->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    specular->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    specular->setResizeNonPowerOfTwoHint(false);
    specular->setMaxAnisotropy(8.0f);

    // Set texture matrix
    osg::TexMat* texMat = new osg::TexMat;
    osg::Vec3d texScale(scale_x, scale_y, 1.0);
    texMat->setMatrix(osg::Matrix::scale(texScale));

    osg::TexEnv* tex_env = new osg::TexEnv;
    tex_env->setMode(osg::TexEnv::REPLACE);
    tex_env->setColor(osg::Vec4(1.0f, 1.0f, 1.0f, 0.0f));

    const int TEXTURE_UNIT_SPECULAR = 2;
    const int TEXTURE_UNIT_NORMAL = 1;
    const int TEXTURE_UNIT_COLOR = 0;

//    bumpState->setTextureAttribute(TEXTURE_UNIT_COLOR, tex_env, osg::StateAttribute::ON);
//    bumpState->setTextureAttribute(TEXTURE_UNIT_NORMAL, tex_env, osg::StateAttribute::ON);

    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_COLOR, color, osg::StateAttribute::ON);
    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_COLOR, texMat, osg::StateAttribute::ON);

    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_NORMAL, normal, osg::StateAttribute::ON);
    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_NORMAL, texMat, osg::StateAttribute::ON);

    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_SPECULAR, specular, osg::StateAttribute::ON);
    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_SPECULAR, texMat, osg::StateAttribute::ON);

    bumpState->addUniform(new osg::Uniform("normalTexture", TEXTURE_UNIT_NORMAL));
    bumpState->addUniform(new osg::Uniform("diffuseTexture", TEXTURE_UNIT_COLOR));
    bumpState->addUniform(new osg::Uniform("specularTexture", TEXTURE_UNIT_SPECULAR));
    bumpState->setAttributeAndModes(bumpMapProgramObject, osg::StateAttribute::ON);

    return bumpState;
}

osgFX::BumpMapping* bumpMapOSG(osg::Geode *geode, osg::Image *normal_image, osg::Image *difuse_image, double scale_x, double scale_y) {

    if (!normal_image || !difuse_image) {
        std::cout << "IMAGE FAIL" << std::endl;
        exit(0);
    }

    osg::StateSet* bumpState = new osg::StateSet();

    // Set textures
    osg::ref_ptr<osg::Texture2D> normal_texture(new osg::Texture2D());
    osg::ref_ptr<osg::Texture2D> difuse_texture(new osg::Texture2D());

    normal_texture->setImage(normal_image);
    normal_texture->setDataVariance(osg::Object::DYNAMIC);
    normal_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    normal_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    normal_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    normal_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    normal_texture->setResizeNonPowerOfTwoHint(false);
    normal_texture->setMaxAnisotropy(8.0f);

    difuse_texture->setImage(difuse_image);
    difuse_texture->setDataVariance(osg::Object::DYNAMIC);
    difuse_texture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
    difuse_texture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);
    difuse_texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
    difuse_texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
    difuse_texture->setResizeNonPowerOfTwoHint(false);
    difuse_texture->setMaxAnisotropy(8.0f);

    const int TEXTURE_UNIT_NORMAL = 1;
    const int TEXTURE_UNIT_DIFUSE = 2;

    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_NORMAL, normal_texture, osg::StateAttribute::ON);
    bumpState->setTextureAttributeAndModes(TEXTURE_UNIT_DIFUSE, difuse_texture, osg::StateAttribute::ON);

    osg::ref_ptr<osg::Geometry> geometry = geode->asGeode()->getDrawable(0)->asGeometry();
    osg::Vec2Array* tex_coord = dynamic_cast<osg::Vec2Array*>(geometry->getTexCoordArray(0));

    for (unsigned int i = 0; i < tex_coord->getNumElements(); ++i)
        (*tex_coord)[i].set((*tex_coord)[i].x() * scale_x, (*tex_coord)[i].y() * scale_y);

    geometry->setStateSet(bumpState);
    if (tex_coord) {
        geometry->setTexCoordArray(TEXTURE_UNIT_NORMAL, tex_coord);
        geometry->setTexCoordArray(TEXTURE_UNIT_DIFUSE, tex_coord);
    } else {
        std::cout << "MISS TEXTURE COORDINATE " << std::endl;
        exit(0);
    }

    osgFX::BumpMapping* bump_mapping = new osgFX::BumpMapping();
    bump_mapping->setEnabled(true);
    bump_mapping->setLightNumber(0);
    bump_mapping->setNormalMapTextureUnit(TEXTURE_UNIT_NORMAL);
    bump_mapping->setDiffuseTextureUnit(TEXTURE_UNIT_DIFUSE);
    bump_mapping->addChild(geode);
    bump_mapping->prepareChildren();

    return bump_mapping;
}

void selectTexture(int texture, std::string *normal_path, std::string *difuse_path, std::string *specular_path) {

    std::string path = "textures/";

    std::string texture_type;
    switch (texture) {
    case 0:
        texture_type = "gray_texture";
        break;
    case 1:
        texture_type = "yellow_texture";
        break;
    case 2:
        texture_type = "sand_texture";
        break;
    case 3:
        texture_type = "wood_texture";
        break;
    case 4:
        texture_type = "red_texture";
        break;
    case 5:
        texture_type = "concrete";
        break;
    case 6:
        texture_type = "black_texture";
        break;
    case 7:
        texture_type = "corrosion_texture";
        break;
    case 8:
        texture_type = "metal_corrosion";
        break;
    case 9:
        texture_type = "pedras_texture";
        break;
    default:
        exit(0);
        break;
    }

    (*normal_path) = path + texture_type + "_n.jpg";
    (*difuse_path) = path + texture_type + "_d.jpg";
    (*specular_path) = path + texture_type + "_s.jpg";

//    (*normal_path) = path + texture_type + "_n.png";
//    (*difuse_path) = path + texture_type + "_d.png";

}

int main(int argc, char **argv) {
//    int bla(int argc, char **argv) {

// LOCAL TESTES SHADER BUMP MAP
    osg::Group* bumpRoot = new osg::Group();

    // Model with bump mapping
    osg::Geode* bumpModel = new osg::Geode;
    bumpModel->addDrawable(createSquare());

    std::string normal_path, difuse_path, specular_path;
    selectTexture(9, &normal_path, &difuse_path, &specular_path);

    osg::ref_ptr<osg::Image> difuse_image = osgDB::readImageFile(difuse_path);
    osg::ref_ptr<osg::Image> normal_image = osgDB::readImageFile(normal_path);
    osg::ref_ptr<osg::Image> specular_image = osgDB::readImageFile(specular_path);

    bumpModel->getDrawable(0)->asGeometry()->setStateSet(bumpMapShader(difuse_image, normal_image, specular_image, 1, 1));
    bumpRoot->addChild(bumpModel);

    osgViewer::Viewer bumpViewer;
    bumpViewer.setSceneData(bumpRoot);
    bumpViewer.setCameraManipulator(new osgGA::TrackballManipulator());
    bumpViewer.run();

//================================================================

//    // LOCAL TESTES BUMP MAP
//    osg::Group* bumpRoot = new osg::Group();
//
//// Model with bump mapping
//    osg::Geode* bumpModel = new osg::Geode;
//    bumpModel->addDrawable(createSquare());
//
//    std::string color_path, normal_path, difuse_path;
//    selectTexture(8, &normal_path, &difuse_path);
//
//    osg::ref_ptr<osg::Image> difuse_image = osgDB::readImageFile(difuse_path);
//    osg::ref_ptr<osg::Image> normal_image = osgDB::readImageFile(normal_path);
//
//    bumpRoot->addChild(bumpMapOSG(bumpModel, normal_image, difuse_image, 1, 1));
//
//    osgViewer::Viewer bumpViewer;
//    bumpViewer.setSceneData(bumpRoot);
//    bumpViewer.setCameraManipulator(new osgGA::TrackballManipulator());
//    bumpViewer.run();

//=====================================================================================
//    EXTERNAL MODELS
//    osg::ref_ptr<osg::Group> original_group = (osg::Group*) osgDB::readNodeFile(
//            "/home/tiagotrocoli/flat_fish/dev/bundles/flat_fish/models/sdf/oil_rig_manifold/old/visual_wellhead_1.3ds");
//
//    osg::ref_ptr<osg::Group> bumpRoot = (osg::Group*) osgDB::readNodeFile("out.osg");
//    if (!bumpRoot)
//        bumpRoot = new osg::Group();
//
//    int total = original_group->getNumChildren();
//
//    int i = atoi(argv[2]);
//
//    osg::ref_ptr<osg::Geode> temp_geode = (osg::Geode*) original_group->getChild(i);
//    std::cout << "INDEX = " << i;
//    std::cout << " TOTAL = " << total;
//    std::cout << "  % = " << i * 1.0 / total << std::endl;
//    osg::ref_ptr<osg::Node> temp_node;
//
//    if (argv[1][0] == 's') {
//        temp_node = temp_geode;
//
//    } else {
//
//        int texture = atoi(argv[3]);
//        double scale_x = atof(argv[4]);
//        double scale_y = atof(argv[5]);
//
//        std::string normal_path, difuse_path;
//        selectTexture(texture, &normal_path, &difuse_path);
//        osg::ref_ptr<osg::Image> difuse_image = osgDB::readImageFile(difuse_path);
//        osg::ref_ptr<osg::Image> normal_image = osgDB::readImageFile(normal_path);
//
//        temp_geode->getDrawable(0)->asGeometry()->setStateSet(bumpMapShader(difuse_image, normal_image, scale_x, scale_y));
//        temp_node = temp_geode;
//
////        temp_node = bumpMapOSG(temp_geode, normal_image, difuse_image, scale_x, scale_y);
//    }
//
//    bool setted = bumpRoot->setChild(i, temp_node);
//    if (!setted) {
//        std::cout << "ADD NODE" << std::endl;
//        bumpRoot->addChild(temp_node);
//    }
//
//    if (argv[5]) {
//        osgViewer::Viewer bumpViewer;
//        bumpViewer.setSceneData(temp_node);
//        bumpViewer.setCameraManipulator(new osgGA::TrackballManipulator());
//        bumpViewer.run();
//    }
// write
    if (writeOSGFileFromNode((*bumpRoot)))
        std::cout << " --> FILE WROTE!!" << std::endl;

    return 0;
}

