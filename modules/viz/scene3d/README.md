# sight::module::viz::scene3d

Contains adaptors available for the generic scene.

## Services

- **SAxis**: shows a simple coordinate system.
- **SCamera**: transforms a Sight camera to an Ogre camera.
- **SCompositorParameter**: binds a Sight data to a shader uniform from a specific compositor.
- **SFragmentsInfo**: takes a snapshot of layer fragments information and output it as a sight::data::Image.
- **SFrustum**: displays the frustum of a sight::data::Camera.
- **SFrustumList**: displays a new Frustum each time the transform is updated.
- **SImageMultiDistances**: displays distances retrieved from the image fields.
- **SLandmarks**: displays landmarks.
- **SLight**: adds a light to the scene manager.
- **SLine**: shows a simple line.
- **SMaterial**: adapts a sight::data::Material, allowing to tweak material parameters.
- **SMesh**: shows individual meshes.
- **SMeshList**: shows a list of mesh.
- **SModelSeries**: shows a modelSeries. It creates an adaptor for each reconstruction in the model.
- **SNegato2D**: displays a 2D negato.
- **SNegato2DCamera**: lets the user move an orthographic camera to visualize medical images in 2D.
- **SNegato3D**: displays a 3D negato.
- **SOrientationMarker**: displays an orientation "marker", marker is represented by a human body mesh, its orientation follows camera movement.
- **SPicker**: performs picking queries on mouse clicks and forwards the picked data through a signal.
- **SPointList**: shows a point list using billboards generated by a geometry shader.
- **SReconstruction**: displays a reconstruction.
- **SRenderStats**: displays rendering statistics in the window overlay.
- **SResizeViewport**: resizes and moves viewports.
- **SShaderParameter**: sends a Sight data as a shader parameter.
- **SShapeExtruder**: allows to draw a 2D shape with a lasso tool, and create an extruded mesh from it.
- **SText**: displays a text object in the center or along the window borders.
- **STexture**: maps a texture on a mesh. This is done via module::viz::scene3d::adaptor::SMaterial.
- **STrackballCamera**: lets the user move the camera around a point of interest using the mouse and keyboard.
- **STransform**: binds a sight::data::Matrix4 to a scene node.
- **SVector**: shows a simple vector.
- **SVideo**: renders a video frame from a 2D-image.
- **SVolumeRender**: displays a volume rendering.
- **SVoxelPicker**: performs picking queries on an image voxel using slices.

## How to use it

### CMake

```cmake
add_dependencies(myTarget module_viz_scene3d ... )
```

### XML

Please consult the [doxygen](https://sight.pages.ircad.fr/sight) of each service to learn more about its use in xml configurations.
