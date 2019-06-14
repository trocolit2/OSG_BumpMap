# OSG BumpMap

Adds texture to .osg models

## Getting Started

Create one build folder, compile your package, set the file path inside the main.cpp and then run the main file

It requires 4 arguments, were they are:
```
$ ./build/main operation child texture_number scale_x scale y
```
There are 3 avaiable operations

- ```r``` - clean all the geodes who not have a drawable (geometry)
- ```p``` - Preview of the child with the desired texture
- ```f``` - Preview of the child in the full model with the desired texture
- ```t``` - Applies the texture

After applying your texture, clean the geodes in order to reduce the model size
### Prerequisites

OpenSceneGraph required libraries:
```
osgDB osgUtil osgViewer osgFX osgGA
```

### Installing

Install the OpenSceneGraph :

```
sudo apt install openscenegraph
```

### Avaible textures

The repository contains already some textures, in order to add new ones it's necessary to include the normal map and the specular, it's up to the user to change the texture options in the main code

```
Give an example
```

## Built With

* [OpenSceneGraph](http://www.openscenegraph.org/) - 3D View Tool

## Authors

* **Tiago Trocol** - *Initial work* - [PurpleBooth](https://github.com/PurpleBooth)
* **Cleber Couto Filho** - *Repository organization* - [clebercoutof](https://github.com/clebercoutof)


## Acknowledgments

This script was used to apply texture to simulation models, drawn on blender and converted from .obj to .osg. They were used together with camera algorithms and Gazebo simulator.

